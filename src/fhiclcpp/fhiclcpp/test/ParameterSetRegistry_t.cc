// vim: set sw=2 expandtab :
#define BOOST_TEST_MODULE (ParameterSetRegistry_t)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/SimultaneousFunctionSpawner.h"
#include "cetlib/container_algorithms.h"
#include "fhiclcpp/ParameterSetRegistry.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/test/boost_test_print_pset.h"
#include "hep_concurrency/RecursiveMutex.h"

#include "sqlite3.h"

#include <atomic>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace fhicl;
using namespace hep::concurrency;
using namespace std;
using namespace string_literals;

using fhicl::detail::throwOnSQLiteFailure;

BOOST_AUTO_TEST_SUITE(ParameterSetRegistry_t)

BOOST_AUTO_TEST_CASE(Typedefs)
{
  using ctype =
    unordered_map<ParameterSetID, ParameterSet, detail::HashParameterSetID>;
  static_assert(is_same_v<ctype, ParameterSetRegistry::collection_type>);
  static_assert(is_same_v<ParameterSetID, ParameterSetRegistry::key_type>);
  static_assert(is_same_v<ParameterSet, ParameterSetRegistry::mapped_type>);
  static_assert(is_same_v<pair<ParameterSetID const, ParameterSet>,
                          ParameterSetRegistry::value_type>);
  static_assert(is_same_v<size_t, ParameterSetRegistry::size_type>);
  static_assert(
    is_same_v<ctype::const_iterator, ParameterSetRegistry::const_iterator>);
}

BOOST_AUTO_TEST_CASE(MakeAndAdd)
{
  BOOST_TEST_REQUIRE(ParameterSetRegistry::empty());
  BOOST_TEST_REQUIRE(ParameterSetRegistry::size() == 0ul);
  ParameterSet pset;
  make_ParameterSet(
    "x: 5 y: { a: \"oops\" b: 9 } z: { c: \"Looooong striiiiiing.\" }", pset);
  BOOST_TEST_REQUIRE(!ParameterSetRegistry::empty());
  BOOST_TEST_REQUIRE(ParameterSetRegistry::size() == 2ul);
  ParameterSetRegistry::put(pset);
  BOOST_TEST_REQUIRE(ParameterSetRegistry::size() == 3ul);
  auto ps2 = ParameterSetRegistry::get(pset.id());
  BOOST_TEST_REQUIRE(pset.id() == ps2.id());
  BOOST_TEST_REQUIRE(pset == ps2);
}

BOOST_AUTO_TEST_CASE(AddFromIterAndGet)
{
  vector<ParameterSet> v1;
  auto expected_size = ParameterSetRegistry::size() + 2ul; // a3, x.
  for (auto s :
       {"a1: 4 b1: 7.0 c1: [ 5, 4, 3 ]",
        "a2: [ oh, my, stars ]",
        "a3: { x: { y: [ now, is, the, time, for, all, good, men ] } }"}) {
    ParameterSet pset;
    make_ParameterSet(s, pset);
    v1.emplace_back(pset);
  }
  BOOST_TEST_REQUIRE(ParameterSetRegistry::size() == expected_size);
  vector<ParameterSetRegistry::value_type> v2;
  string const f{"filler"};
  for (auto p : v1) {
    p.put("f", f);
    v2.emplace_back(p.id(), move(p));
  }
  expected_size += 3;
  ParameterSetRegistry::put(v1.cbegin(), v1.cend());
  BOOST_TEST_REQUIRE(ParameterSetRegistry::size() == expected_size);
  expected_size += 3;
  ParameterSetRegistry::put(v2.cbegin(), v2.cend());
  BOOST_TEST_REQUIRE(ParameterSetRegistry::size() == expected_size);
  ParameterSet pset;
  ParameterSetRegistry::get(v2.front().first, pset);
  BOOST_TEST_REQUIRE(pset == v2.front().second);
  auto p2 = ParameterSetRegistry::get(v2.back().first);
  BOOST_TEST_REQUIRE(p2 == v2.back().second);
}

BOOST_AUTO_TEST_CASE(TestImport)
{
  atomic<size_t> expected_size{ParameterSetRegistry::size()};
  sqlite3* db = nullptr;
  BOOST_TEST_REQUIRE(!sqlite3_open(":memory:", &db));
  throwOnSQLiteFailure(db);
  char* errMsg = nullptr;
  sqlite3_exec(db,
               "BEGIN TRANSACTION; DROP TABLE IF EXISTS ParameterSets;"
               "CREATE TABLE ParameterSets(ID PRIMARY KEY, PSetBlob); COMMIT;",
               nullptr,
               nullptr,
               &errMsg);
  throwOnSQLiteFailure(db, errMsg);
  // testData: bool represents whether the top-level set is already
  // expected to be in the registry or not.
  vector<pair<string, bool>> testData{
    {"x: [ 1, 3, 5, 7 ]", false},
    {"a2: [ oh, my, stars ]", true}, // Should match a2, above.
    {"y1: \"Oh, home on the range\" y2: \"Where the deer and the antelope "
     "roam\"",
     false}};
  vector<pair<ParameterSet, bool>> v1(testData.size());
  // Make ParameterSets in parallel
  {
    vector<function<void()>> tasks;
    cet::for_all_with_index(testData,
                            [&v1, &tasks](size_t const i, auto const& p) {
                              auto& entry = v1[i];
                              tasks.push_back([&entry, &p] {
                                ParameterSet pset;
                                make_ParameterSet(p.first, pset);
                                entry = make_pair(pset, p.second);
                              });
                            });
    cet::SimultaneousFunctionSpawner sfs{tasks};
  }
  // Insert ParameterSets into db in parallel
  {
    RecursiveMutex m{"ParameterSetRegistry_t::m"};
    auto insert_into_db = [&db, &m](auto const& pr) {
      // Since this lambda is intended to be executed in parallel, one
      // should not specify a BEGIN (IMMEDIATE|EXCLUSIVE) TRANSACTION
      // statement--when executed in parallel, two or more BEGIN
      // statements could be executed in succession, which is an
      // error.  Rather, one should rely on the SQLite's internal
      // locking mechanisms to ensure an appropriate transaction.
      // Note that SQLite implicitly assumes a transaction is in
      // flight until an sqlite3_reset or sqlite3_finalize function
      // call is made, at which point it is committed.
      sqlite3_stmt* oStmt = nullptr;
      sqlite3_prepare_v2(
        db,
        "INSERT INTO ParameterSets(ID, PSetBlob) VALUES(?, ?);",
        -1,
        &oStmt,
        nullptr);
      throwOnSQLiteFailure(db);
      auto const& pset = pr.first;
      bool const inRegistry{pr.second};
      string const id{pset.id().to_string()};
      auto const rc1 =
        sqlite3_bind_text(oStmt, 1, id.c_str(), id.size() + 1, SQLITE_STATIC);
      throwOnSQLiteFailure(rc1);

      string const psBlob{pset.to_compact_string()};
      auto const rc2 = sqlite3_bind_text(
        oStmt, 2, psBlob.c_str(), psBlob.size() + 1, SQLITE_STATIC);
      throwOnSQLiteFailure(rc2);
      {
        // FIXME: sqlite3_step is not thread-safe according to tsan!
        RecursiveMutexSentry sentry{m, "test"};
        assert(sqlite3_step(oStmt) == SQLITE_DONE);
      }
      auto const rc3 = sqlite3_finalize(oStmt);
      throwOnSQLiteFailure(rc3);

      assert(ParameterSetRegistry::has(pset.id()) == inRegistry);
    };
    vector<function<void()>> tasks;
    cet::transform_all(
      v1, back_inserter(tasks), [insert_into_db](auto const& pr) {
        return [insert_into_db, pr] { insert_into_db(pr); };
      });
    cet::SimultaneousFunctionSpawner sfs{tasks};
  }
  BOOST_TEST_REQUIRE(ParameterSetRegistry::size() == expected_size);
  ParameterSetRegistry::importFrom(db);
  // Make sure the registry didn't expand as a result of the insert.
  BOOST_TEST_REQUIRE(ParameterSetRegistry::size() == expected_size);
  BOOST_TEST_REQUIRE(sqlite3_close(db) == SQLITE_OK);
  // Read from registry in parallel
  {
    RecursiveMutex m{"ParameterSetRegistry_t::m"};
    auto read_from_registry = [&expected_size, &m](auto const& p) {
      auto const& id = p.first.id();
      if (p.second) {
        {
          // FIXME: Not thread-safe according to tsan!
          RecursiveMutexSentry sentry{m, "test"};
          // Should be in registry already.
          BOOST_TEST_REQUIRE(ParameterSetRegistry::has(id));
        }
      } else {
        {
          // FIXME: Not thread-safe according to tsan!
          RecursiveMutexSentry sentry{m, "test"};
          // Make sure the import didn't inject them into the registry.
          BOOST_TEST_REQUIRE(!ParameterSetRegistry::has(id));
        }
        // We expect the get() call below to increase the size of the
        // registry by pulling the entry in from the backing DB.
        ++expected_size;
      }
      {
        // FIXME: Not thread-safe according to tsan!
        RecursiveMutexSentry sentry{m, "test"};
        auto const& p2 = ParameterSetRegistry::get(id);
        BOOST_TEST_REQUIRE(p2 == p.first);
      }
    };
    vector<function<void()>> tasks;
    cet::transform_all(
      v1, back_inserter(tasks), [read_from_registry](auto const& p) {
        return [read_from_registry, p] { read_from_registry(p); };
      });
    cet::SimultaneousFunctionSpawner sfs{tasks};
    BOOST_TEST_REQUIRE(ParameterSetRegistry::size() == expected_size);
  }
}

BOOST_AUTO_TEST_CASE(TestExport)
{
  sqlite3* db = nullptr;
  BOOST_TEST_REQUIRE(!sqlite3_open(":memory:", &db));
  // Check empty!
  sqlite3_stmt* stmt = nullptr;
  // Make sure we get our own fresh and empty DB.
  sqlite3_prepare_v2(
    db,
    "SELECT 1 from sqlite_master where type='table' and name='ParameterSets';",
    -1,
    &stmt,
    nullptr);
  BOOST_TEST_REQUIRE(sqlite3_step(stmt) == SQLITE_DONE); // No rows.
  sqlite3_reset(stmt);
  ParameterSetRegistry::exportTo(db);
  BOOST_TEST_REQUIRE(sqlite3_step(stmt) == SQLITE_ROW); // Found table.
  sqlite3_finalize(stmt);
  sqlite3_prepare_v2(
    db, "SELECT COUNT(*) from ParameterSets;", -1, &stmt, nullptr);
  BOOST_TEST_REQUIRE(sqlite3_step(stmt) == SQLITE_ROW);
  BOOST_TEST_REQUIRE(sqlite3_column_int64(stmt, 0) == 13l);
  sqlite3_finalize(stmt);
  sqlite3_close(db);
}

BOOST_AUTO_TEST_SUITE_END()

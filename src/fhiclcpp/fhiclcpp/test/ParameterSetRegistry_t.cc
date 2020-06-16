#define BOOST_TEST_MODULE (ParameterSetRegistry_t)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/SimultaneousFunctionSpawner.h"
#include "cetlib/container_algorithms.h"
#include "fhiclcpp/ParameterSetRegistry.h"
#include "fhiclcpp/make_ParameterSet.h"

#include "sqlite3.h"

using namespace fhicl;

using fhicl::detail::throwOnSQLiteFailure;

BOOST_AUTO_TEST_SUITE(ParameterSetRegistry_t)

BOOST_AUTO_TEST_CASE(Typedefs)
{
  using ctype = std::
    unordered_map<ParameterSetID, ParameterSet, detail::HashParameterSetID>;
  static_assert(
    std::is_same<ctype, ParameterSetRegistry::collection_type>::value,
    "collection_type failure");
  static_assert(
    std::is_same<ParameterSetID, ParameterSetRegistry::key_type>::value,
    "key_type failure");
  static_assert(
    std::is_same<ParameterSet, ParameterSetRegistry::mapped_type>::value,
    "mapped_type failure");
  static_assert(std::is_same<std::pair<ParameterSetID const, ParameterSet>,
                             ParameterSetRegistry::value_type>::value,
                "value_type failure");
  static_assert(
    std::is_same<std::size_t, ParameterSetRegistry::size_type>::value,
    "size_type failure");
  static_assert(std::is_same<ctype::const_iterator,
                             ParameterSetRegistry::const_iterator>::value,
                "const_iterator failure");
}

BOOST_AUTO_TEST_CASE(MakeAndAdd)
{
  BOOST_REQUIRE(ParameterSetRegistry::empty());
  BOOST_REQUIRE_EQUAL(ParameterSetRegistry::size(), 0ul);
  ParameterSet pset;
  make_ParameterSet(
    "x: 5 y: { a: \"oops\" b: 9 } z: { c: \"Looooong striiiiiing.\" }", pset);
  BOOST_REQUIRE(!ParameterSetRegistry::empty());
  BOOST_REQUIRE_EQUAL(ParameterSetRegistry::size(), 2ul);
  ParameterSetRegistry::put(pset);
  BOOST_REQUIRE_EQUAL(ParameterSetRegistry::size(), 3ul);
  auto ps2 = ParameterSetRegistry::get(pset.id());
  BOOST_REQUIRE_EQUAL(pset.id(), ps2.id());
  BOOST_REQUIRE(pset == ps2);
}

BOOST_AUTO_TEST_CASE(AddFromIterAndGet)
{
  std::vector<ParameterSet> v1;
  auto expected_size = ParameterSetRegistry::size() + 2ul; // a3, x.
  for (auto s :
       {"a1: 4 b1: 7.0 c1: [ 5, 4, 3 ]",
        "a2: [ oh, my, stars ]",
        "a3: { x: { y: [ now, is, the, time, for, all, good, men ] } }"}) {
    ParameterSet pset;
    make_ParameterSet(s, pset);
    v1.emplace_back(pset);
  }
  BOOST_REQUIRE_EQUAL(ParameterSetRegistry::size(), expected_size);
  std::vector<ParameterSetRegistry::value_type> v2;
  std::string const f{"filler"};
  for (auto p : v1) {
    p.put("f", f);
    v2.emplace_back(p.id(), std::move(p));
  }
  expected_size += 3;
  ParameterSetRegistry::put(v1.cbegin(), v1.cend());
  BOOST_REQUIRE_EQUAL(ParameterSetRegistry::size(), expected_size);
  expected_size += 3;
  ParameterSetRegistry::put(v2.cbegin(), v2.cend());
  BOOST_REQUIRE_EQUAL(ParameterSetRegistry::size(), expected_size);

  ParameterSet pset;
  ParameterSetRegistry::get(v2.front().first, pset);
  BOOST_REQUIRE(pset == v2.front().second);
  auto p2 = ParameterSetRegistry::get(v2.back().first);
  BOOST_REQUIRE(p2 == v2.back().second);
}

BOOST_AUTO_TEST_CASE(TestImport)
{
  std::atomic<std::size_t> expected_size{ParameterSetRegistry::size()};
  sqlite3* db = nullptr;
  BOOST_REQUIRE_EQUAL(sqlite3_open_v2(":memory:", &db,
                       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY |
                       SQLITE_OPEN_FULLMUTEX, nullptr),
                      SQLITE_OK );
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
  std::vector<std::pair<std::string, bool>> testData{
    {"x: [ 1, 3, 5, 7 ]", false},
    {"a2: [ oh, my, stars ]", true}, // Should match a2, above.
    {"y1: \"Oh, home on the range\" y2: \"Where the deer and the antelope "
     "roam\"",
     false}};
  std::vector<std::pair<ParameterSet, bool>> v1(testData.size());

  // Make ParameterSets in parallel
  {
    std::vector<std::function<void()>> tasks;
    cet::for_all_with_index(testData,
                            [&v1, &tasks](std::size_t const i, auto const& p) {
                              auto& entry = v1[i];
                              tasks.push_back([&entry, &p] {
                                ParameterSet pset;
                                make_ParameterSet(p.first, pset);
                                entry = std::make_pair(pset, p.second);
                              });
                            });
    cet::SimultaneousFunctionSpawner sfs{tasks};
  }

  // Insert ParameterSets into db in parallel
  {
    auto insert_into_db = [&db](auto const& pr) {

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
      std::string const id{pset.id().to_string()};
      std::string const psBlob{pset.to_compact_string()};
      sqlite3_bind_text(oStmt, 1, id.c_str(), id.size() + 1, SQLITE_STATIC);
      throwOnSQLiteFailure(db);

      sqlite3_bind_text(
        oStmt, 2, psBlob.c_str(), psBlob.size() + 1, SQLITE_STATIC);
      throwOnSQLiteFailure(db);
      BOOST_REQUIRE_EQUAL(sqlite3_step(oStmt), SQLITE_DONE);

      sqlite3_finalize(oStmt);
      throwOnSQLiteFailure(db);

      BOOST_REQUIRE_EQUAL(ParameterSetRegistry::has(pset.id()), inRegistry);
    };

    std::vector<std::function<void()>> tasks;
    cet::transform_all(
      v1, std::back_inserter(tasks), [insert_into_db](auto const& pr) {
        return [insert_into_db, pr] { insert_into_db(pr); };
      });
    cet::SimultaneousFunctionSpawner sfs{tasks};
  }

  BOOST_REQUIRE_EQUAL(ParameterSetRegistry::size(), expected_size);
  ParameterSetRegistry::importFrom(db);
  // Make sure the registry didn't expand as a result of the insert.
  BOOST_REQUIRE_EQUAL(ParameterSetRegistry::size(), expected_size);
  BOOST_REQUIRE_EQUAL(sqlite3_close(db), SQLITE_OK);

  // Read from registry in parallel
  {
    auto read_from_registry = [&expected_size](auto const& p) {
      auto const& id = p.first.id();
      if (p.second) {
        // Should be in registry already.
        BOOST_REQUIRE(ParameterSetRegistry::has(id));
      } else {
        // Make sure the import didn't inject them into the registry.
        BOOST_REQUIRE(!ParameterSetRegistry::has(id));
        // We expect the get() call below to increase the size of the
        // registry by pulling the entry in from the backing DB.
        ++expected_size;
      }
      auto const& p2 = ParameterSetRegistry::get(id);
      BOOST_REQUIRE(p2 == p.first);
    };

    std::vector<std::function<void()>> tasks;
    cet::transform_all(
      v1, std::back_inserter(tasks), [read_from_registry](auto const& p) {
        return [read_from_registry, p] { read_from_registry(p); };
      });
    cet::SimultaneousFunctionSpawner sfs{tasks};
    BOOST_REQUIRE_EQUAL(ParameterSetRegistry::size(), expected_size);
  }
}

BOOST_AUTO_TEST_CASE(TestExport)
{
  sqlite3* db = nullptr;
  BOOST_REQUIRE_EQUAL(sqlite3_open_v2(":memory:", &db,
                       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY |
                       SQLITE_OPEN_FULLMUTEX, nullptr),
                      SQLITE_OK );
  // Check empty!
  sqlite3_stmt* stmt = nullptr;
  // Make sure we get our own fresh and empty DB.
  sqlite3_prepare_v2(
    db,
    "SELECT 1 from sqlite_master where type='table' and name='ParameterSets';",
    -1,
    &stmt,
    nullptr);
  BOOST_REQUIRE_EQUAL(sqlite3_step(stmt), SQLITE_DONE); // No rows.
  sqlite3_reset(stmt);
  ParameterSetRegistry::exportTo(db);
  BOOST_REQUIRE_EQUAL(sqlite3_step(stmt), SQLITE_ROW); // Found table.
  sqlite3_finalize(stmt);
  sqlite3_prepare_v2(
    db, "SELECT COUNT(*) from ParameterSets;", -1, &stmt, nullptr);
  BOOST_REQUIRE_EQUAL(sqlite3_step(stmt), SQLITE_ROW);
  BOOST_REQUIRE_EQUAL(sqlite3_column_int64(stmt, 0), 13l);
  sqlite3_finalize(stmt);
  sqlite3_close(db);
}

BOOST_AUTO_TEST_SUITE_END()

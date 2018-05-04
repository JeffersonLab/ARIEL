// ======================================================================
//
// test of tuple-type decoding
//
// template<typename T,std::size_t SIZE>
// void decode( boost::any const & a, std::array<T,SIZE> & result );
//
// template<typename KEY,typename VALUE>
// void decode( boost::any const & a, std::pair<KEY,VALUE> & result );
//
// template<typename ... ARGS>
// void decode( boost::any const & a, std::tuple<ARGS...> & result );
//
// ======================================================================

#include "cetlib_except/demangle.h"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"

#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <tuple>
#include <typeinfo>
#include <utility>
#include <vector>

using namespace fhicl;

namespace {

  template <typename T>
  void
  print(T const& t)
  {
    std::cout << std::setw(15) << std::left
              << cet::demangle_symbol(typeid(t).name()) << " with value \"" << t
              << "\"" << std::endl;
  }
}

int
main()
{

  putenv(const_cast<char*>("FHICL_FILE_PATH=./test:."));
  cet::filepath_lookup policy("FHICL_FILE_PATH");
  std::string const in("to_tupleTypes_test.fcl");

  fhicl::intermediate_table tbl1;
  fhicl::parse_document(in, policy, tbl1);
  fhicl::ParameterSet pset;
  fhicl::make_ParameterSet(tbl1, pset);

  //======================================================================
  // array checking
  //======================================================================

  std::cout << std::endl;
  std::cout << "=================================" << std::endl;
  std::cout << " ARRAY CHECK " << std::endl;
  std::cout << "=================================" << std::endl;
  std::cout << std::endl;

  // warmup
  auto const vs0 = pset.get<std::vector<std::string>>("vec0");

  // to array
  auto const array = pset.get<std::array<std::string, 4>>("vec0");
  auto const vec_arrays =
    pset.get<std::vector<std::array<std::string, 3>>>("vec_arrays");

  // test error
  try {
    auto const err0 = pset.get<std::array<std::string, 3>>("vec0");
  }
  catch (const fhicl::exception& e) {
    std::cout << e.what() << std::endl;
  }

  //======================================================================
  // pair checking
  //======================================================================

  std::cout << std::endl;
  std::cout << "=================================" << std::endl;
  std::cout << " PAIR CHECK " << std::endl;
  std::cout << "=================================" << std::endl;
  std::cout << std::endl;

  // warmup
  auto const vs1 = pset.get<std::vector<std::string>>("vec1");
  auto const pr_as_strings = pset.get<std::vector<std::string>>("pair1");

  // to pair
  auto const pr = pset.get<std::pair<unsigned, std::string>>("pair1");

  // to vector of pairs
  auto const vofp =
    pset.get<std::vector<std::pair<std::string, unsigned>>>("pair2");
  std::cout << std::endl;
  for (const auto& entry : vofp) {
    std::cout << entry.first << " " << entry.second << std::endl;
  }

  // to pair of pairs
  auto const pofp =
    pset.get<std::pair<std::pair<std::string, unsigned>,
                       std::pair<std::string, unsigned>>>("pair2");
  std::cout << std::endl;
  std::cout << pofp.first.first << " " << pofp.first.second << std::endl
            << pofp.second.first << " " << pofp.second.second << std::endl;

  // test error
  try {
    auto const err1 = pset.get<std::pair<std::string, std::string>>("pair3");
  }
  catch (const fhicl::exception& e) {
    std::cout << e.what() << std::endl;
  }

  //======================================================================
  // tuple checking
  //======================================================================

  std::cout << std::endl;
  std::cout << "=================================" << std::endl;
  std::cout << " TUPLE CHECK" << std::endl;
  std::cout << "=================================" << std::endl;
  std::cout << std::endl;

  // warmup
  auto const vs2 = pset.get<std::vector<std::string>>("vec2");
  auto const tup_as_strings = pset.get<std::vector<std::string>>("tup1");

  // to tuple
  auto const tup1 = pset.get<std::tuple<int, std::string, double>>("tup1");

  print(std::get<0>(tup1));
  print(std::get<1>(tup1));
  print(std::get<2>(tup1));

  // vector of tuples
  auto const composers =
    pset
      .get<std::vector<std::tuple<std::size_t, std::string, unsigned, double>>>(
        "tup2");

  for (const auto& composer : composers) {
    std::cout << std::endl;
    print(std::get<0>(composer));
    print(std::get<1>(composer));
    print(std::get<2>(composer));
    print(std::get<3>(composer));
  }

  // test error
  try {
    auto const err1 =
      pset.get<std::vector<std::tuple<std::size_t, std::string, unsigned>>>(
        "tup2");
  }
  catch (const fhicl::exception& e) {
    std::cout << e.what() << std::endl;
  }

  // tuple of tuples
  auto const factoids = pset.get<
    std::tuple<std::tuple<std::string, std::string, std::string, std::string>,
               std::tuple<std::string, std::string, std::string, std::string>,
               std::tuple<std::string,
                          std::string,
                          std::string,
                          std::string,
                          unsigned,
                          unsigned,
                          unsigned>>>("tup3");

  const auto nine_symphonies = std::get<0>(factoids);
  std::cout << std::endl;
  print(std::get<0>(nine_symphonies));
  print(std::get<1>(nine_symphonies));
  print(std::get<2>(nine_symphonies));
  print(std::get<3>(nine_symphonies));

  const auto less_than_9_symphonies = std::get<1>(factoids);
  std::cout << std::endl;
  print(std::get<0>(less_than_9_symphonies));
  print(std::get<1>(less_than_9_symphonies));
  print(std::get<2>(less_than_9_symphonies));

  const auto piano_sonatas = std::get<2>(factoids);
  std::cout << std::endl;
  print(std::get<0>(piano_sonatas));
  print(std::get<1>(piano_sonatas));
  print(std::get<2>(piano_sonatas));
  print(std::get<3>(piano_sonatas));
  print(std::get<4>(piano_sonatas));
  print(std::get<5>(piano_sonatas));
  print(std::get<6>(piano_sonatas));
}

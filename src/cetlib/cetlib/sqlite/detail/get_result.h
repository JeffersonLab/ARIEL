#ifndef cetlib_sqlite_detail_get_result_h
#define cetlib_sqlite_detail_get_result_h

#include "cetlib/sqlite/detail/convert.h"
#include "cetlib/sqlite/query_result.h"

#include <cassert>
#include <tuple>

namespace cet::sqlite::detail {

  template <std::size_t I, typename Tuple>
  std::enable_if_t<(I == std::tuple_size_v<Tuple>)>
  fillData(Tuple&,
           int const ncols [[maybe_unused]],
           int const currentcol [[maybe_unused]],
           char**)
  {
    assert(currentcol == ncols);
  }

  template <std::size_t I, typename Tuple>
  std::enable_if_t<(I < std::tuple_size_v<Tuple>)>
  fillData(Tuple& data, int const ncols, int currentcol, char** results)
  {
    assert(currentcol != ncols);
    using ET = std::tuple_element_t<I, Tuple>;
    std::get<I>(data) = detail::convertTo<ET>(results[currentcol]);
    fillData<I + 1>(data, ncols, ++currentcol, results);
  }

  template <typename... Args>
  int
  get_result(void* data, int ncols, char** results, char** cnames)
  {
    assert(ncols >= 1);
    auto j = static_cast<cet::sqlite::query_result<Args...>*>(data);
    if (j->columns.empty()) {
      for (int i{}; i < ncols; ++i)
        j->columns.push_back(cnames[i]);
    }

    assert(sizeof...(Args) == ncols);
    std::tuple<Args...> rowdata;
    int currentCol{};
    fillData<0u>(rowdata, ncols, currentCol, results);
    j->data.emplace_back(rowdata);
    return 0;
  }
} // cet::sqlite::detail

#endif /* cetlib_sqlite_detail_get_result_h */

// Local variables:
// mode: c++
// End:

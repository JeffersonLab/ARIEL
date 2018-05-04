#ifndef fhiclcpp_detail_try_blocks_h
#define fhiclcpp_detail_try_blocks_h

#include "fhiclcpp/exception.h"

#include <string>

namespace fhicl {
  namespace detail {

    template <typename L>
    void
    try_insert(L l, std::string const& key) try {
      l(key);
    }
    catch (boost::bad_lexical_cast const& e) {
      throw fhicl::exception(cant_insert, key) << e.what();
    }
    catch (boost::bad_numeric_cast const& e) {
      throw fhicl::exception(cant_insert, key) << e.what();
    }
    catch (fhicl::exception const& e) {
      throw fhicl::exception(cant_insert, key, e);
    }
    catch (std::exception const& e) {
      throw fhicl::exception(cant_insert, key) << e.what();
    }
  }
}

#endif /* fhiclcpp_detail_try_blocks_h */

// Local variables:
// mode: c++
// End:

#ifndef cetlib_detail_ostream_handle_impl_h
#define cetlib_detail_ostream_handle_impl_h

#include <fstream>
#include <ostream>
#include <string>

namespace cet {
  namespace detail {

    // Interface
    class ostream_handle_base {
    public:
      virtual ~ostream_handle_base() = default;
      std::ostream&
      stream()
      {
        return get_stream();
      }

    private:
      virtual std::ostream& get_stream() = 0;
    };

    // Concrete types below
    class ostream_observer : public ostream_handle_base {
    public:
      ostream_observer(std::ostream& os) : os_{os} {}

    private:
      std::ostream& os_;
      std::ostream&
      get_stream() override
      {
        return os_;
      }
    };

    template <typename OSTREAM,
              typename =
                std::enable_if_t<std::is_base_of<std::ostream, OSTREAM>::value>>
    class ostream_owner : public ostream_handle_base {
    public:
      ostream_owner(OSTREAM&& os) : os_(std::move(os)) {}

    private:
      OSTREAM os_;
      std::ostream&
      get_stream() override
      {
        return os_;
      }
    };
  }
}

#endif /* cetlib_detail_ostream_handle_impl_h */

// Local variables:
// mode: c++
// End:

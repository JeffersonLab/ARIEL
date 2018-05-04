#ifndef cetlib_ostream_handle_h
#define cetlib_ostream_handle_h

/*
   ====================================================================

   ostream_handle

   This is a handle for dealing with owning/non-owning ostream
   objects.  There are use cases when one needs an externally provided
   stream (e.g. std::cout or std::cerr), and cases when one needs an
   owned stream (std::ofstream).  This handle enables a consistent use
   of both.

   For the 'std::ofstream' case, the destructor of ostream_handle
   implicitly calls std::ofstream::close().

   Example:

     cet::ostream_handle os {std::cout};
     cet::ostream_handle toFile {"myFile.txt"};

     os     << 4 << "thirteen"; // write to STDOUT
     toFile << 5 << "fourteen"; // write to "myFile.txt"
     os = std::move(toFile);    // Reset 'os' to stream to "myFile.txt"
     os     << 6 << "fifteen";  // write to "myFile.txt"

   ====================================================================
*/

#include "cetlib/detail/ostream_handle_impl.h"

#include <memory>

namespace cet {

  class ostream_handle {
  public:
    ostream_handle() = default;

    // Writes to (e.g.) cout, which is owned by the standard library.
    ostream_handle(std::ostream& os)
      : osh_{std::make_unique<detail::ostream_observer>(os)}
    {}

    // Writes to ofstream file.
    ostream_handle(std::string const& fn,
                   std::ios_base::openmode const mode = std::ios_base::out)
      : osh_{std::make_unique<detail::ostream_owner<std::ofstream>>(
          std::ofstream(fn, mode))}
    {}

    // Writes to provided arbitrary ostream (takes ownership).
    template <typename OSTREAM,
              typename =
                std::enable_if_t<std::is_base_of<std::ostream, OSTREAM>::value>>
    ostream_handle(OSTREAM&& os)
      : osh_{std::make_unique<detail::ostream_owner<OSTREAM>>(std::move(os))}
    {}

    ostream_handle&
    operator<<(char const msg[])
    {
      osh_->stream() << msg;
      return *this;
    }

    template <typename T>
    ostream_handle&
    operator<<(T const& t)
    {
      osh_->stream() << t;
      return *this;
    }

    void
    flush()
    {
      osh_->stream().flush();
    }
    explicit operator bool() const { return static_cast<bool>(osh_->stream()); }
    operator std::ostream&() { return osh_->stream(); }

  private:
    std::unique_ptr<detail::ostream_handle_base> osh_;
  };
}

#endif /* cetlib_ostream_handle_h */

// Local variables:
// mode: c++
// End:

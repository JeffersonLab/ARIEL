#ifndef cetlib_except_exception_h
#define cetlib_except_exception_h

// ======================================================================
//
//  exception: An exception type with category capability
//
// ----------------------------------------------------------------------
//
// Each exception is identified by a category string.  This string is a
// short word or phrase (no whitespace) describing the problem that was
// encountered.  These category identifiers can be concatenated when
// exceptions are caught and rethrown; the entire list can be accessed
// using the history() call.  Example:
//
//   try {
//     func();
//   }
//   catch (cet::exception & e)
//   {
//     throw cet::exception("DataCorrupt","encountered while unpacking",e);
//   }
//
// Information can be appended to the exception via operator <<.  Example:
//
//   if((rc=func()) < 0) {
//     throw cet::exception("DataCorrupt")
//       << "I died with rc = " << rc
//       << std::endl;
//   }
//
// ----------------------------------------------------------------------
//
// Derived types are expected to adjust the category, either
//   1) by passing a string literal to the base class constructor, or
//   2) by ensuring the developer gives a category name.
// Example:
//
//  class infinite_loop
//    : public cet::exception
//  {
//    infinite_loop(std::string const& mesg)
//      : exception("infinite_loop", mesg)
//    { }
//  };
//
// ----------------------------------------------------------------------
//
// The output from what() has a simple format that makes it easy to
// locate the reason for and context of the error and separate it from
// the user-supplied free-format text.  The output from what() contains
// all the category and context information in a nested hierarchical format.
//
// ======================================================================

#include <exception>
#include <iosfwd>
#include <list>
#include <sstream>
#include <string>
#include <type_traits>

namespace cet {
  class exception;

  std::ostream& operator<<(std::ostream& os, exception const& e);
}

// ======================================================================

namespace cet {
  namespace detail {

    template <
      class D,
      bool = std::is_base_of<cet::exception, std::remove_reference_t<D>>::value>
    struct enable_if_an_exception {
      using type = D&&;
    };

    template <class D>
    struct enable_if_an_exception<D, false> {
    };

  } // detail
} // cet

// ======================================================================

namespace cet {

  class exception : public std::exception {
  public:
    using Category = std::string;
    using CategoryList = std::list<Category>;

    // --- c'tors, d'tor:

    explicit exception(Category const& category);

    exception(Category const& category, std::string const& message);

    exception(Category const& category,
              std::string const& message,
              exception const& another);

    exception(exception const& other);

    // Unfortunately, move-semantics aren't available for std::ostream
    // objects in GCC 4.9.3.  We therefore have to define our own
    // move-c'tor.  This should be removed once we migrate to a
    // compiler that supports move-semantics on streams.
    //
    // It is possible for an exception to be thrown from this
    // contructor, so cannot declare noexcept.
    exception(exception&&);

    virtual ~exception() noexcept = default;

    // --- inspectors:

    char const* what() const noexcept override;
    virtual std::string explain_self() const;
    std::string category() const;
    CategoryList const& history() const;
    std::string root_cause() const;

    // --- mutators:

    void append(exception const& another);

    void append(std::string const& more_information);
    void append(char const more_information[]);

    void append(std::ostream& f(std::ostream&));
    void append(std::ios_base& f(std::ios_base&));

    template <class T>
    void
    append(T const& more_information)
    {
      ost_ << more_information;
    }

  private:
    std::ostringstream ost_{};
    CategoryList category_;
    mutable std::string what_{};

  }; // exception

  template <class E>
  typename detail::enable_if_an_exception<E>::type
  operator<<(E&& e, std::string const& t)
  {
    e.append(t);
    return std::forward<E>(e);
  }

  template <class E>
  typename detail::enable_if_an_exception<E>::type
  operator<<(E&& e, char const t[])
  {
    e.append(t);
    return std::forward<E>(e);
  }

  template <class E>
  typename detail::enable_if_an_exception<E>::type
  operator<<(E&& e, std::ostream& f(std::ostream&))
  {
    e.append(f);
    return std::forward<E>(e);
  }

  template <class E>
  typename detail::enable_if_an_exception<E>::type
  operator<<(E&& e, std::ios_base& f(std::ios_base&))
  {
    e.append(f);
    return std::forward<E>(e);
  }

  template <class E, class T>
  typename detail::enable_if_an_exception<E>::type
  operator<<(E&& e, T const& t)
  {
    e.append(t);
    return std::forward<E>(e);
  }

} // namespace cet

// ======================================================================

// Local variables
// mode: c++
// End:
#endif /* cetlib_except_exception_h */

// Local Variables:
// mode: c++
// End:

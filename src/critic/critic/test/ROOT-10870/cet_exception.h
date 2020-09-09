#ifndef r10870_exception_h
#define r10870_exception_h

#include <exception>
#include <iosfwd>
#include <list>
#include <sstream>
#include <string>
#include <type_traits>

namespace r10870 {
  class exception;

  std::ostream& operator<<(std::ostream& os, exception const& e);
}

// ======================================================================

namespace r10870::detail {

  template <
    class D,
    bool = std::is_base_of<r10870::exception, std::remove_reference_t<D>>::value>
  struct enable_if_an_exception {
    using type = D&&;
  };

  template <class D>
  struct enable_if_an_exception<D, false> {};

} // r10870::detail

// ======================================================================

namespace r10870 {

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

    // The 'noexcept' keyword is applied to the copy c'tor to indicate
    // that if an exception escapes the copy constructor call, then an
    // immediate std::terminate() will be invoked.  Such a
    // circumstance would likely be a symptom of memory exhaustion, in
    // which case program recovery is incredibly unlikely.
    exception(exception const& other) noexcept;

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

} // namespace r10870

#endif /* r10870_exception_h */

// Local Variables:
// mode: c++
// End:

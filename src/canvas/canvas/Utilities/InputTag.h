#ifndef canvas_Utilities_InputTag_h
#define canvas_Utilities_InputTag_h
// vim: set sw=2 expandtab :

#include <any>
#include <iosfwd>
#include <string>
#include <tuple>

namespace art {

  class InputTag {
  public:
    ~InputTag();
    InputTag();

    InputTag(std::string const& label,
             std::string const& instance,
             std::string const& processName = {});

    InputTag(char const* label,
             char const* instance,
             char const* processName = "");

    InputTag(std::string const&);

    InputTag(char const*);

    InputTag(InputTag const&);
    InputTag(InputTag&&);

    InputTag& operator=(InputTag const&);
    InputTag& operator=(InputTag&&);

    bool operator==(InputTag const&) const noexcept;

    bool empty() const noexcept;

    std::string const& label() const noexcept;
    std::string const& instance() const noexcept;
    std::string const& process() const noexcept;

    std::string encode() const;

  private:
    std::string label_{};
    std::string instance_{};
    std::string process_{};
  };

  bool operator!=(InputTag const&, InputTag const&);

  void decode(std::any const&, InputTag&);

  std::ostream& operator<<(std::ostream&, InputTag const&);

} // namespace art

// Specialization to allow associative containers with InputTag as the
// key type.
namespace std {
  template <>
  class less<art::InputTag> {
  public:
    inline bool
    operator()(art::InputTag const& lhs, art::InputTag const& rhs) const
      noexcept
    {
      auto const& a = std::tie(lhs.label(), lhs.instance(), lhs.process());
      auto const& b = std::tie(rhs.label(), rhs.instance(), rhs.process());
      return a < b;
    }
  };
}

#endif /* canvas_Utilities_InputTag_h */

// Local Variables:
// mode: c++
// End:

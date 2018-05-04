#ifndef canvas_Utilities_InputTag_h
#define canvas_Utilities_InputTag_h

#include <iosfwd>
#include <string>

namespace boost {
  class any;
}

namespace art {
  class InputTag;

  std::ostream& operator<<(std::ostream& ost, InputTag const& tag);
  bool operator!=(InputTag const& left, InputTag const& right) noexcept;
}

class art::InputTag {
public:
  InputTag() = default;

  // Create an InputTag by parsing the given string, which is
  // expected to be in colon-delimited form, fitting one of the
  // following patterns:
  //
  //   label
  //   label:instance
  //   label:instance:process
  InputTag(std::string const& s);
  InputTag(char const* s);

  // Create an InputTag with the given label, instance, and process
  // specificiations.
  InputTag(std::string const& label,
           std::string const& instance,
           std::string const& processName = {});

  // Create an InputTag with the given label, instance, and process
  // specifications.
  InputTag(char const* label,
           char const* instance,
           char const* processName = "");

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  std::string encode() const;

  bool
  empty() const noexcept
  {
    return label_.empty() && instance_.empty() && process_.empty();
  }

  std::string const&
  label() const noexcept
  {
    return label_;
  }
  std::string const&
  instance() const noexcept
  {
    return instance_;
  }
  /// an empty string means find the most recently produced
  /// product with the label and instance
  std::string const&
  process() const noexcept
  {
    return process_;
  }

  bool operator==(InputTag const& tag) const noexcept;

private:
  std::string label_{};
  std::string instance_{};
  std::string process_{};

  // Helper function, to parse colon-separated initialization
  // string.
  void set_from_string_(std::string const& s);
};

inline art::InputTag::InputTag(std::string const& label,
                               std::string const& instance,
                               std::string const& processName)
  : label_(label), instance_(instance), process_(processName)
{}

inline art::InputTag::InputTag(char const* label,
                               char const* instance,
                               char const* processName)
  : label_(label), instance_(instance), process_(processName)
{}

inline art::InputTag::InputTag(std::string const& s)
{
  set_from_string_(s);
}

inline art::InputTag::InputTag(char const* s)
{
  set_from_string_(s);
}

inline bool
art::InputTag::operator==(InputTag const& tag) const noexcept
{
  return (label_ == tag.label_) && (instance_ == tag.instance_) &&
         (process_ == tag.process_);
}

inline bool
art::operator!=(InputTag const& left, InputTag const& right) noexcept
{
  return !(left == right);
}

//=====================================================================
// decode specialization for allowing conversions from
//    atom     ===> InputTag
//    sequence ===> InputTag

namespace art {
  void decode(boost::any const& a, InputTag& tag);
}

#endif /* canvas_Utilities_InputTag_h */

// Local Variables:
// mode: c++
// End:

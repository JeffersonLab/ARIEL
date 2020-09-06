// ======================================================================
//
// ParameterSetID
//
// ======================================================================

#include "fhiclcpp/ParameterSetID.h"
#include "fhiclcpp/ParameterSet.h"

#include <iomanip>

using namespace boost;
using namespace cet;
using namespace fhicl;
using namespace std;

// ======================================================================

constexpr sha1::digest_t invalid_id{{}};

// ----------------------------------------------------------------------

ParameterSetID::ParameterSetID() noexcept : valid_{false}, id_{invalid_id} {}

ParameterSetID::ParameterSetID(ParameterSet const& ps) : valid_{false}, id_{}
{
  reset(ps);
}

ParameterSetID::ParameterSetID(std::string const& id)
  : valid_{id.size() == max_str_size()}, id_{}
{
  if (valid_) {
    for (size_t i = 0, e = id_.size(); i != e; ++i) {
      id_[i] = std::stoi(id.substr(i * 2, 2), nullptr, 16);
    }
    if (id != to_string()) {
      throw exception{error::cant_happen}
        << "ParameterSetID construction failure: " << id
        << " != " << to_string() << ".\n";
    }
  } else if (id.empty()) {
    id_ = invalid_id;
  } else {
    throw fhicl::exception{error::parse_error}
      << "Attempt to construct ParameterSetID from inappropriate input: " << id
      << ".\n";
  }
}

// ----------------------------------------------------------------------

bool
ParameterSetID::is_valid() const noexcept
{
  return valid_;
}

string
ParameterSetID::to_string() const
{
  std::ostringstream oss;
  oss << std::hex << std::setfill('0');
  // The range-for loop performs an implicit cast from 'unsigned char'
  // to 'unsigned int'.
  for (unsigned int const num : id_) {
    oss << std::setw(2) << num;
  }
  return oss.str();
}

// ----------------------------------------------------------------------

void
ParameterSetID::invalidate() noexcept
{
  valid_ = false;
  id_ = invalid_id;
}

void
ParameterSetID::reset(ParameterSet const& ps)
{
  auto const& hash = ps.to_string();
  sha1 sha{hash.c_str()};

  id_ = sha.digest();
  valid_ = true;
}

void
ParameterSetID::swap(ParameterSetID& other)
{
  id_.swap(other.id_);
  std::swap(valid_, other.valid_);
}

// ----------------------------------------------------------------------

bool
ParameterSetID::operator==(ParameterSetID const& other) const noexcept
{
  return id_ == other.id_;
}

bool
ParameterSetID::operator!=(ParameterSetID const& other) const noexcept
{
  return id_ != other.id_;
}

bool
ParameterSetID::operator<(ParameterSetID const& other) const noexcept
{
  return id_ < other.id_;
}

bool
ParameterSetID::operator>(ParameterSetID const& other) const noexcept
{
  return id_ > other.id_;
}

bool
ParameterSetID::operator<=(ParameterSetID const& other) const noexcept
{
  return id_ <= other.id_;
}

bool
ParameterSetID::operator>=(ParameterSetID const& other) const noexcept
{
  return id_ >= other.id_;
}

// ----------------------------------------------------------------------

ostream&
fhicl::operator<<(ostream& os, ParameterSetID const& psid)
{
  return os << psid.to_string();
}

// ======================================================================

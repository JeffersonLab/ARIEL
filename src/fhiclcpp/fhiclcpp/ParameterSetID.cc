// ======================================================================
//
// ParameterSetID
//
// ======================================================================

#include "fhiclcpp/ParameterSetID.h"

#include "boost/format.hpp"
#include "fhiclcpp/ParameterSet.h"

using namespace boost;
using namespace cet;
using namespace fhicl;
using namespace std;

typedef sha1::digest_t digest_t;

// ======================================================================

static digest_t const&
invalid_id_()
{
  static digest_t invalid_value = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
  return invalid_value;
}

// ----------------------------------------------------------------------

ParameterSetID::ParameterSetID() : valid_(false), id_(invalid_id_()) {}

ParameterSetID::ParameterSetID(ParameterSet const& ps) : valid_(false), id_()
{
  reset(ps);
}

ParameterSetID::ParameterSetID(std::string const& id)
  : valid_(id.size() == max_str_size()), id_()
{
  if (valid_) {
    for (size_t i = 0, e = id_.size(); i != e; ++i) {
      id_[i] = std::stoi(id.substr(i * 2, 2), nullptr, 16);
    }
    if (id != to_string()) {
      throw exception(error::cant_happen)
        << "ParameterSetID construction failure: " << id
        << " != " << to_string() << ".\n";
    }
  } else if (id.empty()) {
    id_ = invalid_id_();
  } else {
    throw fhicl::exception(error::parse_error)
      << "Attempt to construct ParameterSetID from inappropriate input: " << id
      << ".\n";
  }
}

// ----------------------------------------------------------------------

bool
ParameterSetID::is_valid() const
{
  return valid_;
}

string
ParameterSetID::to_string() const
{
  string s;
  for (std::size_t i = 0; i != id_.size(); ++i)
    s += str(format("%02x") % (unsigned int)id_[i]);
  return s;
}

// ----------------------------------------------------------------------

void
ParameterSetID::invalidate()
{
  valid_ = false;
  id_ = invalid_id_();
}

void
ParameterSetID::reset(ParameterSet const& ps)
{
  string const& hash(ps.to_string());
  sha1 sha(hash.c_str());

  id_ = sha.digest(), valid_ = true;
}

void
ParameterSetID::swap(ParameterSetID& other)
{
  id_.swap(other.id_);
  std::swap(valid_, other.valid_);
}

// ----------------------------------------------------------------------

bool
ParameterSetID::operator==(ParameterSetID const& other) const
{
  return id_ == other.id_;
}

bool
ParameterSetID::operator!=(ParameterSetID const& other) const
{
  return id_ != other.id_;
}

bool
ParameterSetID::operator<(ParameterSetID const& other) const
{
  return id_ < other.id_;
}

bool
ParameterSetID::operator>(ParameterSetID const& other) const
{
  return id_ > other.id_;
}

bool
ParameterSetID::operator<=(ParameterSetID const& other) const
{
  return id_ <= other.id_;
}

bool
ParameterSetID::operator>=(ParameterSetID const& other) const
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

#include "fhiclcpp/detail/PrettifierPrefixAnnotated.h"
#include "cetlib/container_algorithms.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/detail/Indentation.h"
#include "fhiclcpp/detail/printing_helpers.h"

using namespace fhicl;
using namespace fhicl::detail;

//===================================================================

PrettifierPrefixAnnotated::PrettifierPrefixAnnotated()
  : buffer_()
  , indent_{}
  , info_()
  , sequence_sizes_{{-1u}}
  , curr_size_{}
  , name_stack_{}
{}

//==========================================================================
void
PrettifierPrefixAnnotated::before_action(key_t const& key,
                                         any_t const&,
                                         ParameterSet const* ps)
{
  info_ = ps->get_src_info(key);
}

//==========================================================================

void
PrettifierPrefixAnnotated::enter_table(std::string const& key,
                                       boost::any const&)
{
  buffer_ << print_full_key_(key) << nl() << print_prefix_annotated_info(info_)
          << nl() << indent_() << table::printed_prefix(key) << nl();
  indent_.push();
  name_stack_.emplace_back(key);
}

void
PrettifierPrefixAnnotated::exit_table(std::string const& key, boost::any const&)
{
  name_stack_.pop_back();
  indent_.pop();
  buffer_ << indent_() << table::closing_brace()
          << printed_suffix(key, sequence_sizes_.top()) << nl();
}

//==========================================================================

void
PrettifierPrefixAnnotated::enter_sequence(std::string const& key,
                                          boost::any const& a)
{
  push_size_(a);
  buffer_ << print_full_key_(key) << nl() << print_prefix_annotated_info(info_)
          << nl() << indent_() << sequence::printed_prefix(key) << nl();
  indent_.push();
}

void
PrettifierPrefixAnnotated::exit_sequence(std::string const& key,
                                         boost::any const&)
{
  indent_.pop();
  buffer_ << indent_() << sequence::closing_brace()
          << printed_suffix(key, sequence_sizes_.top()) << nl();
  pop_size_();
}

//==========================================================================

void
PrettifierPrefixAnnotated::atom(std::string const& key, boost::any const& a)
{
  buffer_ << print_full_key_(key) << nl() << print_prefix_annotated_info(info_)
          << nl() << indent_() << atom::printed_prefix(key) << atom::value(a)
          << printed_suffix(key, sequence_sizes_.top()) << nl();
}

//=========================================================================

void
PrettifierPrefixAnnotated::push_size_(boost::any const& a)
{
  sequence_sizes_.emplace(boost::any_cast<ps_sequence_t>(a).size());
  curr_size_ = sequence_sizes_.top();
}

void
PrettifierPrefixAnnotated::pop_size_()
{
  sequence_sizes_.pop();
  curr_size_ = sequence_sizes_.top();
}

std::string
PrettifierPrefixAnnotated::print_full_key_(name_t const& name) const
{
  std::ostringstream os;
  os << "#KEY|";
  cet::copy_all(name_stack_, std::ostream_iterator<std::string>{os, "."});
  os << name << "|";
  return os.str();
}

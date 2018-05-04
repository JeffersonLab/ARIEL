#include "fhiclcpp/detail/PrettifierAnnotated.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/detail/Indentation.h"
#include "fhiclcpp/detail/printing_helpers.h"

using namespace fhicl;
using namespace fhicl::detail;

//===================================================================

PrettifierAnnotated::PrettifierAnnotated(unsigned const initial_indent_level)
  : buffer_()
  , indent_{initial_indent_level}
  , curr_info_()
  , cached_info_()
  , sequence_sizes_{{-1u}}
  , curr_size_{sequence_sizes_.top()}
{}

//==========================================================================
void
PrettifierAnnotated::before_action(key_t const& key,
                                   any_t const&,
                                   ParameterSet const* ps)
{
  curr_info_ = ps->get_src_info(key);
}

void
PrettifierAnnotated::after_action()
{
  cached_info_ = curr_info_;
}

//==========================================================================

void
PrettifierAnnotated::enter_table(std::string const& key, boost::any const&)
{
  buffer_ << indent_() << table::printed_prefix(key)
          << print_annotated_info(curr_info_, cached_info_) << nl();
  indent_.push();
}

void
PrettifierAnnotated::exit_table(std::string const& key, boost::any const&)
{
  indent_.pop();
  buffer_ << indent_() << table::closing_brace()
          << printed_suffix(key, curr_size_) << nl();
}

//==========================================================================

void
PrettifierAnnotated::enter_sequence(std::string const& key, boost::any const& a)
{
  push_size_(a);
  buffer_ << indent_() << sequence::printed_prefix(key)
          << print_annotated_info(curr_info_, cached_info_) << nl();
  indent_.push();
}

void
PrettifierAnnotated::exit_sequence(std::string const& key, boost::any const&)
{
  indent_.pop();
  buffer_ << indent_() << sequence::closing_brace()
          << printed_suffix(key, curr_size_) << nl();
  pop_size_();
}

//==========================================================================

void
PrettifierAnnotated::atom(std::string const& key, boost::any const& a)
{
  buffer_ << indent_() << atom::printed_prefix(key) << atom::value(a)
          << printed_suffix(key, curr_size_)
          << print_annotated_info(curr_info_, cached_info_) << nl();
}

//=========================================================================

void
PrettifierAnnotated::push_size_(boost::any const& a)
{
  sequence_sizes_.emplace(boost::any_cast<ps_sequence_t>(a).size());
  curr_size_ = sequence_sizes_.top();
}

void
PrettifierAnnotated::pop_size_()
{
  sequence_sizes_.pop();
  curr_size_ = sequence_sizes_.top();
}

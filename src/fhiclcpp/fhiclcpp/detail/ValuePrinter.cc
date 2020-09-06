#include "fhiclcpp/detail/ValuePrinter.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/detail/Indentation.h"
#include "fhiclcpp/detail/printing_helpers.h"

#include <limits>

using namespace fhicl;
using namespace fhicl::detail;

namespace {
  constexpr auto size_t_max = std::numeric_limits<std::size_t>::max();
}

//==========================================================================

ValuePrinter::ValuePrinter(std::string const& key_to_search,
                           unsigned const initial_indent_level)
  : key_{key_to_search}
  , indent_{initial_indent_level}
  , sequence_sizes_{{size_t_max}}
  , seq_size_{sequence_sizes_.top()}
  , table_size_{0u}
{}

//==========================================================================
void
ValuePrinter::before_action(key_t const& key,
                            any_t const& a,
                            ParameterSet const* ps)
{
  if (key_ == key) {
    print_encapsulated_values_ = true;
  }
  if (!is_table(a))
    return;
  table_size_ = ps->get<fhicl::ParameterSet>(key).get_all_keys().size();
}

void
ValuePrinter::after_action(key_t const& key)
{
  if (key_ == key) {
    print_encapsulated_values_ = false;
  }
}

//==========================================================================

void
ValuePrinter::enter_table(std::string const& key, std::any const&)
{
  if (not print_encapsulated_values_)
    return;
  buffer_ << indent_() << table::printed_prefix(key) << maybe_nl_(table_size_);
  indent_.push();
}

void
ValuePrinter::exit_table(std::string const& key, std::any const&)
{
  if (not print_encapsulated_values_)
    return;
  indent_.pop();
  buffer_ << maybe_indent_(table_size_) << table::closing_brace()
          << printed_suffix(key, seq_size_) << nl();
}

//==========================================================================

void
ValuePrinter::enter_sequence(std::string const& key, std::any const& a)
{
  if (not print_encapsulated_values_)
    return;
  push_size_(a);
  buffer_ << indent_() << sequence::printed_prefix(key) << maybe_nl_(seq_size_);
  indent_.push();
}

void
ValuePrinter::exit_sequence(std::string const& key, std::any const&)
{
  if (not print_encapsulated_values_)
    return;
  // FIXME: To support a printout like:
  //
  //   empty: []
  //
  // We need to first capture the size of the sequence (0) before we
  // call pop_size_().  However, for non-empty sequences (I think...)
  // pop_size_ needs to be called before we determine if an
  // indentation should take place.  This needs to be cleaned up.
  bool const empty_sequence = seq_size_ == 0;
  indent_.pop();
  pop_size_();
  buffer_ << (empty_sequence ? "" : maybe_indent_(seq_size_))
          << sequence::closing_brace() << printed_suffix(key, seq_size_)
          << nl();
}

//==========================================================================

void
ValuePrinter::atom(std::string const& key, std::any const& a)
{
  if (not print_encapsulated_values_)
    return;
  buffer_ << indent_() << atom::printed_prefix(key) << atom::value(a)
          << printed_suffix(key, seq_size_) << nl();
}

//=========================================================================

void
ValuePrinter::push_size_(std::any const& a)
{
  sequence_sizes_.emplace(std::any_cast<ps_sequence_t>(a).size());
  seq_size_ = sequence_sizes_.top();
}

void
ValuePrinter::pop_size_()
{
  sequence_sizes_.pop();
  seq_size_ = sequence_sizes_.top();
}

std::string
ValuePrinter::maybe_indent_(std::size_t const sz)
{
  return sz ? indent_() : "";
}

std::string
ValuePrinter::maybe_nl_(std::size_t const sz)
{
  return sz ? nl() : "";
}

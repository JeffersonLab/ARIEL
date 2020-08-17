#ifndef canvas_Persistency_Common_Assns_h
#define canvas_Persistency_Common_Assns_h
// vim: set sw=2:
////////////////////////////////////////////////////////////////////////
// Assns
//
// An association collection: a persistable collection of associations
// between two items in other persistent collections, with an optional
// ancillary object containing information about the association
// itself.
//
// N.B. An Assns<A, B> only makes sense when A and B are distinct:
// otherwise, there are ordering ambiguities when making and accessing
// associations. An attempt to specify an Assns<A,A> or <A, A, D> will
// fail with "... has incomplete type and cannot be defined."
//
// Note that the associations may be one-to-one, one-to-many or
// many-to-many.
//
// An Assns need only be used directly:
//
// 1. When being filled and put into the event; or
//
// 2. When the emphasis is on the associations themselves (and
// possibly their order) rather than particularly the associated
// objects. When it is desired to loop over A (or B) objects and
// access their counterparts and/or extra data objects a FindOne or
// FindMany would be more suitable.
//
////////////////////////////////////
// Interface.
//////////
//
// Note that the structure of the Assns template is non-trivial
// because there is a variant that has an extra data object attached
// to each association, and a variant that does not. In order to
// minimize code duplication then, a fairly advanced specialization
// technique is used.
//
// In order that the user of an Assns does not need to be able to
// parse and understand this mechanism, the interface is presented
// below:
//
// Notes:
//
// * L and R below are the types of two objects to be found in
// collections in the event. D where used is an arbitrary
// user-supplied type wherein information is stored about the
// association between a particular L and a particular R.
//
// * An Assns operates essentially as a vector of pairs of Ptr, with
// auxiliary methods for accessing the attached extra data object for
// an association, if applicable.
//
// * An attempt to create an Assns with a template argument D of
// pointer-type will result in a compile-time assertion failure.
//
// Useful typedefs:
//
//   typedef std::pair<Ptr<L>, Ptr<R>> assn_t;
//
// Constructors:
//
//   Assns<L, R>();
//   Assns<L, R, D>();
//
// Modifiers:
//
//   void swap(Assns& other);
//   void addSingle(Ptr<L> const&, Ptr<R> const&); // Assns<L, R> only.
//   void addSingle(Ptr<L> const&, Ptr<R> const&, D const&);
//
// Accessors:
//
//   const_iterator begin() const; // De-referencing an const_iterator
//   const_iterator end() const;   // yields an assn_t const&.
//   assn_t const& operator [] (size_type i) const;
//   assn_t const& at(size_type i) const; // Bounds-checked.
//   size_type size() const;
//   D const& data(std::size_t index) const;
//   D const& data(const_iterator it) const;
//
////////////////////////////////////////////////////////////////////////

#include "canvas/Persistency/Common/AssnsBase.h"
#include "canvas/Persistency/Common/AssnsIter.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "canvas/Persistency/Common/detail/throwPartnerException.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Utilities/TypeID.h"
#include "cetlib/container_algorithms.h"
#include "cetlib_except/demangle.h"

#include <iostream>
#include <typeinfo>
#include <vector>

namespace art {
  // General template.
  template <typename L, typename R, typename D = void>
  class Assns;

  // Specializations.
  // 1.
  template <typename LR>
  class Assns<LR, LR, void>; // Unimplemented.
  // 2.
  template <typename LR, typename D>
  class Assns<LR, LR, D>; // Unimplemented.
  // 3.
  template <typename L, typename R>
  class Assns<L, R, void>; // No data: base class.

  namespace detail {
    class AssnsStreamer;
  }
}

////////////////////////////////////////////////////////////////////////
// Implementation of the specialization (3).
template <typename L, typename R>
class art::Assns<L, R, void> : public art::detail::AssnsBase {
public:
  using left_t = L;
  using right_t = R;
  using partner_t = art::Assns<right_t, left_t, void>;

private:
  using ptrs_t = std::vector<std::pair<Ptr<left_t>, Ptr<right_t>>>;
  using ptr_data_t = std::vector<std::pair<RefCore, std::size_t>>;

public:
  using assn_t = typename ptrs_t::value_type;
  using const_iterator = typename ptrs_t::const_iterator;

  using size_type = typename ptrs_t::size_type;

  // Constructors, destructor.
  Assns();
  Assns(partner_t const& other);
  virtual ~Assns() = default;

  // Accessors.
  const_iterator begin() const;
  const_iterator end() const;
  assn_t const& operator[](size_type index) const;
  assn_t const& at(size_type index) const;
  size_type size() const;
  std::string className() const;

  // Modifiers.
  void addSingle(Ptr<left_t> const& left, Ptr<right_t> const& right);

  template <typename Ls>
  void addMany(Ls const& lefts, Ptr<right_t> const& right);

  template <typename Rs>
  void addMany(Ptr<left_t> const& left, Rs const& rights);

  void swap(art::Assns<L, R, void>& other);

  std::unique_ptr<EDProduct> makePartner(
    std::type_info const& wanted_wrapper_type) const;

  static short
  Class_Version()
  {
    return 11;
  }

  void
  aggregate(Assns const&) const
  {}

protected:
  virtual void swap_(art::Assns<L, R, void>& other);

  virtual std::unique_ptr<EDProduct> makePartner_(
    std::type_info const& wanted_wrapper_type) const;

private:
  friend class detail::AssnsStreamer;
  friend class art::Assns<right_t, left_t, void>; // partner_t.

  // FIXME: The only reason this function is virtual is to cause the
  // correct behavior to occur when the wrong streamer class is
  // called. In future (>5.30.00) versions of ROOT that can register
  // ioread rules for class template instantiations using typedefs, this
  // can be made back into a static function.
#ifdef ROOT_CAN_REGISTER_IOREADS_PROPERLY
  static
#else
  virtual
#endif
    bool
    left_first()
#ifndef ROOT_CAN_REGISTER_IOREADS_PROPERLY
      const
#endif
    ;

  void fill_transients() override;
  void fill_from_transients() override;

  ptrs_t ptrs_{}; //! transient
  ptr_data_t ptr_data_1_{};
  ptr_data_t ptr_data_2_{};
};

////////////////////////////////////////////////////////////////////////
// Yes, the general implementation inherits from the specific. Head
// exploded yet?
template <typename L, typename R, typename D>
class art::Assns : private art::Assns<L, R> {
private:
  using base = Assns<L, R>;

public:
  // We do not allow D == bool since since the AssnsNode requires
  // taking the reference to a std::vector<D> element.  For
  // std::vector<bool>::at or operator[], the returned object is a
  // value type and not a reference type.
  static_assert(
    !std::is_same_v<D, bool>,
    "\n\nart error: An 'Assns<A, B, D>' object with D = bool is not allowed.\n"
    "           If you decide that D must represent a boolean type, then we\n"
    "           recommend that you wrap a boolean value in a struct (e.g.):\n\n"
    "             struct WrappedBool { bool flag; };\n\n"
    "           Please contact artists@fnal.gov for guidance.\n");

  using left_t = typename base::left_t;
  using right_t = typename base::right_t;
  using data_t = D;
  using partner_t = art::Assns<right_t, left_t, data_t>;
  using const_iterator =
    typename art::const_AssnsIter<L, R, D, Direction::Forward>;
  using const_reverse_iterator =
    typename art::const_AssnsIter<L, R, D, Direction::Reverse>;
  using size_type = typename base::size_type;

  Assns();
  Assns(partner_t const& other);

  size_type size() const; // Implemented explicitly only to let Wrapper know.
  const_iterator begin() const;
  const_iterator end() const;
  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;

  using base::operator[];
  using base::at;

  data_t const& data(typename std::vector<data_t>::size_type index) const;
  data_t const& data(const_iterator it) const;

  // Modifiers.
  void addSingle(Ptr<left_t> const& left,
                 Ptr<right_t> const& right,
                 data_t const& data);

  template <typename Ls, typename Ds>
  void addMany(Ls const& lefts, Ptr<right_t> const& right, Ds const& data);

  template <typename Rs, typename Ds>
  void addMany(Ptr<left_t> const& left, Rs const& rights, Ds const& data);

  void swap(art::Assns<L, R, D>& other);

  // This is needed (as opposed to using base::makePartner) because
  // enable_if_function_exists_t does not detect the base's function.
  std::unique_ptr<EDProduct> makePartner(
    std::type_info const& wanted_wrapper_type) const;

  static short
  Class_Version()
  {
    return 11;
  }

  void
  aggregate(Assns const&) const
  {}

private:
  friend class art::Assns<right_t, left_t, data_t>; // partner_t.

  void swap_(art::Assns<L, R, void>& other) override;
  std::unique_ptr<EDProduct> makePartner_(
    std::type_info const& wanted_wrapper_type) const override;

  std::vector<data_t> data_;
};

/////

////////////////////////////////////////////////////////////////////////
template <typename L, typename R>
inline art::Assns<L, R, void>::Assns()
{}

template <typename L, typename R>
inline art::Assns<L, R, void>::Assns(partner_t const& other)
{
  ptrs_.reserve(other.ptrs_.size());
  cet::transform_all(
    other.ptrs_, std::back_inserter(ptrs_), [](auto const& pr) {
      using pr_t = typename ptrs_t::value_type;
      return pr_t{pr.second, pr.first};
    });
}

template <typename L, typename R>
inline typename art::Assns<L, R, void>::const_iterator
art::Assns<L, R, void>::begin() const
{
  return ptrs_.begin();
}

template <typename L, typename R>
inline typename art::Assns<L, R, void>::const_iterator
art::Assns<L, R, void>::end() const
{
  return ptrs_.end();
}

template <typename L, typename R>
inline typename art::Assns<L, R, void>::assn_t const& art::Assns<L, R, void>::
operator[](size_type const index) const
{
  return ptrs_[index];
}

template <typename L, typename R>
inline typename art::Assns<L, R, void>::assn_t const&
art::Assns<L, R, void>::at(size_type const index) const
{
  return ptrs_.at(index);
}

template <typename L, typename R>
inline typename art::Assns<L, R, void>::size_type
art::Assns<L, R, void>::size() const
{
  return ptrs_.size();
}

template <typename L, typename R>
inline std::string
art::Assns<L, R, void>::className() const
{
  TypeID const assns_type{typeid(Assns<L, R, void>)};
  return assns_type.className();
}

template <typename L, typename R>
inline void
art::Assns<L, R, void>::addSingle(Ptr<left_t> const& left,
                                  Ptr<right_t> const& right)
{
  ptrs_.emplace_back(left, right);
}

template <typename L, typename R>
template <typename Ls>
inline void
art::Assns<L, R, void>::addMany(Ls const& lefts, Ptr<right_t> const& right)
{
  static_assert(std::is_same_v<typename Ls::value_type, art::Ptr<L>>,
                "\n\nart error: The first argument must be a container whose "
                "value_type is art::Ptr<L>\n"
                "           corresponding to an Assns<L, R(, D)> object.\n");
  for (auto const& left : lefts) {
    addSingle(left, right);
  }
}

template <typename L, typename R>
template <typename Rs>
inline void
art::Assns<L, R, void>::addMany(Ptr<left_t> const& left, Rs const& rights)
{
  static_assert(std::is_same_v<typename Rs::value_type, art::Ptr<R>>,
                "\n\nart error: The second argument must be a container whose "
                "value_type is art::Ptr<R>\n"
                "           corresponding to an Assns<L, R(, D)> object.\n");
  for (auto const& right : rights) {
    addSingle(left, right);
  }
}

template <typename L, typename R>
inline void
art::Assns<L, R, void>::swap(art::Assns<L, R, void>& other)
{
  swap_(other);
}

template <typename L, typename R>
inline std::unique_ptr<art::EDProduct>
art::Assns<L, R, void>::makePartner(
  std::type_info const& wanted_wrapper_type) const
{
  return makePartner_(wanted_wrapper_type);
}

template <typename L, typename R>
inline void
art::Assns<L, R, void>::swap_(art::Assns<L, R, void>& other)
{
  using std::swap;
  swap(ptrs_, other.ptrs_);
  swap(ptr_data_1_, other.ptr_data_1_);
  swap(ptr_data_2_, other.ptr_data_2_);
}

template <typename L, typename R>
std::unique_ptr<art::EDProduct>
art::Assns<L, R, void>::makePartner_(
  std::type_info const& wanted_wrapper_type) const
{
  if (wanted_wrapper_type != typeid(Wrapper<partner_t>)) {
    detail::throwPartnerException(typeid(*this), wanted_wrapper_type);
  }
  return std::make_unique<Wrapper<partner_t>>(
    std::make_unique<partner_t>(*this));
}

template <typename L, typename R>
inline bool
art::Assns<L, R, void>::left_first() const
{
  static bool const lf_s = (art::TypeID{typeid(left_t)}.friendlyClassName() <
                            art::TypeID{typeid(right_t)}.friendlyClassName());
  return lf_s;
}

template <typename L, typename R>
void
art::Assns<L, R, void>::fill_transients()
{
  // Precondition: ptr_data_1_.size() = ptr_data_2_.size();
  ptrs_.clear();
  ptrs_.reserve(ptr_data_1_.size());
  ptr_data_t const& l_ref = left_first() ? ptr_data_1_ : ptr_data_2_;
  ptr_data_t const& r_ref = left_first() ? ptr_data_2_ : ptr_data_1_;

  for (auto l = cbegin(l_ref), e = cend(l_ref), r = cbegin(r_ref); l != e;
       ++l, ++r) {
    ptrs_.emplace_back(
      Ptr<left_t>{l->first.id(), l->second, l->first.productGetter()},
      Ptr<right_t>{r->first.id(), r->second, r->first.productGetter()});
  }
  // Empty persistent representation.
  ptr_data_t tmp1, tmp2;
  ptr_data_1_.swap(tmp1);
  ptr_data_2_.swap(tmp2);
}

template <typename L, typename R>
void
art::Assns<L, R, void>::fill_from_transients()
{
  if (!ptr_data_1_.empty()) {
    assert(ptr_data_1_.size() == ptr_data_2_.size() &&
           ptr_data_2_.size() == ptrs_.size() &&
           "Assns: internal inconsistency between transient and persistent "
           "member data.");
    // Multiple output modules: nothing to do on second and subsequent
    // calls.
    return;
  }
  ptr_data_t& l_ref = left_first() ? ptr_data_1_ : ptr_data_2_;
  ptr_data_t& r_ref = left_first() ? ptr_data_2_ : ptr_data_1_;
  l_ref.reserve(ptrs_.size());
  r_ref.reserve(ptrs_.size());
  for (auto const& pr : ptrs_) {
    l_ref.emplace_back(pr.first.refCore(), pr.first.key());
    r_ref.emplace_back(pr.second.refCore(), pr.second.key());
  }
}

template <typename L, typename R, typename D>
inline art::Assns<L, R, D>::Assns()
{
  static_assert(!std::is_pointer_v<D>,
                "Data template argument must not be pointer type!");
}

template <typename L, typename R, typename D>
art::Assns<L, R, D>::Assns(partner_t const& other)
  : base(other), data_(other.data_)
{}

template <typename L, typename R, typename D>
inline typename art::Assns<L, R, void>::size_type
art::Assns<L, R, D>::size() const
{
  return base::size();
}

template <typename L, typename R, typename D>
inline typename art::Assns<L, R, D>::const_iterator
art::Assns<L, R, D>::begin() const
{
  return const_iterator{*this, 0};
}

template <typename L, typename R, typename D>
inline typename art::Assns<L, R, D>::const_iterator
art::Assns<L, R, D>::end() const
{
  return const_iterator{*this};
}

template <typename L, typename R, typename D>
inline typename art::Assns<L, R, D>::const_reverse_iterator
art::Assns<L, R, D>::rbegin() const
{
  return const_reverse_iterator{*this, size()};
}

template <typename L, typename R, typename D>
inline typename art::Assns<L, R, D>::const_reverse_iterator
art::Assns<L, R, D>::rend() const
{
  return const_reverse_iterator{*this, 0};
}

template <typename L, typename R, typename D>
inline typename art::Assns<L, R, D>::data_t const&
art::Assns<L, R, D>::data(typename std::vector<data_t>::size_type index) const
{
  return data_.at(index);
}

template <typename L, typename R, typename D>
inline typename art::Assns<L, R, D>::data_t const&
art::Assns<L, R, D>::data(const_iterator it) const
{
  return data_.at(it.getIndex());
}

template <typename L, typename R, typename D>
inline void
art::Assns<L, R, D>::addSingle(Ptr<left_t> const& left,
                               Ptr<right_t> const& right,
                               data_t const& data)
{
  base::addSingle(left, right);
  data_.push_back(data);
}

template <typename L, typename R, typename D>
template <typename Ls, typename Ds>
inline void
art::Assns<L, R, D>::addMany(Ls const& lefts,
                             Ptr<right_t> const& right,
                             Ds const& data)
{
  static_assert(std::is_same_v<typename Ds::value_type, D>,
                "\n\nart error: The data argument must be a container whose "
                "value_type is D corresponding\n"
                "           to an Assns<L, R, D> object.\n");
  assert(lefts.size() == data.size());
  base::addMany(lefts, right);
  data_.insert(data_.end(), data.begin(), data.end());
}

template <typename L, typename R, typename D>
template <typename Rs, typename Ds>
void
art::Assns<L, R, D>::addMany(Ptr<left_t> const& left,
                             Rs const& rights,
                             Ds const& data)
{
  static_assert(std::is_same_v<typename Ds::value_type, D>,
                "\n\nart error: The data argument must be a container whose "
                "value_type is D corresponding\n"
                "           to an Assns<L, R, D> object.\n");
  assert(rights.size() == data.size());
  base::addMany(left, rights);
  data_.insert(data_.end(), data.begin(), data.end());
}

template <typename L, typename R, typename D>
inline void
art::Assns<L, R, D>::swap(Assns<L, R, D>& other)
{
  using std::swap;
  base::swap_(other);
  swap(data_, other.data_);
}

template <typename L, typename R, typename D>
inline std::unique_ptr<art::EDProduct>
art::Assns<L, R, D>::makePartner(
  std::type_info const& wanted_wrapper_type) const
{
  return makePartner_(wanted_wrapper_type);
}

template <typename L, typename R, typename D>
inline void
art::Assns<L, R, D>::swap_(Assns<L, R, void>& other)
{
  try {
    swap(dynamic_cast<Assns<L, R, D>&>(other));
  }
  catch (std::bad_cast const&) {
    throw Exception(errors::LogicError, "AssnsBadCast")
      << "Attempt to swap base with derived!\n";
  }
}

template <typename L, typename R, typename D>
std::unique_ptr<art::EDProduct>
art::Assns<L, R, D>::makePartner_(
  std::type_info const& wanted_wrapper_type) const
{
  using bp = typename base::partner_t;
  std::unique_ptr<art::EDProduct> result;
  if (wanted_wrapper_type == typeid(Wrapper<partner_t>)) { // Partner.
    result =
      std::make_unique<Wrapper<partner_t>>(std::make_unique<partner_t>(*this));
  } else if (wanted_wrapper_type == typeid(Wrapper<base>)) { // Base.
    result = std::make_unique<Wrapper<base>>(
      std::make_unique<base>(static_cast<base>(*this)));
  } else if (wanted_wrapper_type == typeid(Wrapper<bp>)) { // Base of partner.
    result = std::make_unique<Wrapper<bp>>(
      std::make_unique<bp>(static_cast<base>(*this)));
  } else { // Oops.
    detail::throwPartnerException(typeid(*this), wanted_wrapper_type);
  }
  return result;
}
#endif /* canvas_Persistency_Common_Assns_h */

// Local Variables:
// mode: c++
// End:

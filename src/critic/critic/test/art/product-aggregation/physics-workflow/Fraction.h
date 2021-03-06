#ifndef critic_test_art_product_aggregation_physics_workflow_Fraction_h
#define critic_test_art_product_aggregation_physics_workflow_Fraction_h

namespace arttest {

  class Fraction {
  public:
    Fraction() = default;

    Fraction(unsigned const num, unsigned const denom)
      : num_{num}, denom_{denom}
    {}

    double
    value() const
    {
      return denom_ == 0. ? 0. : static_cast<double>(num_) / denom_;
    }

    double
    numerator() const
    {
      return num_;
    }

    void
    aggregate(Fraction const& f)
    {
      num_ += f.num_;
      denom_ += f.denom_;
    }

  private:
    unsigned num_{-1u};
    unsigned denom_{-1u};
  };

} // namespace arttest

#endif /* critic_test_art_product_aggregation_physics_workflow_Fraction_h */

// Local variables:
// mode: c++
// End:

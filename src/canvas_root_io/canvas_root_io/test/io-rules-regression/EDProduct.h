#ifndef art_root_io_test_io_rules_regression_EDProduct
#define art_root_io_test_io_rules_regression_EDProduct

namespace arttest {
  class EDProduct;
}

class arttest::EDProduct {
public:
  virtual ~EDProduct() noexcept = default;

  bool
  isPresent() const
  {
    return isPresent_();
  }

private:
  virtual bool isPresent_() const = 0;
};

#endif /* art_root_io_test_io_rules_regression_EDProduct */

// Local Variables:
// mode: c++
// End:

#ifndef r10870_DataObj_h
#define r10870_DataObj_h

namespace r10870 {
  class DataObj;
} // namespace r10870

class r10870::DataObj {
public:
  DataObj() noexcept;
  DataObj(int i1, int i2, double f1) noexcept;
  int i1() const { return i1_; }
  int i2() const { return i2_; }
  int f1() const { return f1_; }
private:
  int i1_;
  int i2_;
  double f1_;
};

#endif /* r10870_DataObj_h */

// Local Variables:
// mode: c++
// End:

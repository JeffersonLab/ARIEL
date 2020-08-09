//
// Kinematics of 2 body decay.
//

#include "toyExperiment/Utilities/TwoBodyKinematics.h"

#include "cetlib/pow.h"
#include <cmath>
#include <sstream>
#include <stdexcept>

tex::TwoBodyKinematics::TwoBodyKinematics( double m0, double m1, double m2):
  m0_(m0),
  m1_(m1),
  m2_(m2){

  // Numerator of the expression for the momentum.
  double num = cet::diff_of_squares(m0_, m1_+m2 ) * cet::diff_of_squares(m0_, m1_-m2_);
  if ( num < 0. ){
    std::ostringstream out;
    out << "TwoBodyKinematics: masses of daughters exceed that of mother: \n"
        << "   Mother:     " << m0_ << " "
        << "   Daughter 1: " << m1_ << " "
        << "   Daughter 2: " << m2_ << " "
        << "   Numerator:  " << num;
    throw std::invalid_argument( out.str() );
  }

  p_  = std::sqrt(num)/2./m0_;
  e1_ = std::sqrt( cet::sum_of_squares(m1_, p_) );
  e2_ = std::sqrt( cet::sum_of_squares(m2_, p_) );
}

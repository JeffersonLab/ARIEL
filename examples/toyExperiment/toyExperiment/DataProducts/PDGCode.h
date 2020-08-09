#ifndef DataProducts_PDGCode_h
#define DataProducts_PDGCode_h
//
// A minimal version of a convenience class to allow compile time access
// to the PDG identifier codes.
//

namespace tex {

  class PDGCode {

  public:
    enum type
      {
        invalid  =    0 ,
        pi_plus  =  211 ,
        pi_minus = -211 ,
        K_plus   =  321 ,
        K_minus  = -321 ,
        phi      =  333

      };
  };

}

#endif /* DataProducts_PDGCode_h */

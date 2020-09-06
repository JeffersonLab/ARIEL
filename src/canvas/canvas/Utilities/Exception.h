#ifndef canvas_Utilities_Exception_h
#define canvas_Utilities_Exception_h

// ======================================================================
//
// Exception: art-specific customizations for cetlib/coded_exception
//
// Note that errors::ErrorCodes is tightly coupled to detail::translate()
// such that any change to one will necessitate a corresponding change to
// the other.  The actions table in Framework/Core/Actions may also need
// adjustment.
//
// ======================================================================

#include "cetlib_except/coded_exception.h"
#include <string>

// ----------------------------------------------------------------------

namespace art {
  namespace errors {

    enum ErrorCodes {
      OtherArt = 1,
      StdException,
      Unknown,
      BadAlloc,
      BadExceptionType,
      ProductNotFound,
      DictionaryNotFound,
      ProductPutFailure,
      Configuration,
      LogicError, // = 10
      UnimplementedFeature,
      InvalidReference,
      TypeConversion,
      NullPointerError,
      EventTimeout,
      DataCorruption,
      ScheduleExecutionFailure,
      EventProcessorFailure,
      EndJobFailure,
      FileOpenError, // = 20
      FileReadError,
      FatalRootError,
      MismatchedInputFiles,
      CatalogServiceError,
      ProductDoesNotSupportViews,
      ProductDoesNotSupportPtr,
      SQLExecutionError,
      InvalidNumber,
      NotFound,
      ServiceNotFound, // = 30
      ProductCannotBeAggregated,
      ProductRegistrationFailure,
      EventRangeOverlap
    };

  } // errors

  namespace ExceptionDetail {
    std::string translate(errors::ErrorCodes);
  }

  using Exception =
    cet::coded_exception<errors::ErrorCodes, ExceptionDetail::translate>;

} // art

// ======================================================================

#endif /* canvas_Utilities_Exception_h */

// Local Variables:
// mode: c++
// End:

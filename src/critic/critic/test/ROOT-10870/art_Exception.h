#ifndef r10870_Exception_h
#define r10870_Exception_h

#include "coded_exception.h"
#include <string>

namespace r10870 {
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
    r10870::coded_exception<errors::ErrorCodes, ExceptionDetail::translate>;

} // r10870

// ======================================================================

#endif /* r10870_Exception_h */

// Local Variables:
// mode: c++
// End:

#include "art_Exception.h"

using namespace r10870;

std::string
ExceptionDetail::translate(errors::ErrorCodes code)
{
  using namespace errors;

  switch (code) {
    case OtherArt:
      return "OtherArt";
    case StdException:
      return "StdException";
    case Unknown:
      return "Unknown";
    case BadAlloc:
      return "BadAlloc";
    case BadExceptionType:
      return "BadExceptionType";
    case ProductNotFound:
      return "ProductNotFound";
    case DictionaryNotFound:
      return "DictionaryNotFound";
    case ProductPutFailure:
      return "ProductPutFailure";
    case Configuration:
      return "Configuration";
    case LogicError:
      return "LogicError";
    case UnimplementedFeature:
      return "UnimplementedFeature";
    case InvalidReference:
      return "InvalidReference";
    case TypeConversion:
      return "TypeConversion";
    case NullPointerError:
      return "NullPointerError";
    case EventTimeout:
      return "EventTimeout";
    case DataCorruption:
      return "DataCorruption";
    case ScheduleExecutionFailure:
      return "ScheduleExecutionFailure";
    case EventProcessorFailure:
      return "EventProcessorFailure";
    case EndJobFailure:
      return "EndJobFailure";
    case FileOpenError:
      return "FileOpenError";
    case FileReadError:
      return "FileReadError";
    case FatalRootError:
      return "FatalRootError";
    case MismatchedInputFiles:
      return "MismatchedInputFiles";
    case CatalogServiceError:
      return "CatalogServiceError";
    case ProductDoesNotSupportViews:
      return "ProductDoesNotSupportViews";
    case ProductDoesNotSupportPtr:
      return "ProductDoesNotSupportPtr";
    case SQLExecutionError:
      return "SQLExecutionError";
    case InvalidNumber:
      return "InvalidNumber";
    case NotFound:
      return "NotFound";
    case ServiceNotFound:
      return "ServiceNotFound";
    case ProductCannotBeAggregated:
      return "ProductCannotBeAggregated";
    case ProductRegistrationFailure:
      return "ProductRegistrationFailure";
    case EventRangeOverlap:
      return "EventRangeOverlap";
  }
  throw Exception{errors::LogicError}
    << "Internal error: missing string translation for error " << code
    << " which was not caught at compile time!\n";
}

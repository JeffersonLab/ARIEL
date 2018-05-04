#include "canvas_root_io/Streamers/BranchDescriptionStreamer.h"
#include "canvas/Persistency/Provenance/BranchDescription.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "Compression.h"
#include "TBuffer.h"
#include "TClass.h"
#include "TClassStreamer.h"
#include "TDictAttributeMap.h"

namespace art {

  void
  detail::BranchDescriptionStreamer::operator()(TBuffer& R_b, void* objp)
  {
    static TClassRef cl{TClass::GetClass(typeid(BranchDescription))};
    auto obj = reinterpret_cast<BranchDescription*>(objp);
    if (R_b.IsReading()) {
      cl->ReadBuffer(R_b, obj);
      obj->fluffTransients_();
      fluffRootTransients(*obj);
    } else {
      cl->WriteBuffer(R_b, obj);
    }
  }

  void
  detail::BranchDescriptionStreamer::fluffRootTransients(
    BranchDescription const& bd)
  {
    {
      auto pcl = TClass::GetClass(bd.producedClassName().c_str());
      if (pcl == nullptr) {
        // FIXME: This is ok when the produced class is a fundamental type.
        // throw Exception(errors::DictionaryNotFound, "NoTClass")
        //    << "BranchDescription::fluffTransients() could not get TClass for
        //    " "producedClassName: "
        //    << producedClassName()
        //    << '\n';
      } else {
        auto am = pcl->GetAttributeMap();
        if (am && am->HasKey("persistent") &&
            am->GetPropertyAsString("persistent") == std::string("false")) {
          mf::LogWarning("TransientBranch")
            << "BranchDescription::fluffTransients() called for the "
               "non-persistable\n"
            << "entity: " << bd.friendlyClassName()
            << ". Please check your experiment's policy\n"
            << "on the advisability of such products.\n";
          bd.transients_.get().transient_ = true;
        }
      }
    }
    bd.transients_.get().splitLevel_ = BranchDescription::invalidSplitLevel;
    bd.transients_.get().basketSize_ = BranchDescription::invalidBasketSize;
    bd.transients_.get().compression_ = BranchDescription::invalidCompression;
    auto wcl = TClass::GetClass(bd.transients_.get().wrappedName_.c_str());
    if (wcl == nullptr) {
      // FIXME: This is ok when the produced class is a fundamental type.
      // throw Exception(errors::DictionaryNotFound, "NoTClass")
      //    << "BranchDescription::fluffTransients() could not get TClass for "
      //    << "wrappedName: "
      //    << bd.transients_.get().wrappedName_
      //    << '\n';
      return;
    }
    auto wam = wcl->GetAttributeMap();
    if (wam == nullptr) {
      // No attributes specified, all done.
      return;
    }
    if (wam->HasKey("splitLevel")) {
      bd.transients_.get().splitLevel_ =
        strtol(wam->GetPropertyAsString("splitLevel"), 0, 0);
      if (bd.transients_.get().splitLevel_ < 0) {
        throw Exception(errors::Configuration, "IllegalSplitLevel")
          << "' An illegal ROOT split level of "
          << bd.transients_.get().splitLevel_ << " is specified for class "
          << bd.transients_.get().wrappedName_ << ".'\n";
      }
      ++bd.transients_.get().splitLevel_; // Compensate for wrapper
    }
    if (wam->HasKey("basketSize")) {
      bd.transients_.get().basketSize_ =
        strtol(wam->GetPropertyAsString("basketSize"), 0, 0);
      if (bd.transients_.get().basketSize_ <= 0) {
        throw Exception(errors::Configuration, "IllegalBasketSize")
          << "' An illegal ROOT basket size of "
          << bd.transients_.get().basketSize_ << " is specified for class "
          << bd.transients_.get().wrappedName_ << "'.\n";
      }
    }
    if (wam->HasKey("compression")) {
      // FIXME: We need to check for a parsing error from the strtol() here!
      int compression = strtol(wam->GetPropertyAsString("compression"), 0, 0);
      if (compression < 0) {
        throw Exception(errors::Configuration, "IllegalCompression")
          << "' An illegal ROOT compression of " << compression
          << " is specified for class " << bd.transients_.get().wrappedName_
          << "'.\n";
      }
      int algorithm = compression / 100;
      int level = compression % 100;
      if (algorithm >= ROOT::kUndefinedCompressionAlgorithm) {
        throw Exception(errors::Configuration, "IllegalCompressionAlgorithm")
          << "' An illegal ROOT compression algorithm of " << algorithm
          << " is specified for class " << bd.transients_.get().wrappedName_
          << "'.\n";
      }
      if (level > 9) {
        throw Exception(errors::Configuration, "IllegalCompressionLevel")
          << "' An illegal ROOT compression level of " << algorithm
          << " is specified for class " << bd.transients_.get().wrappedName_
          << "'.  The compression level must between 0 and 9 inclusive.\n";
      }
      bd.transients_.get().compression_ = compression;
    }
  }

  void
  detail::setBranchDescriptionStreamer()
  {
    static TClassRef cl{TClass::GetClass(typeid(BranchDescription))};
    if (cl->GetStreamer() == nullptr) {
      cl->AdoptStreamer(new BranchDescriptionStreamer);
    }
  }

} // namespace art

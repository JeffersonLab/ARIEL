#ifndef art_root_io_RootInput_h
#define art_root_io_RootInput_h
// vim: set sw=2 expandtab :

#include "art/Framework/Core/DecrepitRelicInputSourceImplementation.h"
#include "art/Framework/Core/Frameworkfwd.h"
#include "art/Framework/Core/InputSourceDescription.h"
#include "art/Framework/IO/Catalog/InputFileCatalog.h"
#include "art_root_io/Inputfwd.h"
#include "art_root_io/RootInputFileSequence.h"
#include "canvas/Persistency/Provenance/BranchDescription.h"
#include "canvas/Persistency/Provenance/BranchType.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/KeysToIgnore.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/TableFragment.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <array>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace art {

  class RootInput final : public DecrepitRelicInputSourceImplementation {
  public:
    struct Config {

      fhicl::Atom<std::string> module_type{fhicl::Name("module_type")};
      fhicl::TableFragment<DecrepitRelicInputSourceImplementation::Config>
        drisi_config;
      fhicl::TableFragment<InputFileCatalog::Config> ifc_config;
      fhicl::TableFragment<RootInputFileSequence::Config> rifs_config;

      struct KeysToIgnore {
        std::set<std::string>
        operator()() const
        {
          return {"module_label"};
        }
      };
    };

    using Parameters = fhicl::WrappedTable<Config, Config::KeysToIgnore>;

  private:
    class AccessState {
    public:
      enum State {
        SEQUENTIAL = 0,
        SEEKING_FILE,   // 1
        SEEKING_RUN,    // 2
        SEEKING_SUBRUN, // 3
        SEEKING_EVENT   // 4
      };

      ~AccessState();
      AccessState();

      State state() const;

      void setState(State state);

      void resetState();

      EventID const& lastReadEventID() const;

      void setLastReadEventID(EventID const&);

      EventID const& wantedEventID() const;

      void setWantedEventID(EventID const&);

      std::shared_ptr<RootInputFile> rootFileForLastReadEvent() const;

      void setRootFileForLastReadEvent(std::shared_ptr<RootInputFile> const&);

    private:
      State state_{SEQUENTIAL};
      EventID lastReadEventID_{};
      std::shared_ptr<RootInputFile> rootFileForLastReadEvent_{nullptr};
      EventID wantedEventID_{};
    };

    using EntryNumber = input::EntryNumber;

  public:
    RootInput(Parameters const&, InputSourceDescription&);
    ~RootInput();

    RootInput(RootInput const&) = delete;
    RootInput(RootInput&&) = delete;

    RootInput& operator=(RootInput const&) = delete;
    RootInput& operator=(RootInput&&) = delete;

    // Find the requested event and set the system up to read run and
    // subRun records where appropriate. Note the corresponding
    // seekToEvent function must exist in RootInputFileSequence to
    // avoid a compile error.
    template <typename T>
    bool seekToEvent(T eventSpec, bool exact = false);

  private:
    void finish() override;

    input::ItemType nextItemType() override;

    std::unique_ptr<FileBlock> readFile() override;
    std::unique_ptr<RunPrincipal> readRun() override;
    std::unique_ptr<SubRunPrincipal> readSubRun(
      cet::exempt_ptr<RunPrincipal const>) override;
    std::unique_ptr<EventPrincipal> readEvent(
      cet::exempt_ptr<SubRunPrincipal const>) override;

    std::unique_ptr<RangeSetHandler> runRangeSetHandler() override;
    std::unique_ptr<RangeSetHandler> subRunRangeSetHandler() override;

    void endJob() override;

    using DecrepitRelicInputSourceImplementation::readEvent;

    input::ItemType getNextItemType() override;
    std::unique_ptr<RunPrincipal> readRun_() override;
    std::unique_ptr<SubRunPrincipal> readSubRun_(
      cet::exempt_ptr<RunPrincipal const>) override;
    std::unique_ptr<EventPrincipal> readEvent_() override;
    std::unique_ptr<FileBlock> readFile_() override;
    void closeFile_() override;
    void rewind_() override;

    std::unique_ptr<EventPrincipal> readEvent_(
      cet::exempt_ptr<SubRunPrincipal const>);

    InputFileCatalog catalog_;
    std::unique_ptr<RootInputFileSequence> primaryFileSequence_;
    AccessState accessState_{};
  };

  template <typename T>
  bool
  RootInput::seekToEvent(T const eventSpec, bool const exact)
  {
    if (accessState_.state()) {
      throw Exception(errors::LogicError)
        << "Attempted to initiate a random access seek "
        << "with one already in progress at state = " << accessState_.state()
        << ".\n";
    }
    EventID foundID = primaryFileSequence_->seekToEvent(eventSpec, exact);
    if (!foundID.isValid()) {
      return false;
    }
    if constexpr (std::is_convertible_v<T, off_t>) {
      if (eventSpec == 0 && foundID == accessState_.lastReadEventID()) {
        // We're supposed to be reading the "next" event but it's a
        // duplicate of the current one: skip it.
        mf::LogWarning("DuplicateEvent")
          << "Duplicate Events found: "
          << "both events were " << foundID << ".\n"
          << "The duplicate will be skipped.\n";
        foundID = primaryFileSequence_->seekToEvent(1, false);
      }
    }
    accessState_.setWantedEventID(foundID);
    if (primaryFileSequence_->rootFile() !=
        accessState_.rootFileForLastReadEvent()) {
      accessState_.setState(AccessState::SEEKING_FILE);
    } else if (foundID.runID() != accessState_.lastReadEventID().runID()) {
      accessState_.setState(AccessState::SEEKING_RUN);
    } else if (foundID.subRunID() !=
               accessState_.lastReadEventID().subRunID()) {
      accessState_.setState(AccessState::SEEKING_SUBRUN);
    } else {
      accessState_.setState(AccessState::SEEKING_EVENT);
    }
    return true;
  }

} // namespace art

// Local Variables:
// mode: c++
// End:

#endif /* art_root_io_RootInput_h */

include(ArtDictionary)

art_dictionary()

add_subdirectory(RootDB)

art_make_library(
  LIBRARY_NAME art_root_io_detail
  SOURCE
  detail/DummyProductCache.cc
  detail/RangeSetInfo.cc
  detail/RootErrorClassifier.cc
  detail/dropBranch.cc
  detail/getEntry.cc
  detail/getObjectRequireDict.cc
  detail/rangeSetFromFileIndex.cc
  detail/resolveRangeSet.cc
  detail/rootFileSizeTools.cc
  detail/rootOutputConfigurationTools.cc
  LIBRARIES
  art_Framework_Core
  art_root_io_RootDB
  canvas
  canvas_root_io
  hep_concurrency
  ROOT::Core
  ROOT::RIO
  ROOT::Tree)

art_make_library(
  LIBRARY_NAME art_root_io
  SOURCE
  checkDictionaries.cc
  DropMetaData.cc
  DuplicateChecker.cc
  FastCloningInfoProvider.cc
  GetFileFormatEra.cc
  GetFileFormatVersion.cc
  RootBranchInfo.cc
  RootBranchInfoList.cc
  RootDelayedReader.cc
  RootInputFile.cc
  RootInputFileSequence.cc
  RootIOPolicy.cc
  RootOutputFile.cc
  RootOutputTree.cc
  setup.cc
  LIBRARIES
  art_Framework_Core
  art_Framework_Principal
  art_Framework_IO
  art_Framework_IO_Catalog
  art_Framework_IO_detail
  art_Framework_Services_Registry
  art_Framework_Services_System_FileCatalogMetadata_service
  art_Persistency_Common
  art_Persistency_Provenance
  art_root_io_detail
  cetlib
  canvas
  fhiclcpp
  Boost::date_time
  ROOT::Hist
  ROOT::Tree)

art_make_library(
  LIBRARY_NAME art_root_io_tfile_support
  SOURCE TFileDirectory.cc detail/RootDirectorySentry.cc
  LIBRARIES cetlib_except
  ROOT::Hist
  ROOT::Core
  art_root_io
  canvas)

simple_plugin(TFileService "service"
  art_Framework_Services_System_TriggerNamesService_service
  art_root_io_tfile_support
  art_Framework_IO
  art_Framework_IO_detail
  art_Framework_Principal
  ROOT::RIO
  ROOT::Thread
  ROOT::Tree
  ALLOW_UNDERSCORES)

simple_plugin(RootInput "source" art_root_io art_Framework_IO_Catalog ALLOW_UNDERSCORES)
simple_plugin(RootOutput "module" art_root_io ALLOW_UNDERSCORES)
simple_plugin(SamplingInput "source"
  art_root_io
  ROOT::RIO
  ROOT::Tree
  SOURCE
  SamplingInput_source.cc
  detail/DataSetBroker.cc
  detail/DataSetSampler.cc
  detail/SamplingDelayedReader.cc
  detail/SamplingInputFile.cc
  detail/event_start.cc
  ALLOW_UNDERSCORES)


art_make_exec(NAME product_sizes_dumper
  SOURCE product_sizes_dumper.cc RootSizeOnDisk.cc
  LIBRARIES
  art_root_io
  cetlib
  Boost::program_options)

art_make_exec(NAME config_dumper
  LIBRARIES art_root_io
  art_Utilities canvas
  art_Framework_Core
  Boost::program_options
  ROOT::Tree
  ROOT::RIO)

art_make_exec(NAME sam_metadata_dumper
  LIBRARIES art_root_io
  art_Utilities canvas
  art_Framework_Core
  Boost::program_options
  ROOT::RIO)

art_make_exec(NAME count_events
  LIBRARIES
  art_root_io
  art_Framework_Core
  art_Utilities canvas
  Boost::program_options
  ROOT::Tree
  ROOT::RIO)

art_make_exec(NAME file_info_dumper
  SOURCE file_info_dumper.cc detail/InfoDumperInputFile.cc
  LIBRARIES art_root_io
  art_Framework_Principal
  art_Persistency_Provenance
  canvas
  cetlib
  Boost::program_options)

include(CetMakeCompletions)
cet_make_completions(product_sizes_dumper)
cet_make_completions(config_dumper)
cet_make_completions(sam_metadata_dumper)
cet_make_completions(count_events)
cet_make_completions(file_info_dumper)

install_headers(SUBDIRS detail)
install_source(SUBDIRS detail)

add_subdirectory(test)

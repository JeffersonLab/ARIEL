set(art_Framework_Services_Registry_sources
  ServiceRegistry.cc
  ServicesManager.cc
  detail/ServiceCacheEntry.cc
  detail/ensure_only_one_thread.cc)

art_make_library(LIBRARY_NAME art_Framework_Services_Registry
  SOURCE ${art_Framework_Services_Registry_sources}
  LIBRARIES ${TBB}
  art_Persistency_Provenance
  art_Utilities
  canvas)

install_headers(SUBDIRS detail)
install_source(SUBDIRS detail)

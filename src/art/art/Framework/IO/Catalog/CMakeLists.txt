set(art_Framework_IO_Catalog_sources
  InputFileCatalog.cc
  )

art_make_library( LIBRARY_NAME art_Framework_IO_Catalog
                  SOURCE ${art_Framework_IO_Catalog_sources}
                  LIBRARIES art_Framework_Services_Optional_TrivialFileDelivery_service
  art_Framework_Services_Optional_TrivialFileTransfer_service
  art_Utilities canvas
  Boost::filesystem
)

install_headers()
install_source()

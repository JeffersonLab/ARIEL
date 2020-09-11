# Link list fragment for a module making histograms
set( WORKBOOK_BASIC_ROOT_LIBRARIES
   ${ART_ROOT_IO_TFILESERVICE_SERVICE}
   ${ART_FRAMEWORK_SERVICES_REGISTRY}
   ${ART_ROOT_IO_TFILE_SUPPORT}
   ROOT::Core
   ROOT::Hist
)

add_subdirectory(AssnsSmartQueryObjects)
add_subdirectory(CreatingAssns)
add_subdirectory(SimpleDataProducts)
add_subdirectory(Debugging)
add_subdirectory(EventDisplay3D)
add_subdirectory(ExUtilities)
add_subdirectory(ExDataProducts)
add_subdirectory(FirstFilter)
add_subdirectory(FirstHistogram)
add_subdirectory(FirstModule)
add_subdirectory(FirstProducer)
add_subdirectory(FirstPtr)
add_subdirectory(GoodArtWorkflow)
add_subdirectory(IterativeAlgDevel)
add_subdirectory(LoopOverGens)
add_subdirectory(ModuleInstances)
add_subdirectory(ModuleNameCollision)
add_subdirectory(OptionalMethods)
add_subdirectory(ParameterSets)
add_subdirectory(ReadGenParticles)
add_subdirectory(RootPart2)
add_subdirectory(UsingSimpleDataProducts)
add_subdirectory(UsePDTService)
add_subdirectory(Valgrind)

# These are retained as source material for the
# upcoming exercises.  They are part of an older,
# version that is now obsolete.
#add_subdirectory(Ex08)
#add_subdirectory(Ex10)
#add_subdirectory(ExN)

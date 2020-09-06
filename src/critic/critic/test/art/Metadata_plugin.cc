#include "art/Framework/Core/FileCatalogMetadataPlugin.h"

#include "art/Framework/Principal/fwd.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <iostream>
#include <string>

namespace art::test {
  class Metadata;
}

class art::test::Metadata : public FileCatalogMetadataPlugin {
public:
  explicit Metadata(fhicl::ParameterSet const& pset);

private:
  void beginJob() override;
  void endJob() override;
  void collectMetadata(art::Event const& e) override;
  void beginRun(art::Run const& r) override;
  void endRun(art::Run const& r) override;
  void beginSubRun(art::SubRun const& sr) override;
  void endSubRun(art::SubRun const& sr) override;
  collection_type produceMetadata() override;

  unsigned const limit_;
};

art::test::Metadata::Metadata(fhicl::ParameterSet const& pset)
  : FileCatalogMetadataPlugin(pset)
  , limit_{pset.get<unsigned>("nExtraEntries", 2)}
{}

void
art::test::Metadata::beginJob()
{
  mf::LogVerbatim("Metadata") << "MetadataPlugin::beginJob()";
}

void
art::test::Metadata::endJob()
{
  mf::LogVerbatim("Metadata") << "MetadataPlugin::endJob()";
}

void
art::test::Metadata::collectMetadata(art::Event const&)
{
  mf::LogVerbatim("Metadata") << "MetadataPlugin::collectMetadata()";
}

void
art::test::Metadata::beginRun(art::Run const&)
{
  mf::LogVerbatim("Metadata") << "MetadataPlugin::beginRun()";
}

void
art::test::Metadata::endRun(art::Run const&)
{
  mf::LogVerbatim("Metadata") << "MetadataPlugin::endRun()";
}

void
art::test::Metadata::beginSubRun(art::SubRun const&)
{
  mf::LogVerbatim("Metadata") << "MetadataPlugin::beginSubRun()";
}

void
art::test::Metadata::endSubRun(art::SubRun const&)
{
  mf::LogVerbatim("Metadata") << "MetadataPlugin::endSubRun()";
}

auto
art::test::Metadata::produceMetadata() -> collection_type
{
  collection_type result;
  for (unsigned i = 0; i < limit_; ++i) {
    auto const suffix = std::to_string(i + 1);
    result.emplace_back("key" + suffix, "\"value" + suffix + "\"");
  }
  mf::LogVerbatim("Metadata") << "MetadataPlugin::produceMetadata()";
  return result;
}

DEFINE_ART_FILECATALOGMETADATA_PLUGIN(art::test::Metadata)

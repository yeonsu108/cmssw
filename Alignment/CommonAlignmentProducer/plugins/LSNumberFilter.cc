#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include <iostream>

//
// class declaration
//

class LSNumberFilter : public edm::stream::EDFilter<> {
public:
  explicit LSNumberFilter(const edm::ParameterSet&);
  ~LSNumberFilter() override;

private:
  void beginRun(edm::Run const&, edm::EventSetup const&) override;
  bool filter(edm::Event&, const edm::EventSetup&) override;
  bool is_HLT_vetoed;
  unsigned int minLS;
  const std::string veto_HLT_Menu;
  HLTConfigProvider hltConfig_;
};

LSNumberFilter::LSNumberFilter(const edm::ParameterSet& iConfig)
    : minLS(iConfig.getUntrackedParameter<unsigned>("minLS", 21)),
      veto_HLT_Menu(iConfig.getUntrackedParameter<std::string>("veto_HLT_Menu", "LumiScan")) {}

LSNumberFilter::~LSNumberFilter() {}

//
// member functions
//

// ------------ method called on each new Event  ------------
bool LSNumberFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  if ((iEvent.luminosityBlock() < minLS) || is_HLT_vetoed)
    return false;

  return true;
}

void LSNumberFilter::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {
  bool changed(false);
  hltConfig_.init(iRun, iSetup, "HLT", changed);
  std::size_t found = hltConfig_.tableName().find(veto_HLT_Menu);
  is_HLT_vetoed = (found != std::string::npos);
  if (is_HLT_vetoed) {
    edm::LogWarning("LSNumberFilter") << "Detected " << veto_HLT_Menu
                                      << " in HLT Config tableName(): " << hltConfig_.tableName()
                                      << "; Events of this run will be ignored" << std::endl;
  }
}
//define this as a plug-in
DEFINE_FWK_MODULE(LSNumberFilter);

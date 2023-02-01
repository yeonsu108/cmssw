// -*- C++ -*-
//
// Package:    MuMu/DisappearingMuonsSkimming
// Class:      DisappearingMuonsSkimming
//
/**\class DisappearingMuonsSkimming DisappearingMuonsSkimming_AOD.cc MuMu/DisappearingMuonsSkimming/plugins/DisappearingMuonsSkimming_AOD.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Michael Revering
//         Created:  Mon, 18 Jun 2018 21:22:23 GMT
//
//
// system include files
#include <memory>
#include <iomanip>
#include <iostream>

#include "Math/VectorUtil.h"
// user include files
#include "Configuration/Skimming/interface/DisappearingMuonsSkimming.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/Common/interface/SortedCollection.h"
#include "DataFormats/TrackReco/interface/Track.h"

// HCAL Stuff
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"

// for vertexing
#include "FWCore/Framework/interface/ESHandle.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"

DisappearingMuonsSkimming::DisappearingMuonsSkimming(const edm::ParameterSet& iConfig)
    : recoMuonToken_(consumes<std::vector<reco::Muon>>(iConfig.getParameter<edm::InputTag>("recoMuons"))),
      standaloneMuonToken_(consumes<std::vector<reco::Track>>(iConfig.getParameter<edm::InputTag>("StandaloneTracks"))),
      trackCollectionToken_(consumes<std::vector<reco::Track>>(iConfig.getParameter<edm::InputTag>("tracks"))),
      primaryVerticesToken_(
          consumes<std::vector<reco::Vertex>>(iConfig.getParameter<edm::InputTag>("primaryVertices"))),
      reducedEndcapRecHitCollectionToken_(
          consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("EERecHits"))),
      reducedBarrelRecHitCollectionToken_(
          consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("EBRecHits"))),
      trigResultsToken_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("TriggerResultsTag"))),
      transientTrackToken_(
          esConsumes<TransientTrackBuilder, TransientTrackRecord>(edm::ESInputTag("", "TransientTrackBuilder"))),
      geometryToken_(esConsumes<CaloGeometry, CaloGeometryRecord>(edm::ESInputTag{})) {
  muonPathsToPass_ = iConfig.getParameter<std::vector<std::string>>("muonPathsToPass");
  //Get the options
  minMuPt_ = iConfig.getParameter<double>("minMuPt");
  maxMuEta_ = iConfig.getParameter<double>("maxMuEta");
  minTrackEta_ = iConfig.getParameter<double>("minTrackEta");
  maxTrackEta_ = iConfig.getParameter<double>("maxTrackEta");
  minTrackPt_ = iConfig.getParameter<double>("minTrackPt");
  maxTransDCA_ = iConfig.getParameter<double>("maxTransDCA");
  maxLongDCA_ = iConfig.getParameter<double>("maxLongDCA");
  maxVtxChi_ = iConfig.getParameter<double>("maxVtxChi");
  minInvMass_ = iConfig.getParameter<double>("minInvMass");
  maxInvMass_ = iConfig.getParameter<double>("maxInvMass");
  trackIsoConesize_ = iConfig.getParameter<double>("trackIsoConesize");
  trackIsoInnerCone_ = iConfig.getParameter<double>("trackIsoInnerCone");
  ecalIsoConesize_ = iConfig.getParameter<double>("ecalIsoConesize");
  minEcalHitE_ = iConfig.getParameter<double>("minEcalHitE");
  maxTrackIso_ = iConfig.getParameter<double>("maxTrackIso");
  maxEcalIso_ = iConfig.getParameter<double>("maxEcalIso");
  minSigInvMass_ = iConfig.getParameter<double>("minSigInvMass");
  maxSigInvMass_ = iConfig.getParameter<double>("maxSigInvMass");
  minStandaloneDr_ = iConfig.getParameter<double>("minStandaloneDr");
  maxStandaloneDE_ = iConfig.getParameter<double>("maxStandaloneDE");
  keepOffPeak_ = iConfig.getParameter<bool>("keepOffPeak");
  keepSameSign_ = iConfig.getParameter<bool>("keepSameSign");
  keepTotalRegion_ = iConfig.getParameter<bool>("keepTotalRegion");
  keepPartialRegion_ = iConfig.getParameter<bool>("keepPartialRegion");
}

DisappearingMuonsSkimming::~DisappearingMuonsSkimming() {}

//
// member functions
//

// ------------ method called for each event  ------------
bool DisappearingMuonsSkimming::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  using namespace std;
  using namespace reco;

  totalRegion = false;
  sameSign = false;
  offPeak = false;
  partialRegion = false;

  edm::Handle<reco::TrackCollection> staTracks;
  iEvent.getByToken(standaloneMuonToken_, staTracks);
  edm::Handle<std::vector<reco::Vertex>> vertices;
  iEvent.getByToken(primaryVerticesToken_, vertices);
  edm::Handle<std::vector<reco::Muon>> recoMuons;
  iEvent.getByToken(recoMuonToken_, recoMuons);
  edm::Handle<std::vector<reco::Track>> thePATTrackHandle;
  iEvent.getByToken(trackCollectionToken_, thePATTrackHandle);
  // this wraps tracks with additional methods that are used in vertex-calculation
  const TransientTrackBuilder* transientTrackBuilder = &iSetup.getData(transientTrackToken_);

  if (!passTriggers(iEvent, trigResultsToken_, muonPathsToPass_))
    return false;

  int nMuonTrackCand = 0;
  float MuonTrackMass = 0.;

  //Looping over the reconstructed Muons
  for (std::vector<reco::Muon>::const_iterator iMuon = recoMuons->begin(); iMuon != recoMuons->end(); iMuon++) {
    if (!(iMuon->isPFMuon() && iMuon->isGlobalMuon()))
      continue;
    if (!(iMuon->passed(reco::Muon::CutBasedIdTight)))
      continue;
    if (!(iMuon->passed(reco::Muon::PFIsoTight)))
      continue;
    if (iMuon->pt() < minMuPt_ || fabs(iMuon->eta()) > maxMuEta_)
      continue;

    //Looping over tracks for any good muon
    for (std::vector<reco::Track>::const_iterator iTrack = thePATTrackHandle->begin();
         iTrack != thePATTrackHandle->end();
         ++iTrack) {
      if (!iTrack->quality(reco::Track::qualityByName("highPurity")))
        continue;
      if (fabs(iTrack->eta()) > maxTrackEta_ || fabs(iTrack->eta()) < minTrackEta_)
        continue;
      if (iTrack->pt() < minTrackPt_)
        continue;
      //Check if the track belongs to a primary vertex for isolation calculation
      bool foundtrack = false;
      GlobalPoint tkVtx;
      for (unsigned int i = 0; i < vertices->size(); i++) {
        reco::VertexRef vtx(vertices, i);
        if (!vtx->isValid()) {
          continue;
        }
        for (unsigned int j = 0; j < vtx->tracksSize(); j++) {
          if (vtx->trackRefAt(j)->pt() == iTrack->pt() && vtx->trackRefAt(j)->eta() == iTrack->eta() &&
              vtx->trackRefAt(j)->phi() == iTrack->phi()) {
            foundtrack = true;
            GlobalPoint vert(vtx->x(), vtx->y(), vtx->z());
            tkVtx = vert;
          }
        }
      }
      if (!foundtrack)
        continue;
      reco::TransientTrack tk = transientTrackBuilder->build(*iTrack);
      TrajectoryStateClosestToPoint traj = tk.trajectoryStateClosestToPoint(tkVtx);
      double transDCA = traj.perigeeParameters().transverseImpactParameter();
      double longDCA = traj.perigeeParameters().longitudinalImpactParameter();
      if (longDCA > maxLongDCA_)
        continue;
      if (transDCA > maxTransDCA_)
        continue;
      // make a pair of TransientTracks to feed the vertexer
      std::vector<reco::TransientTrack> tracksToVertex;
      tracksToVertex.push_back(transientTrackBuilder->build(*iTrack));
      if (iMuon->isGlobalMuon()) {
        tracksToVertex.push_back(transientTrackBuilder->build(iMuon->globalTrack()));
      } else {
        continue;
      }
      // try to fit these two tracks to a common vertex
      KalmanVertexFitter vertexFitter;
      CachingVertex<5> fittedVertex = vertexFitter.vertex(tracksToVertex);
      double vtxChi = 0;
      // some poor fits will simply fail to find a common vertex
      if (fittedVertex.isValid() && fittedVertex.totalChiSquared() >= 0. && fittedVertex.degreesOfFreedom() > 0) {
        // others we can exclude by their poor fit
        vtxChi = fittedVertex.totalChiSquared() / fittedVertex.degreesOfFreedom();

        if (vtxChi < maxVtxChi_) {
          // important! evaluate momentum vectors AT THE VERTEX
          TrajectoryStateClosestToPoint one_TSCP =
              tracksToVertex[0].trajectoryStateClosestToPoint(fittedVertex.position());
          TrajectoryStateClosestToPoint two_TSCP =
              tracksToVertex[1].trajectoryStateClosestToPoint(fittedVertex.position());
          GlobalVector one_momentum = one_TSCP.momentum();
          GlobalVector two_momentum = two_TSCP.momentum();

          double total_energy = sqrt(one_momentum.mag2() + 0.106 * 0.106) + sqrt(two_momentum.mag2() + 0.106 * 0.106);
          double total_px = one_momentum.x() + two_momentum.x();
          double total_py = one_momentum.y() + two_momentum.y();
          double total_pz = one_momentum.z() + two_momentum.z();
          MuonTrackMass = sqrt(pow(total_energy, 2) - pow(total_px, 2) - pow(total_py, 2) - pow(total_pz, 2));
        } else {
          continue;
        }
      } else {
        continue;
      }
      if (MuonTrackMass < minInvMass_ || MuonTrackMass > maxInvMass_)
        continue;

      double trackIso = getTrackIsolation(iEvent, vertices, iTrack);
      double ecalIso = getECALIsolation(iEvent, iSetup, transientTrackBuilder->build(*iTrack));
      if (trackIso > maxTrackIso_ || ecalIso > maxEcalIso_)
        continue;

      //A good tag/probe pair has been selected, now check for control or signal regions

      if (iMuon->charge() == iTrack->charge()) {
        sameSign = true;
      }
      if (MuonTrackMass < minSigInvMass_ || MuonTrackMass > maxSigInvMass_) {
        offPeak = true;
      }

      //If neither off-peak or same sign CR, need to check standalone muons for signal regions
      double staMinDr = 10;
      double staMinDEoverE = -10;
      if (!staTracks->empty()) {
        for (reco::TrackCollection::const_iterator staTrack = staTracks->begin(); staTrack != staTracks->end();
             ++staTrack) {
          reco::TransientTrack track = transientTrackBuilder->build(*staTrack);
          double dR = deltaR(track.impactPointTSCP().momentum().eta(),
                             track.impactPointTSCP().momentum().phi(),
                             (*iTrack).eta(),
                             (*iTrack).phi());
          double staDE = (std::sqrt(track.impactPointTSCP().momentum().mag2()) - (*iTrack).p()) / (*iTrack).p();
          if (dR < staMinDr) {
            staMinDr = dR;
          }
          //Pick the largest standalone muon within the cone
          if (dR < minStandaloneDr_ && staDE > staMinDEoverE) {
            staMinDEoverE = staDE;
          }
        }
      }
      if (staMinDr > minStandaloneDr_) {
        totalRegion = true;
      } else {
        if (staMinDEoverE < maxStandaloneDE_) {
          partialRegion = true;
        }
      }
      nMuonTrackCand++;
    }
  }

  if (nMuonTrackCand == 0)
    return false;
  bool passes = false;
  //Pass all same sign CR events
  if (sameSign && keepSameSign_) {
    passes = true;
  }
  //Pass all total disappearance events
  else if (totalRegion && keepTotalRegion_) {
    passes = true;
  }
  //Pass all partial disappearkance off-peak events
  else if (offPeak && keepOffPeak_) {
    passes = true;
  }
  //Pass partial region events that pass minimum standalone muon DE/E.
  else if (partialRegion && keepPartialRegion_) {
    passes = true;
  }
  return passes;
}

bool DisappearingMuonsSkimming::passTriggers(const edm::Event& iEvent,
                                             edm::EDGetToken m_trigResultsToken,
                                             std::vector<std::string> m_muonPathsToPass) {
  bool passTriggers = false;
  edm::Handle<edm::TriggerResults> triggerResults;
  iEvent.getByToken(m_trigResultsToken, triggerResults);
  const edm::TriggerNames& trigNames = iEvent.triggerNames(*triggerResults);
  for (size_t i = 0; i < trigNames.size(); ++i) {
    const std::string& name = trigNames.triggerName(i);
    for (auto& pathName : m_muonPathsToPass) {
      if ((name.find(pathName) != std::string::npos)) {
        if (triggerResults->accept(i)) {
          passTriggers = true;
        }
      }
    }
  }
  return passTriggers;
}

double DisappearingMuonsSkimming::getTrackIsolation(const edm::Event& iEvent,
                                                    edm::Handle<reco::VertexCollection> vtxHandle,
                                                    std::vector<reco::Track>::const_iterator& iTrack) {
  bool foundtrack = false;
  unsigned int vtxindex = -1;
  unsigned int trackindex = -1;
  double Isolation = 0;
  for (unsigned int i = 0; i < vtxHandle->size(); i++) {
    reco::VertexRef vtx(vtxHandle, i);
    if (!vtx->isValid()) {
      continue;
    }
    for (unsigned int j = 0; j < vtx->tracksSize(); j++) {
      if (fabs(vtx->trackRefAt(j)->pt() == iTrack->pt())) {
        vtxindex = i;
        trackindex = j;
        foundtrack = true;
        GlobalPoint vert(vtx->x(), vtx->y(), vtx->z());
      }
    }
  }

  if (!foundtrack) {
    return -1;
  }

  reco::VertexRef primaryVtx(vtxHandle, vtxindex);

  for (unsigned int i = 0; i < primaryVtx->tracksSize(); i++) {
    if (i == trackindex)
      continue;
    reco::TrackBaseRef secondarytrack = primaryVtx->trackRefAt(i);
    if (deltaR(iTrack->eta(), iTrack->phi(), secondarytrack->eta(), secondarytrack->phi()) > trackIsoConesize_ ||
        deltaR(iTrack->eta(), iTrack->phi(), secondarytrack->eta(), secondarytrack->phi()) < trackIsoInnerCone_)
      continue;
    Isolation += secondarytrack->pt();
  }

  return Isolation / iTrack->pt();
}

double DisappearingMuonsSkimming::getECALIsolation(const edm::Event& iEvent,
                                                   const edm::EventSetup& iSetup,
                                                   const reco::TransientTrack track) {
  edm::Handle<EcalRecHitCollection> rechitsEE;
  iEvent.getByToken(reducedEndcapRecHitCollectionToken_, rechitsEE);

  edm::Handle<EcalRecHitCollection> rechitsEB;
  iEvent.getByToken(reducedBarrelRecHitCollectionToken_, rechitsEB);

  const CaloGeometry& caloGeom = iSetup.getData(geometryToken_);
  TrajectoryStateClosestToPoint t0 = track.impactPointTSCP();
  double eDR = 0;

  for (EcalRecHitCollection::const_iterator hit = rechitsEE->begin(); hit != rechitsEE->end(); hit++) {
    const DetId id = (*hit).detid();
    const GlobalPoint hitPos = caloGeom.getSubdetectorGeometry(id)->getGeometry(id)->getPosition();
    //Check if hit and track trajectory ar in the same endcap (transient track projects both ways)
    if ((hitPos.eta() * t0.momentum().eta()) < 0) {
      continue;
    }
    TrajectoryStateClosestToPoint traj = track.trajectoryStateClosestToPoint(hitPos);
    math::XYZVector idPositionRoot(hitPos.x(), hitPos.y(), hitPos.z());
    math::XYZVector trajRoot(traj.position().x(), traj.position().y(), traj.position().z());
    if (ROOT::Math::VectorUtil::DeltaR(idPositionRoot, trajRoot) < ecalIsoConesize_ && (*hit).energy() > minEcalHitE_) {
      eDR += (*hit).energy();
    }
  }
  for (EcalRecHitCollection::const_iterator hit = rechitsEB->begin(); hit != rechitsEB->end(); hit++) {
    const DetId id = (*hit).detid();
    const GlobalPoint hitPos = caloGeom.getSubdetectorGeometry(id)->getGeometry(id)->getPosition();
    if ((hitPos.eta() * t0.momentum().eta()) < 0) {
      continue;
    }
    TrajectoryStateClosestToPoint traj = track.trajectoryStateClosestToPoint(hitPos);
    math::XYZVector idPositionRoot(hitPos.x(), hitPos.y(), hitPos.z());
    math::XYZVector trajRoot(traj.position().x(), traj.position().y(), traj.position().z());
    if (ROOT::Math::VectorUtil::DeltaR(idPositionRoot, trajRoot) < ecalIsoConesize_ && (*hit).energy() > minEcalHitE_) {
      eDR += (*hit).energy();
    }
  }

  return eDR;
}

// ------------ method called once each job just before starting event loop  ------------
void DisappearingMuonsSkimming::beginJob() {}

// ------------ method called once each job just after ending the event loop  ------------
void DisappearingMuonsSkimming::endJob() {}

//define this as a plug-in
DEFINE_FWK_MODULE(DisappearingMuonsSkimming);

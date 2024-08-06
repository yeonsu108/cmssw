import FWCore.ParameterSet.Config as cms

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer

BTVHLTOfflineSource = DQMEDAnalyzer("BTVHLTOfflineSource",

    dirname                 = cms.untracked.string("HLT/BTV"),
    processname             = cms.string("HLT"),
    verbose                 = cms.untracked.bool(False),

    triggerSummaryLabel     = cms.InputTag("hltTriggerSummaryAOD", "", "HLT"),
    triggerResultsLabel     = cms.InputTag("TriggerResults", "", "HLT"),
    onlineDiscrLabelPF      = cms.InputTag("hltParticleNetDiscriminatorsJetTags", "BvsAll"),
    offlineDiscrLabelb      = cms.InputTag("pfParticleNetAK4DiscriminatorsJetTagsForRECO", "BvsAll"),
    hltPFPVLabel            = cms.InputTag("hltVerticesPFSelector"),
    offlinePVLabel          = cms.InputTag("offlinePrimaryVertices"),
    offlineIPLabel          = cms.InputTag("pfImpactParameterTagInfos"),
    turnon_threshold_loose  = cms.double(0.1),
    turnon_threshold_medium = cms.double(0.5),
    turnon_threshold_tight  = cms.double(0.9),
    turnon_threshold_offline_loose  = cms.double(0.03),
    turnon_threshold_offline_medium = cms.double(0.2),
    turnon_threshold_offline_tight  = cms.double(0.6),
    minDecayLength          = cms.double(-9999.0),
    maxDecayLength          = cms.double(5.0),
    minJetDistance          = cms.double(0.0),
    maxJetDistance          = cms.double(0.07),
    dRTrackMatch            = cms.double(0.01),

    pathPairs = cms.VPSet(

        cms.PSet(
            pathname = cms.string("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30_v"),
            pathtype = cms.string("PF")
        ),
   ),
)

#
#  Relative Online-Offline Track Monitoring
#
from DQM.TrackingMonitorSource.TrackToTrackComparisonHists_cfi import TrackToTrackComparisonHists

referenceTracksForHLTBTag = cms.EDFilter('TrackSelector',
    src = cms.InputTag('generalTracks'),
    cut = cms.string("quality('highPurity')")
)

bTagHLTTrackMonitoring_muPF1 = TrackToTrackComparisonHists.clone(
    dzWRTPvCut               = 0.1,
    monitoredTrack           = "hltMergedTracks",
    referenceTrack           = "referenceTracksForHLTBTag",
    monitoredBeamSpot        = "hltOnlineBeamSpot",
    referenceBeamSpot        = "offlineBeamSpot",
    topDirName               = "HLT/BTV/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30PF",
    referencePrimaryVertices = "offlinePrimaryVertices",
    monitoredPrimaryVertices = "hltVerticesPFSelector",
    genericTriggerEventPSet = dict(hltPaths = ["HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30_v*"])
)

BTVEfficiency_TurnOnCurves = DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring(
        "HLT/BTV/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30*",
    ),
    verbose        = cms.untracked.uint32(0),
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "Turnon_loose   'turn-on (loose online OP);discriminator;efficiency'   Turnon_loose_Discr_numerator   Turnon_loose_Discr_denominator",
        "Turnon_medium  'turn-on (medium online OP);discriminator;efficiency'  Turnon_medium_Discr_numerator  Turnon_medium_Discr_denominator",
        "Turnon_tight   'turn-on (tight online OP);discriminator;efficiency'   Turnon_tight_Discr_numerator   Turnon_tight_Discr_denominator",
        "Turnon_loose_pt    'turn-on (loose online OP);pt;efficiency'    Turnon_loose_Pt_numerator    Turnon_loose_Pt_denominator",
        "Turnon_medium_pt   'turn-on (medium online OP);pt;efficiency'   Turnon_medium_Pt_numerator   Turnon_medium_Pt_denominator",
        "Turnon_tight_pt    'turn-on (tight online OP);pt;efficiency'    Turnon_tight_Pt_numerator    Turnon_tight_Pt_denominator",
        "Turnon_loose_eta   'turn-on (loose online OP);eta;efficiency'   Turnon_loose_Eta_numerator   Turnon_loose_Eta_denominator",
        "Turnon_medium_eta  'turn-on (medium online OP);eta;efficiency'  Turnon_medium_Eta_numerator  Turnon_medium_Eta_denominator",
        "Turnon_tight_eta   'turn-on (tight online OP);eta;efficiency'   Turnon_tight_Eta_numerator   Turnon_tight_Eta_denominator",
        "Turnon_loose_phi   'turn-on (loose online OP);phi;efficiency'   Turnon_loose_Phi_numerator   Turnon_loose_Phi_denominator",
        "Turnon_medium_phi  'turn-on (medium online OP);phi;efficiency'  Turnon_medium_Phi_numerator  Turnon_medium_Phi_denominator",
        "Turnon_tight_phi   'turn-on (tight online OP);phi;efficiency'   Turnon_tight_Phi_numerator   Turnon_tight_Phi_denominator",
    ),
)

BTVEfficiency_OnlineTrackEff = DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring(
        "HLT/BTV/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30*",
    ),
    verbose        = cms.untracked.uint32(0),
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "OnlineTrkEff_Pt   'Relative Online Track Eff vs Pt;Pt;relative efficiency'   OnlineTrkEff_Pt_numerator   OnlineTrkEff_Pt_denominator",
        "OnlineTrkEff_Eta  'Relative Online Track Eff vs Eta;Eta;relative efficiency' OnlineTrkEff_Eta_numerator   OnlineTrkEff_Eta_denominator",
        "OnlineTrkEff_3d_ip_distance  'Relative Online Track Eff vs IP3D;IP3D;relative efficiency' OnlineTrkEff_3d_ip_distance_numerator   OnlineTrkEff_3d_ip_distance_denominator",
        "OnlineTrkEff_3d_ip_sig  'Relative Online Track Eff vs IP3D signifance;IP3D significance;relative efficiency' OnlineTrkEff_3d_ip_sig_numerator   OnlineTrkEff_3d_ip_sig_denominator",
    ),
)

BTVEfficiency_OnlineTrackFake = DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring(
        "HLT/BTV/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30*",
    ),
    verbose        = cms.untracked.uint32(0),
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "OnlineTrkFake_Pt   'Relative Online Fake Rate vs Pt;Pt;relative fake rate'   OnlineTrkFake_Pt_numerator   OnlineTrkFake_Pt_denominator",
        "OnlineTrkFake_Eta  'Relative Online Fake Rate vs Eta;Eta;relative fake rate' OnlineTrkFake_Eta_numerator   OnlineTrkFake_Eta_denominator",
        "OnlineTrkFake_3d_ip_distance  'Relative Online Fake Rate vs IP3D;IP3D;relative fake rate' OnlineTrkFake_3d_ip_distance_numerator   OnlineTrkFake_3d_ip_distance_denominator",
        "OnlineTrkFake_3d_ip_sig  'Relative Online Fake Rate vs IP3D signifance;IP3D significance;relative fake rate' OnlineTrkFake_3d_ip_sig_numerator   OnlineTrkFake_3d_ip_sig_denominator",
    ),
)

from DQMOffline.Trigger.TrackingMonitoring_Client_cff import TrackToTrackEfficiencies

BJetTrackToTrackEfficiencies = TrackToTrackEfficiencies.clone(
    subDirs = ["HLT/BTV/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30*",
              ]
)

bTagHLTTrackMonitoringSequence = cms.Sequence(
    cms.ignore(referenceTracksForHLTBTag)
    + bTagHLTTrackMonitoring_muPF1
    + BTVEfficiency_TurnOnCurves
    + BTVEfficiency_OnlineTrackEff
    + BTVEfficiency_OnlineTrackFake
    + BJetTrackToTrackEfficiencies
)


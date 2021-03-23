import FWCore.ParameterSet.Config as cms

# Settings from $HERWIGPATH/LHE-MGMerging.in, including reading in the LHE file- do not use this together with Herwig7LHECommonSettings 
# User needs to include the following lines in their user settings block:
# 'set FxFxHandler:MergeMode TreeMG5' (for mlm merging) OR 'set FxFxHandler:MergeMode FxFx' (for FxFx merging)
# 'set FxFxHandler:njetsmax  MAXIMUM_NUMBER_OF_PARTONS_IN_LHE_FILE'

herwig7MGMergingSettingsBlock = cms.PSet(
    hw_mg_merging_settings = cms.vstring(
        'cd /Herwig/EventHandlers',
        'library HwFxFx.so',
        'create Herwig::FxFxEventHandler LesHouchesHandler',
        'set LesHouchesHandler:PartonExtractor /Herwig/Partons/PPExtractor',
        'set LesHouchesHandler:HadronizationHandler /Herwig/Hadronization/ClusterHadHandler',
        'set LesHouchesHandler:DecayHandler /Herwig/Decays/DecayHandler',
        'set LesHouchesHandler:WeightOption VarNegWeight',
        'set /Herwig/Generators/EventGenerator:EventHandler  /Herwig/EventHandlers/LesHouchesHandler',
        'create ThePEG::Cuts /Herwig/Cuts/NoCuts',
        'cd /Herwig/EventHandlers',
        'create Herwig::FxFxFileReader FxFxLHReader',
        'insert LesHouchesHandler:FxFxReaders[0] FxFxLHReader',
        'cd /Herwig/Shower',
        'library HwFxFxHandler.so',
        'create Herwig::FxFxHandler FxFxHandler',
        'set /Herwig/Shower/FxFxHandler:SplittingGenerator /Herwig/Shower/SplittingGenerator',
        'set /Herwig/Shower/FxFxHandler:KinematicsReconstructor /Herwig/Shower/KinematicsReconstructor',
        'set /Herwig/Shower/FxFxHandler:PartnerFinder /Herwig/Shower/PartnerFinder',
        'set /Herwig/EventHandlers/LesHouchesHandler:CascadeHandler /Herwig/Shower/FxFxHandler',
        'set /Herwig/Partons/PDFSet_nnlo:PDFName NNPDF31_nnlo_as_0118',
        'set /Herwig/Partons/RemnantDecayer:AllowTop Yes',
        'set /Herwig/Partons/PDFSet_nnlo:RemnantHandler /Herwig/Partons/HadronRemnants',
        'set /Herwig/Particles/p+:PDF /Herwig/Partons/PDFSet_nnlo',
        'set /Herwig/Particles/pbar-:PDF /Herwig/Partons/PDFSet_nnlo',
        'set /Herwig/Partons/PPExtractor:FirstPDF  /Herwig/Partons/PDFSet_nnlo',
        'set /Herwig/Partons/PPExtractor:SecondPDF /Herwig/Partons/PDFSet_nnlo',
        'set /Herwig/Shower/ShowerHandler:PDFA /Herwig/Partons/PDFSet_nnlo',
        'set /Herwig/Shower/ShowerHandler:PDFB /Herwig/Partons/PDFSet_nnlo',
        'set /Herwig/EventHandlers/FxFxLHReader:FileName cmsgrid_final.lhe',
        'set /Herwig/EventHandlers/FxFxLHReader:WeightWarnings false',
        'set /Herwig/EventHandlers/FxFxLHReader:AllowedToReOpen No',
        'set /Herwig/EventHandlers/FxFxLHReader:InitPDFs 0',
        'set /Herwig/EventHandlers/FxFxLHReader:Cuts /Herwig/Cuts/NoCuts',
        'set /Herwig/EventHandlers/FxFxLHReader:MomentumTreatment RescaleEnergy',
        'set /Herwig/EventHandlers/FxFxLHReader:PDFA /Herwig/Partons/PDFSet_nnlo',
        'set /Herwig/EventHandlers/FxFxLHReader:PDFB /Herwig/Partons/PDFSet_nnlo',
        'set /Herwig/Shower/ShowerHandler:MaxPtIsMuF Yes',
        'set /Herwig/Shower/ShowerHandler:RestrictPhasespace Yes',
        'set /Herwig/Shower/PartnerFinder:PartnerMethod Random',
        'set /Herwig/Shower/PartnerFinder:ScaleChoice Partner',
        'set /Herwig/Shower/KinematicsReconstructor:InitialInitialBoostOption LongTransBoost',
        'set /Herwig/Shower/KinematicsReconstructor:ReconstructionOption General',
        'set /Herwig/Shower/KinematicsReconstructor:InitialStateReconOption Rapidity',
        'set /Herwig/Shower/ShowerHandler:SpinCorrelations Yes',
        'cd /Herwig/Shower',
        'set /Herwig/Shower/FxFxHandler:MPIHandler  /Herwig/UnderlyingEvent/MPIHandler',
        'set /Herwig/Shower/FxFxHandler:RemDecayer  /Herwig/Partons/RemnantDecayer',
        'set /Herwig/Shower/FxFxHandler:ShowerAlpha  AlphaQCD',
        'set FxFxHandler:HeavyQVeto Yes',
        'set FxFxHandler:HardProcessDetection Automatic',
        'set FxFxHandler:drjmin 0',
        'cd /Herwig/Shower',
        'set FxFxHandler:VetoIsTurnedOff VetoingIsOn',
        'set FxFxHandler:ETClus 20*GeV', # Note this is the default, but this parameter should be tuned in future
        'set FxFxHandler:RClus 1.0',
        'set FxFxHandler:EtaClusMax 10',
        'set FxFxHandler:RClusFactor 1.5',
    )
)

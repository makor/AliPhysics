/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: Joshua Koenig                                                  *
 * Version 1.0                                                            *
 *                                                                        *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

//***************************************************************************************
// This AddTask is supposed to set up the main task
//($ALIPHYSICS/PWGGA/GammaConv/AliAnalysisTaskMesonJetCorrelation.cxx) for
// pp together with all supporting classes
//***************************************************************************************

//***************************************************************************************
// main function
//***************************************************************************************

void AddTask_MesonJetCorr_Conv(
  Int_t trainConfig = 1,                // change different set of cuts
  Int_t isMC = 0,                       // run MC
  int meson = 0,                        // meson: 0=pi0, 1 = eta
  TString photonCutNumberV0Reader = "", // 00000008400000000100000000 nom. B, 00000088400000000100000000 low B
  TString periodNameV0Reader = "",
  // general setting for task
  Int_t enableQAMesonTask = 0,             // enable QA in AliAnalysisTaskGammaConvV1
  Int_t enableQAPhotonTask = 0,            // enable additional QA task
  int enableLightOutput = kFALSE,          // switch to run light output (only essential histograms for afterburner)
  Bool_t enableTHnSparse = kFALSE,         // switch on THNsparse
  Int_t enableTriggerMimicking = 0,        // enable trigger mimicking
  Bool_t enableTriggerOverlapRej = kFALSE, // enable trigger overlap rejection
  TString settingMaxFacPtHard = "3.",      // maximum factor between hardest jet and ptHard generated
  Int_t debugLevel = 0,                    // introducing debug levels for grid running
  // settings for weights
  // FPTW:fileNamePtWeights, FMUW:fileNameMultWeights, FMAW:fileNameMatBudWeights, FEPC:fileNamedEdxPostCalib, separate with ;
  TString fileNameExternalInputs = "",
  Int_t doWeightingPart = 0,                   // enable Weighting
  TString generatorName = "DPMJET",            // generator Name
  Bool_t enableMultiplicityWeighting = kFALSE, //
  TString periodNameAnchor = "",               //
  Int_t enableMatBudWeightsPi0 = 0,            // 1 = three radial bins, 2 = 10 radial bins
  Bool_t enableElecDeDxPostCalibration = kFALSE,
  // special settings
  Bool_t enableChargedPrimary = kFALSE,
  Bool_t doSmear = kFALSE, // switches to run user defined smearing
  Double_t bremSmear = 1.,
  Double_t smearPar = 0.,      // conv photon smearing params
  Double_t smearParConst = 0., // conv photon smearing params
  // subwagon config
  TString additionalTrainConfig = "0" // additional counter for trainconfig + special settings
)
{

  AliCutHandlerPCM cuts(13);

  TString addTaskName = "AddTask_MesonJetCorr_ConvCalo";
  TString sAdditionalTrainConfig = cuts.GetSpecialSettingFromAddConfig(additionalTrainConfig, "", "", addTaskName);
  if (sAdditionalTrainConfig.Atoi() > 0) {
    trainConfig = trainConfig + sAdditionalTrainConfig.Atoi();
    cout << "INFO: " << addTaskName.Data() << " running additionalTrainConfig '" << sAdditionalTrainConfig.Atoi() << "', train config: '" << trainConfig << "'" << endl;
  }

  TString fileNamePtWeights = cuts.GetSpecialFileNameFromString(fileNameExternalInputs, "FPTW:");
  TString fileNameMultWeights = cuts.GetSpecialFileNameFromString(fileNameExternalInputs, "FMUW:");
  TString fileNamedEdxPostCalib = cuts.GetSpecialFileNameFromString(fileNameExternalInputs, "FEPC:");
  TString fileNameCustomTriggerMimicOADB = cuts.GetSpecialFileNameFromString(fileNameExternalInputs, "FTRM:");

  Int_t trackMatcherRunningMode = 0; // CaloTrackMatcher running mode
  TString strTrackMatcherRunningMode = cuts.GetSpecialSettingFromAddConfig(additionalTrainConfig, "TM", "", addTaskName);
  if (additionalTrainConfig.Contains("TM"))
    trackMatcherRunningMode = strTrackMatcherRunningMode.Atoi();

  TObjArray* rmaxFacPtHardSetting = settingMaxFacPtHard.Tokenize("_");
  if (rmaxFacPtHardSetting->GetEntries() < 1) {
    cout << "ERROR: AddTask_MesonJetCorr_pp during parsing of settingMaxFacPtHard String '" << settingMaxFacPtHard.Data() << "'" << endl;
    return;
  }
  Bool_t fMinPtHardSet = kFALSE;
  Double_t minFacPtHard = -1;
  Bool_t fMaxPtHardSet = kFALSE;
  Double_t maxFacPtHard = 100;
  Bool_t fSingleMaxPtHardSet = kFALSE;
  Double_t maxFacPtHardSingle = 100;
  Bool_t fJetFinderUsage = kFALSE;
  Bool_t fUsePtHardFromFile = kFALSE;
  Bool_t fUseAddOutlierRej = kFALSE;
  for (Int_t i = 0; i < rmaxFacPtHardSetting->GetEntries(); i++) {
    TObjString* tempObjStrPtHardSetting = (TObjString*)rmaxFacPtHardSetting->At(i);
    TString strTempSetting = tempObjStrPtHardSetting->GetString();
    if (strTempSetting.BeginsWith("MINPTHFAC:")) {
      strTempSetting.Replace(0, 10, "");
      minFacPtHard = strTempSetting.Atof();
      cout << "running with min pT hard jet fraction of: " << minFacPtHard << endl;
      fMinPtHardSet = kTRUE;
    } else if (strTempSetting.BeginsWith("MAXPTHFAC:")) {
      strTempSetting.Replace(0, 10, "");
      maxFacPtHard = strTempSetting.Atof();
      cout << "running with max pT hard jet fraction of: " << maxFacPtHard << endl;
      fMaxPtHardSet = kTRUE;
    } else if (strTempSetting.BeginsWith("MAXPTHFACSINGLE:")) {
      strTempSetting.Replace(0, 16, "");
      maxFacPtHardSingle = strTempSetting.Atof();
      cout << "running with max single particle pT hard fraction of: " << maxFacPtHardSingle << endl;
      fSingleMaxPtHardSet = kTRUE;
    } else if (strTempSetting.BeginsWith("USEJETFINDER:")) {
      strTempSetting.Replace(0, 13, "");
      if (strTempSetting.Atoi() == 1) {
        cout << "using MC jet finder for outlier removal" << endl;
        fJetFinderUsage = kTRUE;
      }
    } else if (strTempSetting.BeginsWith("PTHFROMFILE:")) {
      strTempSetting.Replace(0, 12, "");
      if (strTempSetting.Atoi() == 1) {
        cout << "using MC jet finder for outlier removal" << endl;
        fUsePtHardFromFile = kTRUE;
      }
    } else if (strTempSetting.BeginsWith("ADDOUTLIERREJ:")) {
      strTempSetting.Replace(0, 14, "");
      if (strTempSetting.Atoi() == 1) {
        cout << "using path based outlier removal" << endl;
        fUseAddOutlierRej = kTRUE;
      }
    } else if (rmaxFacPtHardSetting->GetEntries() == 1 && strTempSetting.Atof() > 0) {
      maxFacPtHard = strTempSetting.Atof();
      cout << "running with max pT hard jet fraction of: " << maxFacPtHard << endl;
      fMaxPtHardSet = kTRUE;
    }
  }

  Int_t isHeavyIon = 0;

  // ================== GetAnalysisManager ===============================
  AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    Error(Form("%s_%i", addTaskName.Data(), trainConfig), "No analysis manager found.");
    return;
  }

  // ================== GetInputEventHandler =============================
  AliVEventHandler* inputHandler = mgr->GetInputEventHandler();

  //=========  Set Cutnumber for V0Reader ================================
  TString cutnumberPhoton = photonCutNumberV0Reader.Data();
  TString cutnumberEvent = "00000003";
  AliAnalysisDataContainer* cinput = mgr->GetCommonInputContainer();

  //========= Add V0 Reader to  ANALYSIS manager if not yet existent =====
  TString V0ReaderName = Form("V0ReaderV1_%s_%s", cutnumberEvent.Data(), cutnumberPhoton.Data());
  AliV0ReaderV1* fV0ReaderV1 = NULL;
  if (!(AliV0ReaderV1*)mgr->GetTask(V0ReaderName.Data())) {
    cout << "V0Reader: " << V0ReaderName.Data() << " not found!!" << endl;
    return;
  } else {
    cout << "V0Reader: " << V0ReaderName.Data() << " found!!" << endl;
  }

  //================================================
  //========= Add task to the ANALYSIS manager =====
  //================================================
  AliAnalysisTaskMesonJetCorrelation* task = NULL;
  task = new AliAnalysisTaskMesonJetCorrelation(Form("MesonJetCorrelation_%i", trainConfig));
  task->SetIsHeavyIon(isHeavyIon);
  task->SetIsMC(isMC);
  task->SetV0ReaderName(V0ReaderName);
  task->SetTrackMatcherRunningMode(trackMatcherRunningMode); // have to do this!

  //---------------------------------------
  // configs for pi0 meson pp 13 TeV
  //---------------------------------------
  if (trainConfig == 1) {
    cuts.AddCutPCM("00010103", "0dm00009f9730000dge0474000", "0152103500000000"); // config without jet requirement
  } else if (trainConfig == 2) {
    cuts.AddCutPCM("00010103", "0dm00009f9730000dge0474000", "2r52103500000000"); // in-Jet mass cut around pi0: 0.1-0.15, rotation back
  } else if (trainConfig == 3) {
    cuts.AddCutPCM("00010103", "0dm00009f9730000dge0474000", "2152103500000000"); // in-Jet mass cut around pi0: 0.1-0.15, mixed jet back

    //---------------------------------------
    // configs for eta meson pp 13 TeV
    //---------------------------------------

  } else if (trainConfig == 1002) {
    cuts.AddCutPCM("00010103", "0dm00009f9730000dge0474000", "2r52103l00000000"); // in-Jet mass cut around eta: 0.5-0.6, rotation back
  } else if (trainConfig == 1003) {
    cuts.AddCutPCM("00010103", "0dm00009f9730000dge0474000", "2152103l00000000"); // in-Jet mass cut around pi0: 0.5-0.6, mixed jet back

  } else {
    Error(Form("MesonJetCorrelation_%i", trainConfig), "wrong trainConfig variable no cuts have been specified for the configuration");
    return;
  }

  if (!cuts.AreValid()) {
    cout << "\n\n****************************************************" << endl;
    cout << "ERROR: No valid cuts stored in CutHandlerCalo! Returning..." << endl;
    cout << "****************************************************\n\n"
         << endl;
    return;
  }

  Int_t numberOfCuts = cuts.GetNCuts();

  TList* EventCutList = new TList();
  TList* ConvCutList = new TList();
  TList* MesonCutList = new TList();

  EventCutList->SetOwner(kTRUE);
  AliConvEventCuts** analysisEventCuts = new AliConvEventCuts*[numberOfCuts];
  ConvCutList->SetOwner(kTRUE);
  AliConversionPhotonCuts** analysisConvCuts = new AliConversionPhotonCuts*[numberOfCuts];
  MesonCutList->SetOwner(kTRUE);
  AliConversionMesonCuts** analysisMesonCuts = new AliConversionMesonCuts*[numberOfCuts];

  for (Int_t i = 0; i < numberOfCuts; i++) {

    //---------------------------------------------------------//
    //------------------------ Event Cuts ---------------------//
    //---------------------------------------------------------//
    analysisEventCuts[i] = new AliConvEventCuts();
    // analysisEventCuts[i]->SetCaloTriggerHelperName(TriggerHelperName.Data());
    analysisEventCuts[i]->SetTriggerMimicking(enableTriggerMimicking);
    analysisEventCuts[i]->SetTriggerOverlapRejecion(enableTriggerOverlapRej);
    analysisEventCuts[i]->SetV0ReaderName(V0ReaderName);
    if (enableLightOutput > 0)
      analysisEventCuts[i]->SetLightOutput(kTRUE);
    if (fMinPtHardSet)
      analysisEventCuts[i]->SetMinFacPtHard(minFacPtHard);
    if (fMaxPtHardSet)
      analysisEventCuts[i]->SetMaxFacPtHard(maxFacPtHard);
    if (fSingleMaxPtHardSet)
      analysisEventCuts[i]->SetMaxFacPtHardSingleParticle(maxFacPtHardSingle);
    if (fJetFinderUsage)
      analysisEventCuts[i]->SetUseJetFinderForOutliers(kTRUE);
    if (fUsePtHardFromFile)
      analysisEventCuts[i]->SetUsePtHardBinFromFile(kTRUE);
    if (fUseAddOutlierRej)
      analysisEventCuts[i]->SetUseAdditionalOutlierRejection(kTRUE);
    if (periodNameV0Reader.CompareTo("") != 0)
      analysisEventCuts[i]->SetPeriodEnum(periodNameV0Reader);
    analysisEventCuts[i]->InitializeCutsFromCutString((cuts.GetEventCut(i)).Data());
    analysisEventCuts[i]->SetFillCutHistograms("", kFALSE);
    EventCutList->Add(analysisEventCuts[i]);

    //---------------------------------------------------------//
    //------------------- Conv Photon Cuts --------------------//
    //---------------------------------------------------------//
    analysisConvCuts[i] = new AliConversionPhotonCuts();

    // if (enableMatBudWeightsPi0 > 0){
    //   if (isMC > 0){
    // if (analysisConvCuts[i]->InitializeMaterialBudgetWeights(enableMatBudWeightsPi0,fileNameMatBudWeights)){
    //   initializedMatBudWeigths_existing = kTRUE;}
    // else {cout << "ERROR The initialization of the materialBudgetWeights did not work out." << endl;}
    //   }
    //   else {cout << "ERROR 'enableMatBudWeightsPi0'-flag was set > 0 even though this is not a MC task. It was automatically reset to 0." << endl;}
    // }

    analysisConvCuts[i]->SetV0ReaderName(V0ReaderName);
    if (enableElecDeDxPostCalibration) {
      if (isMC == 0) {
        if (fileNamedEdxPostCalib.CompareTo("") != 0) {
          analysisConvCuts[i]->SetElecDeDxPostCalibrationCustomFile(fileNamedEdxPostCalib);
          cout << "Setting custom dEdx recalibration file: " << fileNamedEdxPostCalib.Data() << endl;
        }
        analysisConvCuts[i]->SetDoElecDeDxPostCalibration(enableElecDeDxPostCalibration);
        cout << "Enabled TPC dEdx recalibration." << endl;
      } else {
        cout << "ERROR enableElecDeDxPostCalibration set to True even if MC file. Automatically reset to 0" << endl;
        enableElecDeDxPostCalibration = kFALSE;
        analysisConvCuts[i]->SetDoElecDeDxPostCalibration(kFALSE);
      }
    }
    if (enableLightOutput == 1 || enableLightOutput == 2 || enableLightOutput == 5)
      analysisConvCuts[i]->SetLightOutput(1);
    if (enableLightOutput == 4)
      analysisConvCuts[i]->SetLightOutput(2);
    if (enableLightOutput == 0)
      analysisConvCuts[i]->SetPlotTrackPID(kTRUE);
    analysisConvCuts[i]->InitializeCutsFromCutString((cuts.GetPhotonCut(i)).Data());
    analysisConvCuts[i]->SetIsHeavyIon(isHeavyIon);
    analysisConvCuts[i]->SetFillCutHistograms("", kFALSE);
    ConvCutList->Add(analysisConvCuts[i]);

    //---------------------------------------------------------//
    //------------------------ Meson Cuts ---------------------//
    //---------------------------------------------------------//
    analysisMesonCuts[i] = new AliConversionMesonCuts();
    analysisMesonCuts[i]->InitializeCutsFromCutString((cuts.GetMesonCut(i)).Data());
    analysisMesonCuts[i]->SetFillCutHistograms("");
    // analysisEventCuts[i]->SetAcceptedHeader(HeaderList);
    if (enableLightOutput > 0)
      analysisMesonCuts[i]->SetLightOutput(kTRUE);
    MesonCutList->Add(analysisMesonCuts[i]);
  }

  task->SetMesonKind(meson);
  task->SetIsConv(true);
  task->SetEventCutList(numberOfCuts, EventCutList);
  task->SetMesonCutList(numberOfCuts, MesonCutList);
  task->SetConversionCutList(numberOfCuts, ConvCutList);
  //   task->SetDoMesonAnalysis(kTRUE); // I think we dont need that!
  task->SetDoMesonQA(enableQAMesonTask); //Attention new switch for Pi0 QA
  task->SetUseTHnSparseForResponse(enableTHnSparse);

  //connect containers
  AliAnalysisDataContainer* coutput =
    mgr->CreateContainer(Form("MesonJetCorrelation_Conv_%i_%i", meson, trainConfig), TList::Class(),
                         AliAnalysisManager::kOutputContainer, Form("MesonJetCorrelation_Conv_%i_%i.root", meson, trainConfig));

  mgr->AddTask(task);
  cout << "before connect input\n";
  mgr->ConnectInput(task, 0, cinput);
  cout << "before ConnectOutput\n";
  mgr->ConnectOutput(task, 1, coutput);

  return;
}

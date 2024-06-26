#include "TDirectory.h"
#include "TMath.h"
#include "TString.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

AliAnalysisTask *AddTaskJFFlucJCAnalysis(TString taskName = "JFFlucJCAnalysis", UInt_t period = 0,
  double ptMin = 0.2, double ptMax = 5.0, double etaMin = -0.8, double etaMax = 0.8, double etaGapMin = 0.4, double etaGapMax = 0.8, std::string configArray = "0 1 2 4 5 8 11 13",
  bool saveQA = kFALSE, bool ESDpileup = false, double intercept = 15000,
  bool TPCpileup = false, bool saveQA_TPCpileup = false,
  bool Aside = false, bool Cside = false, bool saveQCNUA = false)
{
  // Less essential global variables.
  bool removeBadArea = kFALSE;
  int debug = 0;
  bool useTightCuts = kTRUE;
  double slope = 3.38; bool saveQA_ESDpileup = false;
  
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  AliJCorrectionMapTask *cMapTask = new AliJCorrectionMapTask ("JCorrectionMapTask");
  // Prepare the configuration of the wagons.
  enum { lhc15o = 0, lhc18q = 1, lhc18r = 2 };
  TString speriod[3] = { "15o", "18q", "18r" };   // Needed to load correct map config.
  std::cout << "AddTaskJFFlucJCAnalysis:: period =" << period << "\t pT range = ("
    << ptMin << "," << ptMax << ")." << std::endl;

  if (period == lhc18q || period == lhc18r) {   // 2018 PbPb datasets.
    cMapTask->EnableCentFlattening(Form(
      "alien:///alice/cern.ch/user/j/jparkkil/legotrain/Cent/CentWeights_LHC%s_pass13.root",
      speriod[period].Data() ));
    cMapTask->EnableEffCorrection(Form(
      "alien:///alice/cern.ch/user/d/djkim/legotrain/efficieny/data/Eff--LHC%s-LHC18l8-0-Lists.root",
      speriod[period].Data() ));
  }  else if (period == lhc15o) {   // 2015 PbPb datasets.
    cMapTask->EnableEffCorrection(Form(
      "alien:///alice/cern.ch/user/d/djkim/legotrain/efficieny/data/Eff--LHC%s-LHC16g-0-Lists.root",
      speriod[period].Data() ));
  }


  int iConfig = -1;
  int iOldConfig = -2;
  int index = 0;
  std::vector<TString> configNames;
  std::istringstream sConfig(configArray);

  do {
    sConfig >> iConfig;
    if (iOldConfig == iConfig) {break;}

    switch(iConfig) {   // Hardcoded names to prevent typo in phi weights files.
      // For the "syst" selection: only the changed variables is indicated, all the others are left as in default.
    case 0 :    // Default selection.     // V0M + |zVtx < 8| + (pileup > 15000)
      configNames.push_back("default");   // + global tracks 96 + (NTPC < 70) + (chi2 in [0.1, 4]).
      break;
    case 1 :    // Syst: global changed to hybrid.
      configNames.push_back("hybrid");
      break;
    case 2 :    // Syst: V0M changed to SPD clusters.
      configNames.push_back("SPD");
      break;
    case 3 :    // Syst: (pileup > 15000) changed to (no pileup cut).
      configNames.push_back("noPileup");
      break;
    case 4 :    // Syst: (pileup > 15000) changed to (pileup > 10000).
      configNames.push_back("pileup10");
      break;
    case 5 :    // Syst: |zVtx < 8| changed to |zVtx < 9|.
      configNames.push_back("zvtx9");
      break;
    case 6 :    // Syst: |zVtx < 8| changed to |zVtx < 6|.
      configNames.push_back("zvtx6");
      break;
    case 7 :    // Syst: |zVtx < 8| changed to |zVtx < 7|.
      configNames.push_back("zvtx7");
      break;
    case 8 :    // Syst: (NTPC > 70) changed to (NTPC > 80).
      configNames.push_back("NTPC80");
      break;
    case 9 :    // Syst: (NTPC > 70) changed to (NTPC > 90).
      configNames.push_back("NTPC90");
      break;
    case 10 :    // Syst: (NTPC > 70) changed to (NTPC > 100).
      configNames.push_back("NTPC100");
      break;
    case 11 :    // Syst: (chi2 in [0.1, 4]) changed to (chi2 < 4).
      configNames.push_back("chi2def");
      break;
    case 12 :    // Syst: (chi2 in [0.1, 4]) changed to (chi2 < 2.5).
      configNames.push_back("chi2tight");
      break;
    case 13 :     // Syst: (DCAz < 2cm - default in global) changed to (DCAz < 1cm).
      configNames.push_back("DCAz1");
      break;
    case 14 :     // Syst: (DCAz < 2cm - default in global) changed to (DCAz < 0.5cm).
      configNames.push_back("DCAz05");
      break;
    case 15 :     // Syst: (all charges) changed to (negative charges only).
      configNames.push_back("nqq");
      break;
    case 16 :     // Syst: (all charges) changed to (positive charges only).
      configNames.push_back("pqq");
      break;
    case 17 :     // Syst: subA. TBI
      configNames.push_back("subA");
      break;
    case 18 :     // Syst: (chi2 in [0.1, 4]) changed to (chi2 in [0.1, 2.5]).
      configNames.push_back("chi2tight01");
      break;
    case 19 :     // Syst: (chi2 in [0.1, 4]) changed to (chi2 < 2.3).
      configNames.push_back("chi2tight23");
      break;
    case 20 :     // Syst: (chi2 in [0.1, 4]) changed to (chi2 < 2.3).
      configNames.push_back("chi2tight136");
      break;
    case 21 :     // Syst: (chi2 in [0.1, 1.36])
      configNames.push_back("chi2low136");
      break;  
    case 22 :     // Syst: (chi2 in [0.1, 2.3])
      configNames.push_back("chi2low23");
      break;
    case 23 :
      configNames.push_back("hybridBaseDCA");
      break;
    case 24 :
      configNames.push_back("DCAz15");
      break;
    case 25 :
      configNames.push_back("pileup500");
      break;
    case 26 :
      configNames.push_back("NTPC65");
      break;
    case 27 :    // Syst: |zVtx < 8| changed to |zVtx < 4|. In order to check if the tracking quality make a difference to our measurements
      configNames.push_back("zvtx4");
      break;
    default :
      std::cout << "ERROR: Invalid configuration index. Skipping this element."
        << std::endl;
    }
    index++;
    iOldConfig = iConfig;
  } while (sConfig);

  const int Nsets = index;
  // Load the correction maps.
  // We assume the same maps for all minPt values.

  TString MAPfilenames[Nsets];  // Azimuthal corrections.
  TString MAPdirname = "alien:///alice/cern.ch/user/a/aonnerst/legotrain/NUAError/";

  mgr->AddTask((AliAnalysisTask *) cMapTask);
  // Set the general variables.
  int globalCut = 96;       // Global tracks - default.
  int hybridCut = 768;      // Hybrid tracks.
  UInt_t selEvt;            // Trigger.
  if (period == lhc15o) {   // Minimum bias.
    selEvt = AliVEvent::kINT7;
  } else if (period == lhc18q || period == lhc18r) {  // Minimum bias + central + semicentral.
    selEvt = AliVEvent::kINT7 | AliVEvent::kCentral | AliVEvent::kSemiCentral;
  }

  // Configure the catalyst tasks for each config.
  // taskName added in the name of the catalyst to prevent merging issues between wagons.
  AliJCatalystTask *fJCatalyst[Nsets];  // One catalyst needed per configuration.
  for (int i = 0; i < Nsets; i++) {
    MAPfilenames[i] = Form("%sPhiWeights_LHC%s_%s.root", MAPdirname.Data(), speriod[period].Data(), configNames[i].Data());
    cMapTask->EnablePhiCorrection(i, MAPfilenames[i]);  // i = 0: index for 'SetPhiCorrectionIndex(i)'.

    fJCatalyst[i] = new AliJCatalystTask(Form("JCatalystTask_%s_%s", taskName.Data(), configNames[i].Data()));
    std::cout << "Setting the catalyst: " << fJCatalyst[i]->GetJCatalystTaskName() << std::endl;
    fJCatalyst[i]->SetSaveAllQA(saveQA);
    fJCatalyst[i]->SetSaveQCNUA(saveQCNUA);
    
    /// Trigger and centrality selection.
    fJCatalyst[i]->SelectCollisionCandidates(selEvt);
    fJCatalyst[i]->SetCentrality(0.,5.,10.,20.,30.,40.,50.,60.,70.,80.,-10.,-10.,-10.,-10.,-10.,-10.,-10.);
    fJCatalyst[i]->SetInitializeCentralityArray();
    if (strcmp(configNames[i].Data(), "SPD") == 0) {
      fJCatalyst[i]->SetCentDetName("CL1");
    } else {  // Default: V0M.
      fJCatalyst[i]->SetCentDetName("V0M");
    }

    /// Event selection: pileup cuts and Zvtx.
    if (strcmp(configNames[i].Data(), "noPileup") != 0) {   // Set flag only if we cut on pileup.
      fJCatalyst[i]->AddFlags(AliJCatalystTask::FLUC_CUT_OUTLIERS);
      if (strcmp(configNames[i].Data(), "pileup10") == 0) {fJCatalyst[i]->SetESDpileupCuts(true, slope, 10000, saveQA_ESDpileup);
      } else if (strcmp(configNames[i].Data(), "pileup500") == 0) {   // Vary the cut on the ESD pileup.
        fJCatalyst[i]->SetESDpileupCuts(true, slope, 500, saveQA_ESDpileup);
      } else {fJCatalyst[i]->SetESDpileupCuts(ESDpileup, slope, intercept, saveQA_ESDpileup);}

      fJCatalyst[i]->SetTPCpileupCuts(TPCpileup, saveQA_TPCpileup); // Reject the TPC pileup.
    }
 
    if (period == lhc18q || period == lhc18r) {
      printf("Using the cent flattening!\n"); 
      fJCatalyst[i]->AddFlags(AliJCatalystTask::FLUC_CENT_FLATTENING);}    

    if (strcmp(configNames[i].Data(), "zvtx9") == 0) {    
      fJCatalyst[i]->SetZVertexCut(9.0);
    } else if (strcmp(configNames[i].Data(), "zvtx6") == 0) {
      fJCatalyst[i]->SetZVertexCut(6.0);
    } else if (strcmp(configNames[i].Data(), "zvtx7") == 0) {
      fJCatalyst[i]->SetZVertexCut(7.0);
    } else if (strcmp(configNames[i].Data(), "zvtx4") == 0) {
      fJCatalyst[i]->SetZVertexCut(4.0);
    }  else {  // Default value for JCorran analyses in Run 2.
      fJCatalyst[i]->SetZVertexCut(8.0);
    }

    /// Filtering, kinematic and detector cuts.
    if (strcmp(configNames[i].Data(), "hybrid") == 0 || strcmp(configNames[i].Data(), "hybridBaseDCA") == 0) {
      fJCatalyst[i]->SetTestFilterBit(hybridCut);
    } else {  // Default: global tracks.
      fJCatalyst[i]->SetTestFilterBit(globalCut);
    }

    if (strcmp(configNames[i].Data(), "NTPC80") == 0) {    
      fJCatalyst[i]->SetNumTPCClusters(80);
    } else if (strcmp(configNames[i].Data(), "NTPC90") == 0) {
      fJCatalyst[i]->SetNumTPCClusters(90);
    } else if (strcmp(configNames[i].Data(), "NTPC100") == 0) {
      fJCatalyst[i]->SetNumTPCClusters(100);
    } else if (strcmp(configNames[i].Data(), "NTPC65") == 0) {
      fJCatalyst[i]->SetNumTPCClusters(65);
    } else {  // Default value for JCorran analyses in Run 2.
      fJCatalyst[i]->SetNumTPCClusters(70);
    }

    if (strcmp(configNames[i].Data(), "chi2def") == 0) {    
      fJCatalyst[i]->SetChi2Cuts(0.0, 4.0);
    } else if (strcmp(configNames[i].Data(), "chi2tight") == 0) {
      fJCatalyst[i]->SetChi2Cuts(0.0, 2.5);
    } else if (strcmp(configNames[i].Data(), "chi2tight01") == 0) {
      fJCatalyst[i]->SetChi2Cuts(0.1, 2.5);
    } else if (strcmp(configNames[i].Data(), "chi2tight23") == 0) {
      fJCatalyst[i]->SetChi2Cuts(0.0, 2.3);
    } else if (strcmp(configNames[i].Data(), "chi2tight136") == 0) {
      fJCatalyst[i]->SetChi2Cuts(0.0, 1.36);
    } else if (strcmp(configNames[i].Data(), "chi2low136") == 0) {
      fJCatalyst[i]->SetChi2Cuts(0.1, 1.36);
    } else if (strcmp(configNames[i].Data(), "chi2low23") == 0) {
      fJCatalyst[i]->SetChi2Cuts(0.1, 2.3);
    } else {  // Default value for JCorran analyses in Run 2.
      fJCatalyst[i]->SetChi2Cuts(0.1, 4.0);
    }

    if (strcmp(configNames[i].Data(), "DCAz1") == 0) {    
      fJCatalyst[i]->SetDCAzCut(1.0);
    } else if (strcmp(configNames[i].Data(), "DCAz05") == 0) {
      fJCatalyst[i]->SetDCAzCut(0.5);
    } else if (strcmp(configNames[i].Data(), "DCAz15") == 0) {
      fJCatalyst[i]->SetDCAzCut(1.5);
    } else {  // Default value for JCorran analyses in Run 2.
      fJCatalyst[i]->SetDCAzCut(2.0);
    }

    if (strcmp(configNames[i].Data(), "nqq") == 0) {
      fJCatalyst[i]->SetParticleCharge(-1);
    } else if (strcmp(configNames[i].Data(), "pqq") == 0) {
      fJCatalyst[i]->SetParticleCharge(1);
    }   // Default: charge = 0 to accept all charges.

    if (strcmp(configNames[i].Data(), "hybridBaseDCA") == 0) {
      fJCatalyst[i]->SetDCABaseCuts(true);
    }

    // TBA: subA systematics.

    fJCatalyst[i]->SetPtRange(ptMin, ptMax);
    if (Aside){
      fJCatalyst[i]->SetEtaRange(0.0,0.8);
    } else if (Cside){
      fJCatalyst[i]->SetEtaRange(-0.8,0.0);
    } else {
      fJCatalyst[i]->SetEtaRange(etaMin, etaMax);
    }
    fJCatalyst[i]->SetPhiCorrectionIndex(i);
    fJCatalyst[i]->SetRemoveBadArea(removeBadArea);
    fJCatalyst[i]->SetTightCuts(useTightCuts);
    mgr->AddTask((AliAnalysisTask *)fJCatalyst[i]);
  }

// Configure the analysis task wagons.
  AliJFFlucJCTask *myTask[Nsets];
  for (int i = 0; i < Nsets; i++) {
    myTask[i] = new AliJFFlucJCTask(Form("%s_%s", 
      taskName.Data(), configNames[i].Data()));
    myTask[i]->SelectCollisionCandidates(selEvt);
    myTask[i]->SetJCatalystTaskName(fJCatalyst[i]->GetJCatalystTaskName());
    myTask[i]->SetEtaRange(etaGapMin, etaGapMax); // Used for the etagap.
   
    mgr->AddTask((AliAnalysisTask *)myTask[i]);
  }

// Create the containers for input/output.
  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
  AliAnalysisDataContainer *jHist[2*Nsets];

  for (int i = 0; i < Nsets; i++) {
    mgr->ConnectInput(fJCatalyst[i], 0, cinput);
    mgr->ConnectInput(myTask[i], 0, cinput);

    jHist[i] = new AliAnalysisDataContainer();
    jHist[i] = mgr->CreateContainer(Form("%s", myTask[i]->GetName()), TDirectory::Class(), AliAnalysisManager::kOutputContainer, 
      Form("%s:%s", AliAnalysisManager::GetCommonFileName(), myTask[i]->GetName()));
    mgr->ConnectOutput(myTask[i], 1, jHist[i]);

    jHist[Nsets+i] = new AliAnalysisDataContainer();
    jHist[Nsets+i] = mgr->CreateContainer(Form("%s", fJCatalyst[i]->GetName()), 
      TList::Class(), AliAnalysisManager::kOutputContainer, 
      Form("%s", AliAnalysisManager::GetCommonFileName()));
    mgr->ConnectOutput(fJCatalyst[i], 1, jHist[Nsets+i]);
  }

  return myTask[0];
}

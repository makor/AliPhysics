AliAnalysisVertexingHF* ConfigVertexingHF() {
  printf("Call to AliAnalysisVertexingHF parameters setting :\n");
  AliAnalysisVertexingHF* vHF = new AliAnalysisVertexingHF();
  vHF->SetMakeReducedRHF(kTRUE);
  //--- switch-off candidates finding (default: all on)
  //vHF->SetD0toKpiOff();
  vHF->SetJPSItoEleOff();
  //vHF->Set3ProngOff();
//  vHF->SetLikeSignOff(); // like-sign pairs and triplets
  vHF->Set4ProngOff();
  //vHF->SetDstarOff();
  vHF->SetFindVertexForDstar(kFALSE);
  //--- secondary vertex with KF?
  //vHF->SetSecVtxWithKF();
//  vHF->SetCascadesOff();
  vHF->SetFindVertexForCascades(kFALSE);
  vHF->SetV0TypeForCascadeVertex(AliRDHFCuts::kAllV0s);

  //--- set cuts for single-track selection  
  //     displaced tracks
  AliESDtrackCuts *esdTrackCuts = new AliESDtrackCuts("AliESDtrackCuts","default");
  esdTrackCuts->SetRequireTPCRefit(kTRUE);
  esdTrackCuts->SetMinNClustersTPC(50);
  esdTrackCuts->SetRequireITSRefit(kTRUE);
  //esdTrackCuts->SetMinNClustersITS(4);
  esdTrackCuts->SetClusterRequirementITS(AliESDtrackCuts::kSPD,
					 AliESDtrackCuts::kAny);
  esdTrackCuts->SetMinDCAToVertexXY(0.);
  esdTrackCuts->SetPtRange(0.3,1.e10);
  esdTrackCuts->SetEtaRange(-0.8,+0.8);
  AliAnalysisFilter *trkFilter = new AliAnalysisFilter("trackFilter");
  trkFilter->AddCuts(esdTrackCuts);
  vHF->SetTrackFilter(trkFilter);
  //     D* soft pion tracks
  AliESDtrackCuts *esdTrackCutsSoftPi = new AliESDtrackCuts("AliESDtrackCuts","default");
  esdTrackCutsSoftPi->SetRequireITSRefit(kTRUE);
  esdTrackCutsSoftPi->SetMinNClustersITS(2);
  esdTrackCutsSoftPi->SetEtaRange(-0.8,+0.8);
  AliAnalysisFilter *trkFilterSoftPi = new AliAnalysisFilter("trackFilterSoftPi");
  trkFilterSoftPi->AddCuts(esdTrackCutsSoftPi);
  vHF->SetTrackFilterSoftPi(trkFilterSoftPi);
  //--- set cuts for candidates selection
  AliRDHFCutsD0toKpi *cutsD0toKpi = new AliRDHFCutsD0toKpi("CutsD0toKpi");
  cutsD0toKpi->SetUsePhysicsSelection(kFALSE);
  cutsD0toKpi->SetMaxVtxZ(1.e6);
  cutsD0toKpi->SetTriggerClass("");
  Float_t cutsArrayD0toKpi[11]={0.3,999999.,1.1,0.,0.,999999.,999999.,999999.,0.,-1,0.};
  cutsD0toKpi->SetCuts(11,cutsArrayD0toKpi);
  cutsD0toKpi->AddTrackCuts(esdTrackCuts);
  vHF->SetCutsD0toKpi(cutsD0toKpi);
  AliRDHFCutsJpsitoee *cutsJpsitoee = new AliRDHFCutsJpsitoee("CutsJpsitoee");
  Float_t cutsArrayJpsitoee[9]={0.350,100000.,1.1,0.,0.,100000.,100000.,100000000.,-1.1};
  cutsJpsitoee->SetCuts(9,cutsArrayJpsitoee);
  cutsJpsitoee->AddTrackCuts(esdTrackCuts);
  vHF->SetCutsJpsitoee(cutsJpsitoee);
  AliRDHFCutsDplustoKpipi *cutsDplustoKpipi = new AliRDHFCutsDplustoKpipi("CutsDplustoKpipi");
  Float_t cutsArrayDplustoKpipi[14]={0.2,0.3,0.3,0.,0.,0.01,0.06,0.02,0.,0.7,0.,10000000000.,0.,-1.};
  cutsDplustoKpipi->SetCuts(14,cutsArrayDplustoKpipi);
  cutsDplustoKpipi->AddTrackCuts(esdTrackCuts);
  vHF->SetCutsDplustoKpipi(cutsDplustoKpipi);
  AliRDHFCutsDstoKKpi *cutsDstoKKpi = new AliRDHFCutsDstoKKpi("CutsDstoKKpi");
  Float_t cutsArrayDstoKKpi[20]={0.35,0.3,0.3,0.,0.,0.005,0.06,0.,0.,0.7,0.,1000.,0.1,0.1,-1.,1.,0.,0.,0.,-1.};
  cutsDstoKKpi->SetCuts(20,cutsArrayDstoKKpi);
  cutsDstoKKpi->AddTrackCuts(esdTrackCuts);
  vHF->SetCutsDstoKKpi(cutsDstoKKpi);
  AliRDHFCutsLctopKpi *cutsLctopKpi = new AliRDHFCutsLctopKpi("CutsLctopKpi");
  //Float_t cutsArrayLctopKpi[12]={0.2,0.4,0.4,0.,0.,0.01,0.06,0.02,0.,0.85,0.,10000000000.};
  Float_t cutsArrayLctopKpi[13]={0.18,0.4,0.5,0.,0.,0.01,0.06,0.005,0.7,0.,0.,0.05,0.4};
  cutsLctopKpi->SetCuts(13,cutsArrayLctopKpi);
  cutsLctopKpi->AddTrackCuts(esdTrackCuts);
  vHF->SetCutsLctopKpi(cutsLctopKpi);
  AliRDHFCutsD0toKpipipi *cutsD0toKpipipi = new AliRDHFCutsD0toKpipipi("CutsD0toKpipipi");
  Float_t cutsArrayD0toKpipipi[9]={0.2,0.04,0.00,0.01,0.02,0.8,0.,0.1,0.};
  cutsD0toKpipipi->SetCuts(9,cutsArrayD0toKpipipi);
  cutsD0toKpipipi->AddTrackCuts(esdTrackCuts);
  vHF->SetCutsD0toKpipipi(cutsD0toKpipipi);
  AliRDHFCutsDStartoKpipi *cutsDStartoKpipi = new AliRDHFCutsDStartoKpipi("CutsDStartoKpipi");
  Float_t cutsArrayDStartoKpipi[16]={0.3,999999.,1.1,0.,0.,999999.,999999.,999999.,0.,0.3, 0.1, 0.05, 100000000000.0, 0.5,-1.,0.}; // first 9 for D0 from D*, next 5 for D*, last 2 for D0 again
  cutsDStartoKpipi->SetCuts(16,cutsArrayDStartoKpipi);
  cutsDStartoKpipi->AddTrackCuts(esdTrackCuts);
  cutsDStartoKpipi->AddTrackCutsSoftPi(esdTrackCutsSoftPi);
  vHF->SetCutsDStartoKpipi(cutsDStartoKpipi);
  AliRDHFCutsLctoV0 *cutsLctoV0 = new AliRDHFCutsLctoV0("CutsLctoV0");
  Float_t cutsArrayLctoV0[23]={1.0,1.0,0.05,0.05,0.0,0.0,0.0,1000.,1000.,0.99,3.,1000.,0.,0.,0.,0.,9999.,-9999.,-9999.,-9999.,0.0,1000.,0.0};
  cutsLctoV0->SetCuts(23,cutsArrayLctoV0);
  cutsLctoV0->AddTrackCuts(esdTrackCuts);
  AliESDtrackCuts *esdV0daughterTrackCuts = new AliESDtrackCuts("AliESDtrackCutsForV0D","default cuts for V0 daughters");
  esdV0daughterTrackCuts->SetRequireTPCRefit(kTRUE);
  esdV0daughterTrackCuts->SetMinNClustersTPC(30);
  esdV0daughterTrackCuts->SetRequireITSRefit(kFALSE);
  esdV0daughterTrackCuts->SetMinDCAToVertexXY(0.);
  esdV0daughterTrackCuts->SetPtRange(0.05,1.e10);
  esdV0daughterTrackCuts->SetEtaRange(-1.1,+1.1);
  esdV0daughterTrackCuts->SetAcceptKinkDaughters(kTRUE);
  esdV0daughterTrackCuts->SetRequireSigmaToVertex(kFALSE);
  cutsLctoV0->AddTrackCutsV0daughters(esdV0daughterTrackCuts);
  vHF->SetCutsLctoV0(cutsLctoV0);
    
  AliRDHFCutsDplustoK0spi *cutsDplustoK0s = new AliRDHFCutsDplustoK0spi("CutsDplustoK0spi");
  Float_t cutsArrayDplustoK0s[16] = {0.3, 0.2, 0. , 5., 0., 0.99, 0., 0., 0., 3., 20., 1.1, 0., 0., 0., 0.};
  cutsDplustoK0s->SetCuts(16, cutsArrayDplustoK0s);
  cutsDplustoK0s->AddTrackCuts(esdTrackCuts);
  cutsDplustoK0s->AddTrackCutsV0daughters(esdV0daughterTrackCuts);
  vHF->SetCutsDplustoK0spi(cutsDplustoK0s);
    
  AliRDHFCutsDstoK0sK *cutsDstoK0s = new AliRDHFCutsDstoK0sK("CutsDstoK0sK");
  Float_t cutsArrayDstoK0s[16] = {0.3, 0.2, 0. , 5., 0., 0.99, 0., 0., 0., 3., 20., 1.1, 0., 0., 0., 0.};
  cutsDstoK0s->SetCuts(16, cutsArrayDstoK0s);
  cutsDstoK0s->AddTrackCuts(esdTrackCuts);
  cutsDstoK0s->AddTrackCutsV0daughters(esdV0daughterTrackCuts);
  vHF->SetCutsDstoK0sK(cutsDstoK0s);
    
  // 
  //--- set this if you want to reconstruct primary vertex candidate by
  //    candidate using other tracks in the event (for pp, broad 
  //    interaction region)
  //vHF->SetRecoPrimVtxSkippingTrks();
  //--- OR set this if you want to remove the candidate daughters from 
  //    the primary vertex, without recostructing it from scratch
  //vHF->SetRmTrksFromPrimVtx();

  //--- check the settings
  vHF->PrintStatus();
  //--- verbose
  //  AliLog::SetClassDebugLevel("AliAnalysisVertexingHF",2);

 
  return vHF;
}



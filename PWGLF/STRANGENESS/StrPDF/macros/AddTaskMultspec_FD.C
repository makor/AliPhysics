AliAnalysisTaskMultspec_FDcorr *AddTaskMultspec_FD(TString suffix = "")
{
  // analysis manager
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) { 
    ::Error("AddTaskMultspec_FD", "No analysis manager to connect to.");
    return NULL; 
  }
  if (!mgr->GetInputEventHandler()) { 
    ::Error("AddTaskMultspec_FD", "This task requires an input event handler");
    return NULL; 
  }

  // Create the task and add it to the manager
  TString tskname = Form("Multspec_Task_FD_%s", suffix.Data());
  AliAnalysisTaskMultspec_FDcorr *mytask = new AliAnalysisTaskMultspec_FDcorr(tskname, "");
  mgr->AddTask(mytask);

  // output file name
  TString outputFileName = AliAnalysisManager::GetCommonFileName();

  //create and link only used containers
  AliAnalysisDataContainer *coutput[2];

  mgr->ConnectInput(mytask, 0, mgr->GetCommonInputContainer());
  coutput[0] = mgr->CreateContainer("fHistos_misc", TList::Class(), AliAnalysisManager::kOutputContainer, outputFileName);
  coutput[1] = mgr->CreateContainer("fTree", TTree::Class(), AliAnalysisManager::kOutputContainer, outputFileName);
  mgr->ConnectOutput(mytask, 1, coutput[0]);
  mgr->ConnectOutput(mytask, 2, coutput[1]);
  
  return mytask;

}

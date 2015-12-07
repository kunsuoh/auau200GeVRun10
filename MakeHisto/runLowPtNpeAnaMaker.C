/* **************************************************
 *  A macro to run StLowPtNpeAnaMaker
 *
 *  Authors:  **Kunsu OH (kunsu OH)
 *
 *  **Code Maintainer
 *
 * **************************************************
 */

#include <TSystem>

class StMaker;
class StChain;
class StPicoDstMaker;

StChain * npeChain;

void runLowPtNpeAnaMaker(TString npeList, TString outFileName)
{
    string SL_version = "SL15c";
    string env_SL = getenv("STAR");
    if (env_SL.find(SL_version) == string::npos)
    {
        cout << "Environment Star Library does not match the requested library in runPicoNpeEventMaker.C. Exiting..." << endl;
        exit(1);
    }
    
    gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
    loadSharedLibraries();
    
    gSystem->Load("StBTofUtil");
    gSystem->Load("StPicoDstMaker");
    gSystem->Load("StLowPtNpeAnaMaker");

    npeChain = new StChain();
    
    StPicoDstMaker* picoDstMaker = new StPicoDstMaker(0,npeList,"picoDstMaker");
    StLowPtNpeAnaMaker*  LowPtNpeAnaMaker = new StLowPtNpeAnaMaker("LowPtNpeAnaMaker", picoDstMaker, outFileName.Data());
    
    // -------------- USER variables -------------------------
    
    // add your cuts here.
    npeChain->Init();
    int nEntries = LowPtNpeAnaMaker->getEntries();
    cout << " Total entries = " << nEntries << endl;
    for (int iEvent = 0; iEvent < nEntries; ++iEvent)
    {
        if(iEvent%1000==0) cout << "Working on eventNumber " << iEvent << endl;

        npeChain->Clear();
        int iret = npeChain->Make();
        if (iret)
        {
            cout << "Bad return code!" << iret << endl;
            break;
        }
    }
    
    npeChain->Finish();
    delete npeChain;
    
    // delete list of picos
    command = "rm -f correspondingPico.list";
    gSystem->Exec(command.Data());
    
}

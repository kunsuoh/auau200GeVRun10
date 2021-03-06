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
#include <exception>

class StMaker;
class StChain;
class StPicoDstMaker;

StChain * npeChain;

void runLowPtNpeAnaMaker(TString npeList="small.list", TString outFileName="test")
{
    string SL_version = "SL10k";
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
    gSystem->Load("StRefMultCorr");

    npeChain = new StChain();
    
    StPicoDstMaker* picoDstMaker = new StPicoDstMaker(0,npeList,"picoDstMaker");
    StLowPtNpeAnaMaker*  LowPtNpeAnaMaker = new StLowPtNpeAnaMaker("LowPtNpeAnaMaker", picoDstMaker, outFileName.Data());
    
    // -------------- USER variables -------------------------
    
    npeChain->Init();
    int nEntries = picoDstMaker->chain()->GetEntries();
    cout << " Total entries = " << nEntries << endl;
    
    for (int iEvent = 0; iEvent < nEntries; ++iEvent)
    {
        if(iEvent%100==0) cout << "Working on eventNumber " << iEvent << endl;

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

    
}

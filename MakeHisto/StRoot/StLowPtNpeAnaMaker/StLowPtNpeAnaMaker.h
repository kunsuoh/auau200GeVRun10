#ifndef StLowPtNpeAnaMaker_h
#define StLowPtNpeAnaMaker_h

/* **************************************************
 *  A Maker that reads StPicoEvents and creates
 *  StLowPtNpeAnas and stores them.
 *
 *  Authors:  **Kunsu OH        (kunsuoh@gmail.com)
 *
 *  **Code Maintainer
 * **************************************************
 */

#include "StMaker.h"
#include "TH2F.h"
#include <exception>
#include "THnSparse.h"

class TTree;
class TFile;
class StPicoDstMaker;
class StElectronPair;
class StPicoNpeHists;
class StRefMultCorr;


class StLowPtNpeAnaMaker : public StMaker
{
public:
    StLowPtNpeAnaMaker(char const* makerName, StPicoDstMaker* picoMaker, char const* fileBaseName);
    virtual ~StLowPtNpeAnaMaker();
    
    virtual Int_t Init();
    virtual Int_t Make();
    virtual void  Clear(Option_t *opt="");
    virtual Int_t Finish();

    
private:
    void  getCalTofTrack(StPicoTrack const*, float&) const;
    bool  isGoodEvent();
    bool  isGoodTrack(StPicoTrack const*) const;
    bool  isElectron(StPicoTrack const*) const;
    bool  isTaggedElectron(StPicoTrack const*) const;
    bool  isPartnerElectron(StPicoTrack const*) const;
    bool  isTofMatching(StPicoTrack const * const trk, float cutY, float cutZ) const;
    bool  isTofPid(StPicoTrack const *, float) const;
    bool  isTpcPid(StPicoTrack const *, float) const;
    bool  isGoodElectronPair(StElectronPair const &, float) const;
    void  fillHistogram(StPicoTrack const*) const;
    bool  isTofEvent();
    void  loadTofEvent();
    
    bool mPhE;
    int iCent;
    double weight;
    int nbin;
    int mQa_runID[3000];
    float mQa_west[3000], mQa_east[3000];
    int mTofcal;
    
    StPicoDstMaker* mPicoDstMaker;
    StPicoEvent*    mPicoEvent;
    
    TFile* mOutputFile;
    TTree* mTree;
    
    StRefMultCorr* refmultcorr;
//    TH2F * histoAll[7][5][102];
//    TH2F * histoPureEU[7][5][102];
//    TH2F * histoPureEL[7][5][102];
//    TH2F * histoPhEU[7][5][102];
//    TH2F * histoPhEL[7][5][102];
    
    THnSparse * hs;
    
    TH1D * hEvent;
    TH1D * hEventVz;
    TH2D * hEventVzVpdVz;
    TH1D * hEventCentrality;
    TH1D * hEventCentralityCorr;
    TH1D * hEventRefMult;
    TH1D * hEventRefMultCorr;
    TH1D * hEventWeight;
    
    
    ClassDef(StLowPtNpeAnaMaker, 0)
};

#endif

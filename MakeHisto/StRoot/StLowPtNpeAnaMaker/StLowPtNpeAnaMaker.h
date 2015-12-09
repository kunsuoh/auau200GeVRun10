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
    
    TH2F * histoAll[7][5][102];
    TH2F * histoPhEU[7][5][102];
    TH2F * histoPhEL[7][5][102];
    
    int iCent;
    double weight;
    int nbin;
    
    StRefMultCorr* refmultcorr;
    float tray[3000][120],qa_west[3000], qa_east[3000];
    int tofcal;
    
private:
    void  getCalTofTrack(StPicoTrack const*, float&) const;
    bool  isTofEvent() const;
    void  loadTofEvent() const;
    bool  isGoodEvent() const;
    bool  isGoodTrack(StPicoTrack const*) const;
    bool  isElectron(StPicoTrack const*) const;
    bool  isTaggedElectron(StPicoTrack const*) const;
    bool  isPartnerElectron(StPicoTrack const*) const;
    bool  isTofMatching(StPicoTrack const * const trk, float cutY, float cutZ) const;
    bool  isTofPid(StPicoTrack const *, float) const;
    bool  isTpcPid(StPicoTrack const *, float) const;

    bool  isGoodElectronPair(StElectronPair const &, float) const;
    void  fillHistogram(StPicoTrack const*) const;
    
    StPicoDstMaker* mPicoDstMaker;
    StPicoEvent*    mPicoEvent;
    StPicoNpeHists*  mPicoNpeHists;
    
    TFile* mOutputFile;
    TTree* mTree;

    int mQa_runID[3000];
    float mQa_west[3000], mQa_east[3000];
    
    
    ClassDef(StLowPtNpeAnaMaker, 0)
};

#endif

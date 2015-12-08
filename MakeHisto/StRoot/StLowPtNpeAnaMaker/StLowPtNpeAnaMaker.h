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
    float ptbin[103] = {0, 0.2, 0.21, 0.22, 0.23, 0.24, 0.25, 0.26, 0.27, 0.28, 0.29, 0.3, 0.31, 0.32, 0.33, 0.34, 0.35, 0.36, 0.37, 0.38, 0.39, 0.4, 0.41, 0.42, 0.43, 0.44, 0.45, 0.46, 0.47, 0.48, 0.49, 0.5, 0.51, 0.52, 0.53, 0.54, 0.55, 0.56, 0.57, 0.58, 0.59, 0.6, 0.62, 0.64, 0.66, 0.68, 0.7, 0.72, 0.74, 0.76, 0.78, 0.8, 0.82, 0.84, 0.86, 0.88, 0.9, 0.92, 0.94, 0.96, 0.98, 1, 1.05, 1.1,  1.15,  1.2,  1.25,  1.3,  1.35,  1.4,  1.45,  1.5,  1.55,  1.6,  1.65,  1.7,  1.75,  1.8,  1.85,  1.9,  1.95, 2,  2.1,  2.2,  2.3,  2.4,  2.5,  2.6, 2.7, 2.8, 2.9, 3, 3.1, 3.2, 3.3, 3.4, 3.5, 3.75, 4, 4.25, 4.5, 4.75, 5};

private:
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
    
    ClassDef(StLowPtNpeAnaMaker, 0)
};

#endif

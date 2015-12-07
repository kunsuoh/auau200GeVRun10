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

class TTree;
class TFile;
class StPicoDstMaker;
class StPicoEvent;
class StPicoTrack;
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
    
private:
    bool  isGoodEvent() const;
    bool  isGoodTrack(StPicoTrack const*) const;
    bool  isElectron(StPicoTrack const*) const;
    bool  isPartnerElectron(StPicoTrack const*) const;
    bool  isGoodElectronPair(StElectronPair const &, float) const;
    bool  isGoodQaElectronPair(StElectronPair const&, StPicoTrack const&,StPicoTrack const&) const;
    
    StPicoDstMaker* mPicoDstMaker;
    StPicoEvent*    mPicoEvent;
    StPicoNpeHists*  mPicoNpeHists;
    
    TFile* mOutputFile;
    TTree* mTree;
    
    ClassDef(StLowPtNpeAnaMaker, 0)
};

#endif

#include <vector>
#include <cmath>

#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include "StThreeVectorF.hh"
#include "StLorentzVectorF.hh"
#include "../StPicoDstMaker/StPicoDst.h"
#include "../StPicoDstMaker/StPicoDstMaker.h"
#include "../StPicoDstMaker/StPicoEvent.h"
#include "../StPicoDstMaker/StPicoTrack.h"

#include "StLowPtNpeAnaMaker.h"
#include "StElectronPair.h"
#include "StCuts.h"

ClassImp(StLowPtNpeAnaMaker)

//-----------------------------------------------------------------------------
StLowPtNpeAnaMaker::StLowPtNpeAnaMaker(char const* makerName, StPicoDstMaker* picoMaker, char const* fileBaseName)
: StMaker(makerName), mPicoDstMaker(picoMaker), mPicoEvent(NULL),
mOutputFile(NULL)
{
    
    TString baseName(fileBaseName);
    mOutputFile = new TFile(Form("%s.hist.root",fileBaseName), "RECREATE");
}

//-----------------------------------------------------------------------------
StLowPtNpeAnaMaker::~StLowPtNpeAnaMaker()
{
    /* mTree is owned by mOutputFile directory, it will be destructed once
     * the file is closed in ::Finish() */
}

//-----------------------------------------------------------------------------
Int_t StLowPtNpeAnaMaker::Init()
{
    return kStOK;
}

//-----------------------------------------------------------------------------
Int_t StLowPtNpeAnaMaker::Finish()
{
    mOutputFile->cd();
    // write histograms
    
    
    mOutputFile->Close();
    return kStOK;
}
//-----------------------------------------------------------------------------
void StLowPtNpeAnaMaker::Clear(Option_t *opt)
{
}

//-----------------------------------------------------------------------------
Int_t StLowPtNpeAnaMaker::Make()
{
    if (!mPicoDstMaker)
    {
        LOG_WARN << " No PicoDstMaker! Skip! " << endm;
        return kStWarn;
    }
    
    StPicoDst const * picoDst = mPicoDstMaker->picoDst();
    if (!picoDst)
    {
        LOG_WARN << " No PicoDst! Skip! " << endm;
        return kStWarn;
    }
    
    mPicoEvent = picoDst->event();
    
    if (isGoodEvent())
    {
        UInt_t nTracks = picoDst->numberOfTracks();
        
        std::vector<unsigned short> idxPicoTaggedEs;
        std::vector<unsigned short> idxPicoPartnerEs;
        
        for (unsigned short iTrack = 0; iTrack < nTracks; ++iTrack)
        {
            StPicoTrack const* trk = picoDst->track(iTrack);
            if (!trk || !isGoodTrack(trk)) continue;
            if (isElectron(trk))
            {
                fillHistogram(trk);
                if (isTaggedElectron(trk)) idxPicoTaggedEs.push_back(iTrack);
            }
            
            if (isPartnerElectron(trk)) idxPicoPartnerEs.push_back(iTrack);
        } // .. end tracks loop
        
        
        float const bField = mPicoEvent->bField();
        
        for (unsigned short ik = 0; ik < idxPicoTaggedEs.size(); ++ik)
        {
            
            StPicoTrack const * electron = picoDst->track(idxPicoTaggedEs[ik]);
            
            // make electron pairs
            for (unsigned short ip = 0; ip < idxPicoPartnerEs.size(); ++ip)
            {
                
                if (idxPicoTaggedEs[ik] == idxPicoPartnerEs[ip]) continue;
                
                StPicoTrack const * partner = picoDst->track(idxPicoPartnerEs[ip]);
                
                StElectronPair electronPair(electron, partner, idxPicoTaggedEs[ik], idxPicoPartnerEs[ip], bField);
                
                if (!isGoodElectronPair(electronPair, electron->gMom().perp())) continue;
                
            } // .. end make electron pairs
        } // .. end of tagged e loop
        
        idxPicoTaggedEs.clear();
        idxPicoPartnerEs.clear();
    } //.. end of good event fill
    
    
    return kStOK;
}

//-----------------------------------------------------------------------------
bool StLowPtNpeAnaMaker::isGoodEvent() const
{
    return
    fabs(mPicoEvent->primaryVertex().z()) < cuts::vz &&
    fabs(mPicoEvent->primaryVertex().z() - mPicoEvent->vzVpd()) < cuts::vzVpdVz;
}
//-----------------------------------------------------------------------------
bool StLowPtNpeAnaMaker::isGoodTrack(StPicoTrack const * const trk) const
{
    return trk->gMom().perp() > cuts::pt &&
    trk->nHitsFit() >= cuts::nHitsFit &&
    (float)trk->nHitsFit()/(float)trk->nHitsMax() > cuts::nHitsRatioMin &&
    (float)trk->nHitsFit()/(float)trk->nHitsMax() < cuts::nHitsRatioMax ;
}

//-----------------------------------------------------------------------------
bool StLowPtNpeAnaMaker::isElectron(StPicoTrack const * const trk) const
{
    return
    isGoodTrack(trk) &&
    trk->gMom().pseudoRapidity() < cuts::etaTagged &&
    trk->nHitsDedx() >= cuts::nHitsDedx &&
    trk->dca() < cuts::globalDca &&
    !(trk->gMom().phi() > cuts::phiMin1 && trk->gMom().phi < cuts::phiMax1) &&
    !(trk->gMom().phi() > cuts::phiMin2 && trk->gMom().phi < cuts::phiMax2) ;
}

//-----------------------------------------------------------------------------
bool StLowPtNpeAnaMaker::isTaggedElectron(StPicoTrack const * const trk) const
{
    return
    isElectron(trk) &&
    isTpcPid(trk, cuts::nSigmaTaggedElectron) &&
    isTofMatching(trk, cuts::ylocal, cuts::zlocal) &&
    isTofPid(trk, cuts::beta) ;
}

//-----------------------------------------------------------------------------
bool StLowPtNpeAnaMaker::isPartnerElectron(StPicoTrack const * const trk) const
{
    return
    isGoodTrack(trk) &&
    trk->gMom().pseudoRapidity() < cuts::etaPartner &&
    isTpcPid(trk, cuts::nSigmaPartnerElectron) ;
}

//-----------------------------------------------------------------------------
bool StLowPtNpeAnaMaker::isTofMatching(StPicoTrack const * const trk, float cutY, float cutZ) const
{
    return
    fabs(trk->btofYLocal()) < cutY &&
    fabs(trk->btofZLocal()) < cutZ ;
}
//-----------------------------------------------------------------------------
bool StLowPtNpeAnaMaker::isTofPid(StPicoTrack const * const trk, float cut) const
{
    return
    fabs(1-1/trk->btofBeta()) < cut ;
}
//-----------------------------------------------------------------------------
bool StLowPtNpeAnaMaker::isTpcPid(StPicoTrack const * const trk, float cut) const
{
    return
    fabs(trk->nSigmaElectron()) < cut;
}
//-----------------------------------------------------------------------------
bool StLowPtNpeAnaMaker::isGoodElectronPair(StElectronPair const & epair, float pt) const
{
    return
    epair.pairMass() < cuts::pairMass &&
    epair.pairDca() < cuts::pairDca;
}
//-----------------------------------------------------------------------------
void  fillHistogram(StPicoTrack const * const trk) const
{
    
}

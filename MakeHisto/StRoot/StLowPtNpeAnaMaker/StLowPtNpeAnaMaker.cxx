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
#include "../StRefMultCorr/StRefMultCorr.h"

#include "StLowPtNpeAnaMaker.h"
#include "StElectronPair.h"
#include "StCuts.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern const float ptbin[] = {0, 0.2, 0.21, 0.22, 0.23, 0.24, 0.25, 0.26, 0.27, 0.28, 0.29, 0.3, 0.31, 0.32, 0.33, 0.34, 0.35, 0.36, 0.37, 0.38, 0.39, 0.4, 0.41, 0.42, 0.43, 0.44, 0.45, 0.46, 0.47, 0.48, 0.49, 0.5, 0.51, 0.52, 0.53, 0.54, 0.55, 0.56, 0.57, 0.58, 0.59, 0.6, 0.62, 0.64, 0.66, 0.68, 0.7, 0.72, 0.74, 0.76, 0.78, 0.8, 0.82, 0.84, 0.86, 0.88, 0.9, 0.92, 0.94, 0.96, 0.98, 1, 1.05, 1.1,  1.15,  1.2,  1.25,  1.3,  1.35,  1.4,  1.45,  1.5,  1.55,  1.6,  1.65,  1.7,  1.75,  1.8,  1.85,  1.9,  1.95, 2,  2.1,  2.2,  2.3,  2.4,  2.5,  2.6, 2.7, 2.8, 2.9, 3, 3.1, 3.2, 3.3, 3.4, 3.5, 3.75, 4, 4.25, 4.5, 4.75, 5};
#ifdef __cplusplus
}
#endif

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
    nbin = 102;

    for (int i=0 ; i<7 ; i++)
    for (int j=0 ; j<5 ; j++)
    for (int k=0 ; k<102 ; k++)
    {
        histoAll[i][j][k] = new TH2F(Form("histo%d_eta%d_pt%d",i,j,k), Form("histo%d_eta%d_pt%d",i,j,k) ,800, -0.2, 0.6, 289, -13, 13);
    }
    return kStOK;
}

//-----------------------------------------------------------------------------
Int_t StLowPtNpeAnaMaker::Finish()
{
    mOutputFile->cd();
    // write histograms
    for (int i=0 ; i<7 ; i++)
    for (int j=0 ; j<5 ; j++)
    for (int k=0 ; k<102 ; k++)
    {
        histoAll[i][j][k]->Write();
    }

    mOutputFile->Write();
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
        float refmult = mPicoEvent->refMult();
        float vZ = mPicoEvent->primaryVertex().z();
        float zdcCoincidenceRate = mPicoEvent->ZDCx();

        refmultcorr = new StRefMultCorr();
        refmultcorr->init(mPicoEvent->runId());

        refmultcorr->initEvent(refmult, vZ, zdcCoincidenceRate) ;
        iCent = refmultcorr->getCentralityBin6();
        weight = refmultcorr->getWeight();

        UInt_t nTracks = picoDst->numberOfTracks();
        
        std::vector<unsigned short> idxPicoTaggedEs;
        std::vector<unsigned short> idxPicoPartnerEs;
        for (unsigned short iTrack = 0; iTrack < nTracks; ++iTrack)
        {
            StPicoTrack const* trk = picoDst->track(iTrack);
            if (!trk) continue;
            cout << nTracks << " " << iTrack << endl;
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
    trk->gMom().phi() < cuts::phiMin1 && trk->gMom().phi() > cuts::phiMax1 &&
    trk->gMom().phi() < cuts::phiMin2 && trk->gMom().phi() > cuts::phiMax2 ;
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
void  StLowPtNpeAnaMaker::fillHistogram(StPicoTrack const * const trk) const
{
    float pt = trk->gMom().perp();
    float eta = trk->gMom().pseudoRapidity();
    
    cout << pt << " " << eta << endl;

    if (pt > 5.) return;
    if (eta > 0.5 || eta < -0.5) return;

    float TOF = trk->btof();
    float PathL = trk->btofBeta()*TOF;
    float pp = trk->gMom().mag();
    
    float dbeta = 1 - PathL/TOF/2.99792458e1*TMath::Sqrt(1-0.000511*0.000511/pp/pp);
    float nSigmaElectron = trk->nSigmaElectron();
    
    int iPt=0;
    int iEta=0;
    
    for (int k=0;k<nbin+1;k++) {
        if (pt < ptbin[k]) {
            iPt=k-1;
            break;
        }
    }
    for (int k=0;k<5;k++) if (TMath::Abs(eta) < k*0.1 + 0.1) {iEta=k;break;}
    
    cout << iPt << " " << iEta << endl;
    
    histoAll[iCent][iEta][iPt]->Fill(dbeta,nSigmaElectron,weight);
    histoAll[0][iEta][iPt]->Fill(dbeta,nSigmaElectron,weight);

}

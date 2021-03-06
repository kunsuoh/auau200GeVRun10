#include <vector>
#include <cmath>
#include <exception>

#include "THnSparse.h"
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


ClassImp(StLowPtNpeAnaMaker)

//-----------------------------------------------------------------------------
StLowPtNpeAnaMaker::StLowPtNpeAnaMaker(char const* makerName, StPicoDstMaker* picoMaker, char const* fileBaseName)
: StMaker(makerName), mPicoDstMaker(picoMaker), mPicoEvent(NULL),
mOutputFile(NULL), mTofcal(NULL),mPhE(false)
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
    
    hEvent = new TH1D("hEvent", "hEvent", 10,0,10);
    hEventVz = new TH1D("hEventVz", "hEventVz", 100,-100,100);
    hEventVzVpdVz = new TH2D("hEventVzVpdVz", "hEventVzVpdVz", 100,-100,100, 100,-100,100);
    hEventCentrality = new TH1D("hEventCentrality", "hEventCentrality", 10,0,10);
    hEventCentralityCorr = new TH1D("hEventCentralityCorr", "hEventCentralityCorr", 10,0,10);
    hEventRefMult = new TH1D("hEventRefMult", "hEventRefMult", 1000,0,1000);
    hEventRefMultCorr = new TH1D("hEventRefMultCorr", "hEventRefMultCorr", 1000,0,1000);
    hEventWeight = new TH1D("hEventWeight", "hEventWeight", 100,0,10);
    for (int i=0 ; i<7 ; i++)
    for (int j=0 ; j<5 ; j++)
    for (int k=0 ; k<102 ; k++)
    {
 //       cout << i << " " << j << " " << k << endl;
 //       histoAll[i][j][k] = new TH2F(Form("histo%d_eta%d_pt%d",i,j,k), Form("histo%d_eta%d_pt%d",i,j,k) ,800, -0.2, 0.6, 289, -13, 13);
 //       histoPureEU[i][j][k] = new TH2F(Form("histoPureEU%d_eta%d_pt%d",i,j,k), Form("histoPureEU%d_eta%d_pt%d",i,j,k) ,800, -0.2, 0.6, 289, -13, 13);
 //       histoPureEL[i][j][k] = new TH2F(Form("histoPureEL%d_eta%d_pt%d",i,j,k), Form("histoPureEL%d_eta%d_pt%d",i,j,k) ,800, -0.2, 0.6, 289, -13, 13);
 //       histoPhEU[i][j][k] = new TH2F(Form("histoPhEU%d_eta%d_pt%d",i,j,k), Form("histoPhEU%d_eta%d_pt%d",i,j,k) ,800, -0.2, 0.6, 289, -13, 13);
 //       histoPhEL[i][j][k] = new TH2F(Form("histoPhEL%d_eta%d_pt%d",i,j,k), Form("histoPhEL%d_eta%d_pt%d",i,j,k) ,800, -0.2, 0.6, 289, -13, 13);
    }
    Int_t fDim = 6;
    Int_t* bins = new Int_t[fDim];
    Double_t *xmin = new Double_t[fDim];
    Double_t *xmax = new Double_t[fDim];
    
    Int_t fbins[6] = {5, 7, 6, 102, 800, 289}; // type, cent, eta, pt, dbeta, nsige
    Double_t fxmin[6] = {0,  0,  0,  0,      -0.2,   -13};
    Double_t fxmax[6] = {5,  7,  6,  102,    0.6,    13};
    
    for (int i=0; i<fDim; i++) {
        bins[i] = fbins[i];
        xmin[i] = fxmin[i];
        xmax[i] = fxmax[i];
    }
    
    hs = new THnSparseF("hs", "hs", fDim, bins, xmin, xmax);
    
    loadTofEvent();
    
    // PhE production
    mPhE = cuts::phe ;
    
    return kStOK;
}

//-----------------------------------------------------------------------------
Int_t StLowPtNpeAnaMaker::Finish()
{
    mOutputFile->cd();
    // write histograms
    hEvent->Write();
    hEventVz->Write();
    hEventVzVpdVz->Write();
    hEventCentrality->Write();
    hEventCentralityCorr->Write();
    hEventRefMult->Write();
    hEventRefMultCorr->Write();
    hEventWeight->Write();
    
    
  /*  for (int i=0 ; i<7 ; i++)
    for (int j=0 ; j<5 ; j++)
    for (int k=0 ; k<102 ; k++)
    {
        histoAll[i][j][k]->Write();
        histoPureEU[i][j][k]->Write();
        histoPureEL[i][j][k]->Write();
        if(mPhE) histoPhEU[i][j][k]->Write();
        if(mPhE) histoPhEL[i][j][k]->Write();
    }

    */
    hs->Write();
    
    
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
        //cout << "CHECK!!! after isGoodEvent()" << endl;
        //cout << "check!!" << endl;
        
        float refmult = mPicoEvent->refMult();
        float vZ = mPicoEvent->primaryVertex().z();
        float zdcCoincidenceRate = mPicoEvent->ZDCx();

        refmultcorr = new StRefMultCorr();
        refmultcorr->init(mPicoEvent->runId());

        refmultcorr->initEvent(refmult, vZ, zdcCoincidenceRate) ;
        iCent = refmultcorr->getCentralityBin6();
        if (iCent<0) {
            return kStOK;
        }
        weight = refmultcorr->getWeight();

        // Fill event histograms
        hEvent->Fill(4);
        hEventWeight->Fill(weight);
        hEventCentrality->Fill(iCent);
        hEventCentralityCorr->Fill(iCent,weight);
        hEventRefMult->Fill(refmult);
        hEventRefMultCorr->Fill(refmult,weight);
        
        UInt_t nTracks = picoDst->numberOfTracks();
        
        std::vector<unsigned short> idxPicoTaggedEs;
        std::vector<unsigned short> idxPicoPartnerEs;
        for (unsigned short iTrack = 0; iTrack < nTracks; ++iTrack)
        {
            StPicoTrack const* trk = picoDst->track(iTrack);
            //if (!trk) continue;
            if (isElectron(trk))
            {
                    fillHistogram(trk,0);
                    if(mPhE) if (isTaggedElectron(trk)) idxPicoTaggedEs.push_back(iTrack);
            }
            
            if(mPhE) if (isPartnerElectron(trk)) idxPicoPartnerEs.push_back(iTrack);
            
        } // .. end tracks loop
        
        
        cout << idxPicoTaggedEs.size() << " " << idxPicoPartnerEs.size() << endl;
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
                
                // fill histograms for PhE
                if (!isGoodElectronPair(electronPair, electron->gMom().perp())) continue;
                cout << " " << ik << " " << ip << endl;
                if (electronPair.charge()==0) fillHistogram(electron,1);
                else fillHistogram(electron,2);

                // fill histograms for PureE
                if (!isGoodPureElectronPair(electronPair, electron->gMom().perp())) continue;
                cout << " " << ik << " " << ip << endl;
                if (electronPair.charge()==0) fillHistogram(electron,3);
                else fillHistogram(electron,4);

            } // .. end make electron pairs
        } // .. end of tagged e loop
        
        idxPicoTaggedEs.clear();
        idxPicoPartnerEs.clear();
        delete refmultcorr;
    } //.. end of good event fill
    
    
    return kStOK;
}

//-----------------------------------------------------------------------------
bool StLowPtNpeAnaMaker::isGoodEvent()
{
    //cout << mPicoEvent->runId() << " " << mPicoEvent->eventId() << endl;
    hEvent->Fill(0);
    hEventVz->Fill(mPicoEvent->primaryVertex().z());
    hEventVzVpdVz->Fill(mPicoEvent->primaryVertex().z(),mPicoEvent->vzVpd());
    
    if (isTofEvent()) {
        hEvent->Fill(1);
        
        if (fabs(mPicoEvent->primaryVertex().z()) < cuts::vz) {
            hEvent->Fill(2);
        
            if (fabs(mPicoEvent->primaryVertex().z() - mPicoEvent->vzVpd()) < cuts::vzVpdVz) {
                hEvent->Fill(3);
                return true;
            
            }
        }
    }
    return false;

}
//-----------------------------------------------------------------------------
bool StLowPtNpeAnaMaker::isGoodTrack(StPicoTrack const * const trk) const
{
    return
    trk->gMom().perp() > cuts::ptMin &&
    trk->gMom().perp() < cuts::ptMax &&
    trk->nHitsFit() >= cuts::nHitsFit &&
    (float)trk->nHitsFit()/(float)trk->nHitsMax() > cuts::nHitsRatioMin &&
    (float)trk->nHitsFit()/(float)trk->nHitsMax() < cuts::nHitsRatioMax ;
}

//-----------------------------------------------------------------------------
bool StLowPtNpeAnaMaker::isElectron(StPicoTrack const * const trk) const
{
    return
    isGoodTrack(trk) &&
    fabs(trk->gMom().pseudoRapidity()) < cuts::etaTagged &&
    trk->nHitsDedx() >= cuts::nHitsDedx &&
    trk->dca() < cuts::globalDca &&
    (trk->gMom().phi() < cuts::phiMin1 ||
    (trk->gMom().phi() > cuts::phiMax1 && trk->gMom().phi() < cuts::phiMin2) ||
    trk->gMom().phi() > cuts::phiMax2) ;
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
    fabs(trk->gMom().pseudoRapidity()) < cuts::etaPartner &&
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
bool StLowPtNpeAnaMaker::isGoodPureElectronPair(StElectronPair const & epair, float pt) const
{
    return
    epair.pairMass() < cuts::pairMassPure &&
    epair.pairDca() < cuts::pairDcaPure;
}
//-----------------------------------------------------------------------------
void StLowPtNpeAnaMaker::fillHistogram(StPicoTrack const * const trk, int type) const
{
    //cout << "CHECK fillHistogram" << endl;
    float ptbin[] = {0, 0.2, 0.21, 0.22, 0.23, 0.24, 0.25, 0.26, 0.27, 0.28, 0.29, 0.3, 0.31, 0.32, 0.33, 0.34, 0.35, 0.36, 0.37, 0.38, 0.39, 0.4, 0.41, 0.42, 0.43, 0.44, 0.45, 0.46, 0.47, 0.48, 0.49, 0.5, 0.51, 0.52, 0.53, 0.54, 0.55, 0.56, 0.57, 0.58, 0.59, 0.6, 0.62, 0.64, 0.66, 0.68, 0.7, 0.72, 0.74, 0.76, 0.78, 0.8, 0.82, 0.84, 0.86, 0.88, 0.9, 0.92, 0.94, 0.96, 0.98, 1, 1.05, 1.1,  1.15,  1.2,  1.25,  1.3,  1.35,  1.4,  1.45,  1.5,  1.55,  1.6,  1.65,  1.7,  1.75,  1.8,  1.85,  1.9,  1.95, 2,  2.1,  2.2,  2.3,  2.4,  2.5,  2.6, 2.7, 2.8, 2.9, 3, 3.1, 3.2, 3.3, 3.4, 3.5, 3.75, 4, 4.25, 4.5, 4.75, 5};
    
    float pt = trk->gMom().perp();
    float eta = trk->gMom().pseudoRapidity();
    
    //cout << pt << " " << eta << endl;

    float dbeta=-999;
    //cout << "CHECK fillHistogram" << endl;
    getCalTofTrack(trk, dbeta);
    //cout << "CHECK fillHistogram" << endl;
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

    
    //cout << iCent << " " << iPt << " " << iEta << endl;
    Double_t * fValue = new Double_t[6];
    fValue[0] = type;
    fValue[1] = iCent+0.5;
    fValue[2] = iEta+0.5;
    fValue[3] = iPt+0.5;
    fValue[4] = dbeta;
    fValue[5] = nSigmaElectron;
    
    hs->Fill(fValue, weight);
    
    
    
    //histoAll[iCent][iEta][iPt]->Fill(dbeta,nSigmaElectron,weight);
    //histoAll[0][iEta][iPt]->Fill(dbeta,nSigmaElectron,weight);
    //cout << "CHECK fillHistogram" << endl;
    //cout << "CHECK fillHistogram end" << endl;
    delete fValue;

}
//--------------------
// TOF calibration
//--------------------
//-----------------------------------------------------------------------------
void StLowPtNpeAnaMaker::loadTofEvent() 
{
    TString temp;
    ifstream list_PicoQa("StRoot/StLowPtNpeAnaMaker/QA_P10ik.txt"); // by beta
    if (!list_PicoQa) {
    }
    for(int i=0;i<2388;i++){
        list_PicoQa >> temp;
        mQa_runID[i] = temp.Atoi();
        list_PicoQa >> temp;
        mQa_west[i]  = temp.Atof();
        list_PicoQa >> temp;
        mQa_east[i]  = temp.Atof();
    }
}
//-----------------------------------------------------------------------------
bool StLowPtNpeAnaMaker::isTofEvent()
{
    mTofcal = -1;
    for(int i=0;i<2388;i++){
        if(mQa_runID[i]==mPicoEvent->runId()) {
            mTofcal = i;
        }
    }
    
    if (mTofcal < 0) return false;
    else return true;
}
//-----------------------------------------------------------------------------
void StLowPtNpeAnaMaker::getCalTofTrack(StPicoTrack const * const trk, float & dbeta) const
{
    float beta_  = (Float_t) trk->btofBeta();
    float beta = beta_;
    short tofCellId = (Short_t) trk->btofCellId(); // tof calibration
    int tofTrayId = tofCellId/192; // tof calibration
    if(tofTrayId == 96 || tofTrayId == 97 || tofTrayId == 98 || tofTrayId == 101) return;
    if(tofTrayId==120) cout << "oops!" << endl;
    if(tofTrayId  <60 && tofTrayId >= 0) 	beta = mQa_west[mTofcal] + beta_;        //tof_cal = tof + qa_west[tofcal]; // tof calibration
    else if(tofTrayId >=60 && tofTrayId < 120) beta = mQa_east[mTofcal] + beta_;   //tof_cal = tof + qa_east[tofcal]; // tof calibration
    else {
        beta = beta_;
        cout << "tofTrayId is wrong : " << tofTrayId << endl;
    }
    
    
    
    float TOF = trk->btof();
    float PathL = beta*TOF*30.;
    float pp = trk->gMom().mag();
    
    dbeta = 1 - PathL/TOF/2.99792458e1*TMath::Sqrt(1-0.000511*0.000511/pp/pp);

}


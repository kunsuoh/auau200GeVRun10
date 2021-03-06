#ifndef StPicoEvent_hh
#define StPicoEvent_hh

class StMuDst;
//class StMuEvent;
class TClonesArray;
class StMuPrimaryVertex;
class StPicoDst;
class StPicoDstMaker;
class StBTofHeader;
#include "StThreeVectorF.hh"
#include "TVector2.h"

class StPicoEvent : public TObject {
public:
  StPicoEvent();
  ~StPicoEvent();
  StPicoEvent(const StMuDst& muDst, const Float_t* Q) ;
//  StPicoEvent(StMuEvent*, StBTofHeader*, float*);
  void Clear(const Option_t*) {}
  
  Int_t    runId() const            { return mRunId; }
  Int_t    eventId() const          { return mEventId; }
  Int_t    fillId() const           { return (Int_t)mFillId; }
  Float_t  bField() const           { return mBField; }
  StThreeVectorF primaryVertex() const { return mPrimaryVertex; }
  Int_t    triggerWord() const      { return mTriggerWord; }
  Int_t    refMultPos() const       { return (Int_t)mRefMultPos; }
  Int_t    refMultNeg() const       { return (Int_t)mRefMultNeg; }
  Int_t    refMultFtpcEast() const  { return (Int_t)mRefMultFtpcEast; }
  Int_t    refMultFtpcWest() const  { return (Int_t)mRefMultFtpcWest; }
  Int_t    refMult() const          { return (Int_t)(mRefMultPos+mRefMultNeg); }
  Int_t    refMultFtpc() const      { return (Int_t)(mRefMultFtpcEast+mRefMultFtpcWest); }
  
  Int_t    nVpdHitsEast() const     { return (Int_t)mNVpdHitsEast; }
  Int_t    nVpdHitsWest() const     { return (Int_t)mNVpdHitsWest; } 
  Int_t    nT0() const              { return (Int_t)mNT0; }
  Float_t  vzVpd() const            { return (Float_t)mVzVpd/100.; }
  
  Float_t  ZDCx() const             { return mZDCx; }
  Float_t  BBCx() const             { return mBBCx; }
  Float_t  Vpd(int i) const         { return (Float_t)mVpd[i]; }
  Float_t  ZdcSumAdcEast() const    { return (Float_t)mZdcSumAdcEast; }
  Float_t  ZdcSumAdcWest() const    { return (Float_t)mZdcSumAdcWest; }
  Float_t  ZdcSmdEastHorizontal(int i) const { return (Float_t)mZdcSmdEastHorizontal[i]; }
  Float_t  ZdcSmdEastVertical(int i) const   { return (Float_t)mZdcSmdEastVertical[i]; }
  Float_t  ZdcSmdWestHorizontal(int i) const { return (Float_t)mZdcSmdWestHorizontal[i]; }
  Float_t  ZdcSmdWestVertical(int i) const   { return (Float_t)mZdcSmdWestVertical[i]; }

  Float_t backgroundRate() const             { return mBackgroundRate; }
  Float_t bbcBlueBackgroundRate() const      { return mBbcBlueBackgroundRate; }
  Float_t bbcYellowBackgroundRate() const    { return mBbcYellowBackgroundRate; }
  Float_t bbcEastRate() const { return mBbcEastRate; }
  Float_t bbcWestRate() const { return mBbcWestRate; }  
  Float_t zdcEastRate() const { return mZdcEastRate; }
  Float_t zdcWestRate() const { return mZdcWestRate; }
  
  Float_t spaceCharge() const { return mSpaceCharge; }

  UShort_t btofTrayMultiplicity() const { return mbTofTrayMultiplicity ; }
  UShort_t numberOfGlobalTracks() const { return mNumberOfGlobalTracks ; }

  Float_t ranking() const { return mRanking ; }
  UShort_t nBEMCMatch() const { return mNBEMCMatch ; }

  UShort_t bbcAdcEast(const Int_t i) { return mBbcAdcEast[i]; }
  UShort_t bbcAdcWest(const Int_t i) { return mBbcAdcWest[i]; }

  TVector2 Q() const         { return TVector2(mQx,mQy); }
  TVector2 Q_ran_1() const   { return TVector2(mQx_ran_1,mQy_ran_1); }
  TVector2 Q_ran_2() const   { return TVector2(mQx_ran_2,mQy_ran_2); }
  TVector2 Q_chg_pos() const { return TVector2(mQx_chg_pos,mQy_chg_pos); }
  TVector2 Q_chg_neg() const { return TVector2(mQx_chg_neg,mQy_chg_neg); }
  TVector2 Q_eta_pos() const { return TVector2(mQx_eta_pos,mQy_eta_pos); }
  TVector2 Q_eta_neg() const { return TVector2(mQx_eta_neg,mQy_eta_neg); }
    
  
protected: //these are written out
  Int_t          mRunId;           // run number
  Int_t          mEventId;         // event number
  UShort_t       mFillId;          // fill number
  Float_t        mBField;          // B field in kilogauss
  StThreeVectorF mPrimaryVertex;   // primary Vertex (1st)
  StThreeVectorF mSecondVertex;    // second Vertex position (for study)
  UShort_t       mTriggerWord;     // self-defined trigger word - see code for details
  UShort_t       mRefMultFtpcEast; // FTPC refMult east
  UShort_t       mRefMultFtpcWest; // FTPC refMult west
  UShort_t       mRefMultNeg;      // TPC refMult neg
  UShort_t       mRefMultPos;      // TPC refMult pos
  
  UChar_t        mNVpdHitsEast;    // Vpd Hits east;
  UChar_t        mNVpdHitsWest;    // vpd hits west;
  UShort_t       mNT0;             // number of T0 particles in BTOF self calibration
  Short_t        mVzVpd;           // VzVpd*100.

  UShort_t        mZDCx;           // zdcX
  UShort_t        mBBCx;
  Float_t mBackgroundRate;
  Float_t mBbcBlueBackgroundRate;
  Float_t mBbcYellowBackgroundRate;
  Float_t mBbcEastRate;
  Float_t mBbcWestRate;
  Float_t mZdcEastRate;
  Float_t mZdcWestRate;
  //Nov.10, 2008, Na
  UShort_t mVpd[64];
  UShort_t mZdcSumAdcEast;
  UShort_t mZdcSumAdcWest;
  UShort_t mZdcSmdEastHorizontal[8];
  UShort_t mZdcSmdEastVertical[8];
  UShort_t mZdcSmdWestHorizontal[8];
  UShort_t mZdcSmdWestVertical[8];
  Float_t mSpaceCharge;

  UShort_t mbTofTrayMultiplicity ; // BTOF tray multiplicity
  UShort_t mNumberOfGlobalTracks ; // # of global tracks

  // From StMuPrimaryVertex
  Float_t mRanking ;
  UShort_t mNBEMCMatch ;

  // BBC ADC for q-vectors (Hiroshi)
  UShort_t mBbcAdcEast[24] ; /// BBC East ADC: 0-23
  UShort_t mBbcAdcWest[24] ; /// BBC West ADC: 24-47
  
  Float_t mQx;
  Float_t mQy;
  
  Float_t mQx_ran_1;
  Float_t mQy_ran_1;
  Float_t mQx_ran_2;
  Float_t mQy_ran_2;
  
  Float_t mQx_chg_pos;
  Float_t mQy_chg_pos;
  Float_t mQx_chg_neg;
  Float_t mQy_chg_neg;
  
  Float_t mQx_eta_pos;
  Float_t mQy_eta_pos;
  Float_t mQx_eta_neg;
  Float_t mQy_eta_neg;


  friend class StPicoDst;
  friend class StPicoDstMaker;
    
  ClassDef(StPicoEvent,1)
};

#endif

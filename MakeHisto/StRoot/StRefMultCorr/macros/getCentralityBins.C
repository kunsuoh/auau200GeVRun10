//----------------------------------------------------------------------------------------------------
// Example macro how to use StRefMultCorr
// $Id: getCentralityBins.C,v 1.3 2011/11/08 19:12:24 hmasui Exp $
// $Log: getCentralityBins.C,v $
// Revision 1.3  2011/11/08 19:12:24  hmasui
// Update usage based on the latest update for 200 GeV (luminosity correction)
//
// Revision 1.2  2011/08/12 20:28:45  hmasui
// Change interface according to the update of StRefMultCorr class
//
//----------------------------------------------------------------------------------------------------

//____________________________________________________________________________________________________
// Example macro for StRefMutlCorr class implementation
//   * Before accessing centrality bins via StRefMutlCorr, you must call 'init(const Int_t RunId)'
//     to specify which parameters you want to use
//     - If you stick to the one specific run, you only need to call this function once in your code.
//     - If you read data from multiple different runs, you need to call this function
//       whenever you switch to the run number
//
//   * In the standard STAR analysis maker, the best (and only) place to call 'init(...)' function 
//     is 'Make(...)'
//
//   * Comment for luminosity (zdc coincidence rate) correction
//     - Luminosity correction is only valid for 200 GeV
//     - The default argument is 0 for zdc coincidence rate in initEvent() function, see header StRefMultCorr.h,
//       so that you can still use previous initEvent() function like
//         void StRefMultCorr::initEvent(refmult, vz) ;
//       without specifying zdc coincidence rate for lower beam energies
void getCentralityBins()
{
  gSystem->Load("StRefMultCorr");
  StRefMultCorr* refmultCorrUtil = new StRefMultCorr();

  // You need to specify the run number you are going to process
  refmultCorrUtil->init(11078000);

  // Dummy refmult and primary z-vertex
  const UShort_t refmult = 100 ;
  const Double_t vz      = 20.0 ;
  const Double_t zdcCoincidenceRate = 20000 ; // Hz

  // ******* IMPORTANT ***********
  // Call initEvent(const UShort_t RefMult, const Double_t z) function
  // event-by-event at the beginning before using any other functions
  refmultCorrUtil->initEvent(refmult, vz, zdcCoincidenceRate) ;

  // Get centrality bins
  //   see StRefMultCorr.h for the definition of centrality bins
  const Int_t cent16 = refmultCorrUtil->getCentralityBin16() ;
  const Int_t cent9  = refmultCorrUtil->getCentralityBin9() ;

  // Re-weighting corrections for peripheral bins
  const Double_t reweight = refmultCorrUtil->getWeight() ;

  //----------------------------------------------------------------------------------------------------
  // Not really necessary for your study but if you want to see the corrected refmult distribution
  // Corrected refmult (z-vertex dependent correction)
  //  NOTE: type should be double or float, not integer
  const Double_t refmultCor = refmultCorrUtil->getRefMultCorr() ;
}


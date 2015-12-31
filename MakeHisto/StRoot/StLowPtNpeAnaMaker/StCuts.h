#ifndef NPE_EVENT_CUTS_H
#define NPE_EVENT_CUTS_H
/* **************************************************
 *  Cuts namespace.
 *
 *  Authors:  **Kunsu OH        (kunsuoh@gmail.com)
 *
 *  **Code Maintainer
 *
 * **************************************************
 */

#include "Rtypes.h"
#include <string>
#include <exception>

namespace cuts
{

    //event
    float const vz = 30.0;
    float const vzVpdVz = 3.0;
    
    // track
    int const nHitsFit = 20;
    float const nHitsRatioMin = 0.52; float const nHitsRatioMax = 1.2;
    float const ptMin = .2;
    float const ptMax = 5.;
    
    // eid
    float const nSigmaTaggedElectron = 3.;
    float const nSigmaPartnerElectron = 3.;
    
    // electrons
    int const nHitsDedx = 15;
    float const globalDca = 1.;
    float const etaTagged = 0.5;
    float const firstHit = 73.;
    float const phiMin1 = -1.9;                              // reject supporting structure
    float const phiMax1 = -1.3; float const phiMin2 = 1.9;
    float const phiMax2 = 1.3;

    float const ylocal = 1.9;
    float const zlocal = 3.2;
    float const beta = 0.025;
    
    // partner
    float const etaPartner = 0.7;
    
    // electron pair cuts
    float const pairMass = 0.5;
    float const pairDca = 3.;
    
    // production
    bool const phe = false;
    
}
#endif

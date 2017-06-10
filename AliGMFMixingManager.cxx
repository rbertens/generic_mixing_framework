 // author: redmer alexander bertens (rbertnens@cern.ch)

#include "AliGMFMixingManager.h"
#include "AliVTrack.h"
#include "AliAODTrack.h"

/* This is a non-trivial class and the heart of the mixing process
 *
 * 
 *
 * 'square mode'
 *      fully orthogonal events - each event is from a unique event, and each track is only used once
 *      number of mixed events equals the number of input events
 *              pro: fast and all events are completely uncorrelated
 *              con: limited statistics
 *
 * 'intermediate mode'
 *
 * 'beast mode'
 *      can theoretically deliver up to m^m events
 *      each track in the mixed event comes from a different input event
 *              pro: unlimited statistics
 *              con: to really get to m^m events, computing time averages the lifetime of a medium :wq:star 
 *
 */

ClassImp(AliGMFMixingManager);

AliGMFMixingManager::AliGMFTrackCuts() : TObject(),
    fMultiplicityMin(1), 
    fMultipicityMax(-1),
    fVertexMin(1),
    fVertexMax(-1)    
    fEventPlaneMin(1),
    fEventPlaneMax(-1) 
{
  // default constructor
}
//_____________________________________________________________________________ 
Bool_t AliGMFMixingManager::IsSelected(AliGMFEventContainer* event) {
    // check if this event meets the criteria for mixing
    if(!event) return kFALSE;
    if(event->GetZvtx() < fVertexMin || event->GetZvtx() > fVertexMax) return kFALSE;
    if(event->GetMultiplicity() > fMultipicityMax || event->GetMultiplicity() < fMultiplicityMin) return kFALSE;
    if(event->GetEventPlane() > fEventPlaneMin || event->GetEventPlane() < fEventPlaneMin()) return kFALSE;

    // if it doesn't fail, it passes :)
    return kTRUE;
}
//_____________________________________________________________________________

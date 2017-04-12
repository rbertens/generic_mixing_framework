// root includes
#include "TClonesArray.h"

// custom includes
#include "../reader/AliGMFEventContainer.h"
#include "../reader/AliGMFTTreeHeader.h"
#include "../reader/AliGMFTTreeTrack.h"

ClassImp(AliGMFEventContainer)

//------------------------------------------------------------------------
AliGMFEventContainer::AliGMFEventContainer() : TObject
{
  // constructor for root I/O
}
//------------------------------------------------------------------------
AliGMFEventContainer::AliGMFEventContainer(const AliGMFEventContainer& event) : TObject
{
  // copy constructor
}
//------------------------------------------------------------------------
AliGMFEventContainer& AliGMFEventContainer::operator=( const AliGMFEventContainer& event )
{
  // assignment operator
  AliGMFEventSimple::operator=(event);
  return *this;
}
//------------------------------------------------------------------------
AliGMFEventContainer::AliGMFEventContainer( 
        const AliGMFTTreeHeader* event,
        const TClonesArray* array) : TObject
  AliGMFEventSimple(array->GetEntries())
{

    // link the event
    fHeader = event;
    fTracks = array;
}
//------------------------------------------------------------------------

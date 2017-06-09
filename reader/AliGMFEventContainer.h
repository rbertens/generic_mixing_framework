// AliGMFEventContainer: Event container
// origin:   Redmer Alexander Bertens, rbertens@cern.ch

#ifndef ALIGMFEVENTCONTAINER_H
#define ALIGMFEVENTCONTAINER_H

#include "TClonesArray.h"
#include "TObject.h"
#include "filter/AliGMFTTreeTrack.h"


//forward declarations
class AliGMFTTreeHeader;
class AliGMFTTreeTrack;

class AliGMFEventContainer : public TObject {

   public:
       AliGMFEventContainer( 
               AliGMFTTreeHeader* event,
               TClonesArray* array, Int_t eventID);
       virtual  ~AliGMFEventContainer() {;}

       // setters
       void SetEvent(
               AliGMFTTreeHeader* header,
               TClonesArray* tracks, Int_t eventID);

       void SetUsed(Bool_t used);

       void PrintEventSummary();

       // interface
       Int_t                    GetNumberOfTracks()     { return fTracks->GetEntries();}
       AliGMFTTreeTrack*        GetTrack(Int_t i)       { return static_cast<AliGMFTTreeTrack*>(fTracks->At(i));}

   private:
       // members
       TClonesArray*            fTracks;        // tclones array with tracks
       AliGMFTTreeHeader*       fHeader;        // event header

       ClassDef(AliGMFEventContainer,1)
};

#endif

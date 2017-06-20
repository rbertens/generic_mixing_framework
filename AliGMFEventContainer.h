// AliGMFEventContainer: Event container
// origin:   Redmer Alexander Bertens, rbertens@cern.ch

#ifndef ALIGMFEVENTCONTAINER_H
#define ALIGMFEVENTCONTAINER_H

#include "TClonesArray.h"
#include "TObject.h"
#include "AliGMFTTreeTrack.h"
#include "AliGMFTTreeHeader.h"

//forward declarations
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
       Bool_t  Fill(AliGMFEventContainer* event);

       void SetUsed(Bool_t used);

       void PrintEventSummary();

       // interface
       Int_t                    GetNumberOfTracks()     { return fTracks->GetEntries();}
       AliGMFTTreeTrack*        GetTrack(Int_t i)       { return static_cast<AliGMFTTreeTrack*>(fTracks->At(i));}

       Int_t                    GetMultiplicity()       { return GetNumberOfTracks(); }
       Float_t                  GetEventPlane()         { return fHeader->GetEventPlane(); }
       Float_t                  GetZvtx()               { return fHeader->GetZvtx(); }
       Float_t                  GetCentrality()         { return fHeader->GetCentrality(); }
       
       AliGMFTTreeHeader*       GetHeader()             { return fHeader;}
       TClonesArray*            GetTracks()             { return fTracks;}
       
       void                     Dump() const;

   private:
       // members
       TClonesArray*            fTracks;        // tclones array with tracks
       AliGMFTTreeHeader*       fHeader;        // event header

       ClassDef(AliGMFEventContainer,1)
};

#endif

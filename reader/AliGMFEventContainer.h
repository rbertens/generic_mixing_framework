// AliGMFEventContainer: Event container
// origin:   Redmer Alexander Bertens, rbertens@cern.ch

#ifndef ALIGMFEVENTCONTAINER_H
#define ALIGMFEVENTCONTAINER_H

//forward declarations
class AliGMFTTreeHeader;
class TClonesArray;

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

   private:
       // members
       TClonesArray*            fTracks;        // tclones array with tracks
       AliGMFTTreeHeader*       fHeader;        // event header

       ClassDef(AliGMFEventContainer,1)
};

#endif

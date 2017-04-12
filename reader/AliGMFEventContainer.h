// AliGMFEventContainer: Event container
// origin:   Redmer Alexander Bertens, rbertens@cern.ch

#ifndef ALIFLOWEVENTSIMPLEFROMTTREE_H
#define ALIFLOWEVENTSIMPLEFROMTTREE_H

// aliroot includes
#include "AliGMFEventSimple.h"

//forward declarations
class AliGMFTTreeHeader;
class TClonesArray;

class AliGMFEventContainer : public TObject {

   public:
       AliGMFEventContainer();
       AliGMFEventContainer(const AliGMFEventContainer& event );
       AliGMFEventContainer( 
               const AliGMFTTreeHeader* event,
               const TClonesArray* array);
       AliGMFEventContainer& operator=( const AliGMFEventContainer& event );
       virtual  ~AliGMFEventContainer() {}


       // members
       TClonesArray* fTracks;           //! tclones array with tracks
       AliGMFTTreeHeader fEvent;        //! event header




       ClassDef(AliGMFEventContainer,1)
};

#endif

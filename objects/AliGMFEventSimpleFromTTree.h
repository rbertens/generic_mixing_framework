// AliGMFEventSimpleFromTTree: Event container for flow analysis of star data
// origin:   Redmer Alexander Bertens, rbertens@cern.ch

#ifndef ALIFLOWEVENTSIMPLEFROMTTREE_H
#define ALIFLOWEVENTSIMPLEFROMTTREE_H_H

// aliroot includes
#include "AliGMFEventSimple.h"

//forward declarations
class AliGMFTTreeEvent;
class TClonesArray;
class AliGMFTrackSimpleCuts;

class AliGMFEventSimpleFromTTree : public AliGMFEventSimple {

   public:
       AliGMFEventSimpleFromTTree();
       AliGMFEventSimpleFromTTree(const AliGMFEventSimpleFromTTree& event );
       AliGMFEventSimpleFromTTree( 
               const AliGMFTTreeEvent* event,
               const TClonesArray* array,
               const AliGMFTrackSimpleCuts* rpCuts=NULL,
               const AliGMFTrackSimpleCuts* poiCuts=NULL);

       Bool_t PassesCuts(const AliGMFTTreeEvent* event);

       AliGMFEventSimpleFromTTree& operator=( const AliGMFEventSimpleFromTTree& event );
       virtual  ~AliGMFEventSimpleFromTTree() {}

       ClassDef(AliGMFEventSimpleFromTTree,1)
};

#endif

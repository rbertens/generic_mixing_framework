#ifndef AliGMFEventCuts_H
#define AliGMFEventCuts_H

#include <TObject.h>

class AliVEvent;

class AliGMFEventCuts : public TObject {

 public:
    AliGMFEventCuts();

    Bool_t IsSelected(AliVEvent* event);

    Bool_t PassesCentralitySelection();
    Bool_t PassesVertexSelection();

     
 private:

    AliVEvent*  fCurrentEvent;  //! current event    

    ClassDef(AliGMFEventCuts, 1);    // Help class

};

#endif

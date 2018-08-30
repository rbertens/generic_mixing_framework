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
    Bool_t Passes2010PileUpRejection();

     
    void   Set2010PileUpRejection(Bool_t s)    {fCheck2010PileUpRejection = s;}
 private:

    AliVEvent*  fCurrentEvent;  //! current event    
    Bool_t      fCheck2010PileUpRejection; // check for pile up rejection

    ClassDef(AliGMFEventCuts, 1); 

};

#endif

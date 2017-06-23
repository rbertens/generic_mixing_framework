#ifndef AliGMFTTreeEventCuts_H
#define AliGMFTTreeEventCuts_H

#include <TObject.h>

class AliGMFEventContainer;


class AliGMFTTreeEventCuts : public TObject {

 public:
    AliGMFTTreeEventCuts();

    Bool_t IsSelected(AliGMFEventContainer* event);

    Bool_t PassesMultiplicitySelection();
    Bool_t PassesVertexSelection();
    Bool_t PassesCentralitySelection();
    Bool_t PassesEventPlaneSelection();

    // setters
    void SetMultiplicityRange(Int_t min, Int_t max) {
        fMultiplicityMin = min;
        fMultiplicityMax = max;
    }
    void SetVertexRange(Float_t min, Float_t max) {
        fVertexMin = min;
        fVertexMax = max;
    }
    void SetEventPlaneRange(Float_t min, Float_t max) {
        fEventPlaneMin = min;
        fEventPlaneMax = max;
    }

    void SetCentralityRange(Float_t min, Float_t max) {
        fCentralityMin = min;
        fCentralityMax = max;
    }
     
 private:

    AliGMFEventContainer*  fCurrentEvent;  //! current event    
    Int_t       fMultiplicityMin;   // minimum multiplicity
    Int_t       fMultiplicityMax;   // maximum multiplicity
    Float_t     fVertexMin;         // minimum vertexz
    Float_t     fVertexMax;         // maximum vertexz
    Float_t     fEventPlaneMin;     // minimum event plane angle
    Float_t     fEventPlaneMax;     // maximum event plane angle
    Float_t     fCentralityMin;     // minimum event centrality
    Float_t     fCentralityMax;     // maximum event centrality

    ClassDef(AliGMFTTreeEventCuts, 1);    // Help class

};

#endif

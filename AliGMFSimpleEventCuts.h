#ifndef AliGMFSimpleEventCuts_H
#define AliGMFSimpleEventCuts_H

#include <TObject.h>

class AliGMFEventContainer;


class AliGMFSimpleEventCuts : public TObject {

 public:
    AliGMFSimpleEventCuts();

    Bool_t IsSelected(AliGMFEventContainer* event);

    Bool_t PassesMultiplicitySelection();
    Bool_t PassesVertexSelection();
    Bool_t PassesCentralitySelection();
    Bool_t PassesEventPlaneSelection();
    Bool_t PassesRunNumberSelection();

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
    
    void SetRunNumberRange(Short_t min, Short_t max) {
        fRunNumberMin = min;
        fRunNumberMax = max;
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
    Short_t     fRunNumberMin;      // minimum run number
    Short_t     fRunNumberMax;      // maximum run number

    ClassDef(AliGMFSimpleEventCuts, 1);    // Help class

};

#endif

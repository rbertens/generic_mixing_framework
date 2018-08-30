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

    void Diagnose();

    // setters
    void SetMultiplicityRange(Int_t min, Int_t max) {
        fCheckMultiplicity = kTRUE;
        fMultiplicityMin = min;
        fMultiplicityMax = max;
    }
    void SetVertexRange(Float_t min, Float_t max) {
        fCheckVertex = kTRUE;
        fVertexMin = min;
        fVertexMax = max;
    }
    void SetEventPlaneRange(Float_t min, Float_t max) {
        fCheckEventPlane = kTRUE;
        fEventPlaneMin = min;
        fEventPlaneMax = max;
    }

    void SetCentralityRange(Float_t min, Float_t max) {
        fCheckCentrality = kTRUE;
        fCentralityMin = min;
        fCentralityMax = max;
    }
    
    void SetRunNumberRange(ULong_t min, ULong_t max) {
        fCheckRunNumber = kTRUE;
        fRunNumberMin = min;
        fRunNumberMax = max;
    }
     
 private:

    AliGMFEventContainer*  fCurrentEvent;  //! current event    
    Bool_t      fCheckMultiplicity; // cut on multiplicity
    Int_t       fMultiplicityMin;   // minimum multiplicity
    Int_t       fMultiplicityMax;   // maximum multiplicity
    Bool_t      fCheckVertex;       // cut on vertexz position
    Float_t     fVertexMin;         // minimum vertexz
    Float_t     fVertexMax;         // maximum vertexz
    Bool_t      fCheckEventPlane;   // cut on event plane angle orientation
    Float_t     fEventPlaneMin;     // minimum event plane angle
    Float_t     fEventPlaneMax;     // maximum event plane angle
    Bool_t      fCheckCentrality;   // cut on centrality percentile
    Float_t     fCentralityMin;     // minimum event centrality
    Float_t     fCentralityMax;     // maximum event centrality
    Bool_t      fCheckRunNumber;    // cut on run number
    ULong_t     fRunNumberMin;      // minimum run number
    ULong_t     fRunNumberMax;      // maximum run number

    ClassDef(AliGMFSimpleEventCuts, 1);    // Help class

};

#endif

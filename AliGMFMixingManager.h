#ifndef AliGMFMixingManager_H
#define AliGMFMixingManager_H

#include <TObject.h>

#include "AliGMFEventContainer.h"

class AliGMFMixingManager : public TObject {

 public:
    AliGMFMixingManager();
    Bool_t IsSelected(AliGMFEventContainer* event);


    // setters
    SetMultiplicityRange(Int_t min, Int_t max) {
        fMultiplicityMin = min;
        fMultiplicityMax = max;
    }
    SetVertexrange(Float_t min, Float_t max) {
        fVertexMin = min;
        fVertexMax = max;
                }
    SetEventPlaneRange(Float_t min, Float_t max) {
        fEventPlaneMin = min;
        fEventPlaneMax = max;
    }

 private:
    fMultiplicityMin;   // minimum multiplicity
    fMultipicityMax;    // maximum multiplicity
    fVertexMin;         // minimum vertexz
    fVertexMax;         // maximum vertexz
    fEventPlaneMin;     // minimum event plane angle
    fEventPlaneMax;     // maximum event plane angle



    ClassDef(AliGMFMixingManager, 1);

};

#endif

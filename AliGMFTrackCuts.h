#ifndef AliGMFTrackCuts_H
#define AliGMFTrackCuts_H

#include <TObject.h>
class AliVTrack;
class AliAODTrack;

class AliGMFTrackCuts : public TObject {

 public:
    AliGMFTrackCuts();
    Bool_t IsSelected(AliVTrack* track);

    // setters
    void        SetFilterBit(UInt_t bit)        {fFilterBit = bit;}

    // getters
    UInt_t      GetFilterBit() const          {return fFilterBit;}
     
 private:
    UInt_t              fFilterBit;     // aod filter bit
    AliAODTrack*        fAODTrack;      //! current aod track



    ClassDef(AliGMFTrackCuts, 1);

};

#endif

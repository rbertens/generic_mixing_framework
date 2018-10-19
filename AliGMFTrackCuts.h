#ifndef AliGMFTrackCuts_H
#define AliGMFTrackCuts_H

#include <TObject.h>
class AliVTrack;
class AliAODTrack;

class AliGMFTrackCuts : public TObject {

 public:
    AliGMFTrackCuts();
    void   Diagnose(AliVTrack* track);
    Bool_t IsSelected(AliVTrack* track);

    // setters
    void        SetFilterBit(UInt_t bit)        {fFilterBit = bit;}
    void        SetMinPt(Float_t pt)            {fMinPt = pt;}
    void        SetMaxPt(Float_t pt)            {fMaxPt = pt;}
    void        SetMinEta(Float_t eta)          {fMinEta = eta;}
    void        SetMaxEta(Float_t eta)          {fMaxEta = eta;}

    // getters
    UInt_t      GetFilterBit() const          {return fFilterBit;}
     
 private:
    UInt_t              fFilterBit;     // aod filter bit
    Float_t             fMinPt;         // minimum track pt
    Float_t             fMaxPt;         // maximum track pt
    Float_t             fMinEta;        // minimum track eta
    Float_t             fMaxEta;        // maximum track eta

    AliAODTrack*        fAODTrack;      //! current aod track



    ClassDef(AliGMFTrackCuts, 1);

};

#endif

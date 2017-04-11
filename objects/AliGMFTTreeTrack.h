#ifndef AliGMFTTreeTRACK_H
#define AliGMFTTreeTRACK_H

#include <TObject.h>

class AliGMFTTreeTrack : public TObject {

 public:

    AliGMFTTreeTrack();
 
    void       SetPt(Float_t pt)                {fPt =          pt;}
    void       SetEta(Float_t eta)              {fEta =         eta;}
    void       SetPhi(Float_t phi)              {fPhi =         phi;}
    void       SetCharge(Float_t charge)        {fCharge =      charge;}
   
    Float_t    GetPt() const                    {return fPt;}
    Float_t    GetEta() const                   {return fEta;}
    Float_t    GetPhi() const                   {return fPhi;}
    Float_t    GetCharge() const                {return fCharge;}
    
 private:
    Float_t       fPt;
    Float_t       fEta;
    Float_t       fPhi;
    Float_t       fCharge;

    virtual ~AliGMFTTreeTrack(); // default destructor
  
    ClassDef(AliGMFTTreeTrack, 1);    // Help class

};

#endif

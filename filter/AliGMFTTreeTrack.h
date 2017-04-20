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
    // obviously a float for charge doesn't make sense
    Float_t       fCharge;

    // some transient members that we'll use for bookkeeping, but dont want to store now
    Bool_t        fUsed;        //! was track used for mixing ? 

    virtual ~AliGMFTTreeTrack(); // default destructor
  
    ClassDef(AliGMFTTreeTrack, 1);    // Help class

};

#endif

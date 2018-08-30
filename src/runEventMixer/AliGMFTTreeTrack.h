#ifndef COMPRESSION_LEVEL
#define COMPRESSION_LEVEL 1
#endif

#ifndef AliGMFTTreeTRACK_H
#define AliGMFTTreeTRACK_H

#include <TObject.h>

class AliGMFTTreeTrack : public TObject {

    public:

        AliGMFTTreeTrack();
        void       Fill(AliGMFTTreeTrack* track);
        void       Fill(Double_t pt, Double_t eta, Double_t phi, 
                Short_t charge, Bool_t used, Int_t number) {
            fPt = pt;
            fEta = eta;
            fPhi = phi;
            fCharge = charge;
            fUsed = used;
            fNumber = number; 
        }

        void       SetPt(Float_t pt)                {fPt =          pt;}
        void       SetEta(Float_t eta)              {fEta =         eta;}
        void       SetPhi(Float_t phi)              {fPhi =         phi;}
        void       SetCharge(Short_t charge)        {fCharge =      charge;}
        void       SetUsed(Bool_t used)             {fUsed =        used;}
        void       SetNumber(Int_t number)          {fNumber =      number;}

        Float_t    GetPt() const                    {return fPt;}
        Float_t    GetEta() const                   {return fEta;}
        Float_t    GetPhi() const                   {return fPhi;}
        Float_t    GetCharge() const                {return fCharge;}
        Bool_t     GetUsed() const                  {return fUsed;}
        Bool_t     GetFilled() const                {return fFilled;}
        Int_t      GetNumber() const                {return fNumber;}

        void       Reset();
        void       PrintInfo();
    private:
#if COMPRESSION_LEVEL > 1
        // maximum compression level
        Double32_t       fPt;       //[0,100,8]
        Double32_t       fEta;      //[-1,1,8]
        Double32_t       fPhi;      //[0,6.3,8]
#elif COMPRESSION_LEVEL > 0
        // medium compression level
        Double32_t       fPt;       //[0,100,12]
        Double32_t       fEta;      //[-1,1,12]
        Double32_t       fPhi;      //[0,6.3,12]
#else
        // no compression
        Double_t       fPt;         //[0,100,8]
        Double_t       fEta;        //[-1,1,8]
        Double_t       fPhi;        //[0,6.3,8]
#endif
        Short_t          fCharge;   //charge of track

        // some transient members that we'll use for bookkeeping, but dont want to store now
        Bool_t        fUsed;        //! was track used for mixing ? 
        Bool_t        fFilled;      //! was track filled ?
        Int_t         fNumber;      //! just an int - use as you see fit 

        virtual ~AliGMFTTreeTrack(); // default destructor

        ClassDef(AliGMFTTreeTrack, 1);    // Help class

};

#endif

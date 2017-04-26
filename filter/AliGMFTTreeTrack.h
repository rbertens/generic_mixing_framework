#ifndef COMPRESSION_LEVEL
#define COMPRESSION_LEVEL 2
#endif

#ifndef AliGMFTTreeTRACK_H
#define AliGMFTTreeTRACK_H

#include <TObject.h>

class AliGMFTTreeTrack : public TObject {

    public:

        AliGMFTTreeTrack();

        void       SetPt(Float_t pt)                {fPt =          pt;}
        void       SetEta(Float_t eta)              {fEta =         eta;}
        void       SetPhi(Float_t phi)              {fPhi =         phi;}
        void       SetCharge(Short_t charge)        {fCharge =      charge;}
        void       SetUsed(Bool_t used)             {fUsed =        used;}

        Float_t    GetPt() const                    {return fPt;}
        Float_t    GetEta() const                   {return fEta;}
        Float_t    GetPhi() const                   {return fPhi;}
        Float_t    GetCharge() const                {return fCharge;}

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

        virtual ~AliGMFTTreeTrack(); // default destructor

        ClassDef(AliGMFTTreeTrack, 1);    // Help class

};

#endif

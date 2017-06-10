#ifndef COMPRESSION_LEVEL
#define COMPRESSION_LEVEL 1 
#endif

#ifndef AliGMFTTreeEVENT_H
#define AliGMFTTreeEVENT_H

#include <TObject.h>

class AliGMFTTreeHeader : public TObject{

 public:

  AliGMFTTreeHeader();
  
  // manipulators - persistent
  void  SetZvtx(Float_t Zvtx)           {fZvtx  = Zvtx;}
  void  SetEventPlane(Float_t ep)       {fEventPlane = ep;}

  // manipulators - no persistent
  void SetEventID(Int_t id)             {fEventID = id;}
  void SetUsed(Bool_t used)             {fUsed = used;}
  void SetMultiplicity(Short_t m)       {fMultiplicity = m;}


  // getters
  Float_t       GetZvtx() const         {return fZvtx;}
  Float_t       GetEventPlane() const   {return fEventPlane;}

  Int_t         GetEventID() const      {return fEventID;}
  Bool_t        GetUsed() const         {return fUsed;}
  Short_t       SetMultiplicity() const {return fMultiplicity;}

 private:
  // first the persistent members are listed. these are written to disk
  // so extra care is taken to minimize the space they take
#if COMPRESSION_LEVEL > 1
  // maximum compression, some loss of precision may occur
  Double32_t    fZvtx;          //[0,10,8] rec vertex
  Double32_t    fEventPlane;    //[0,3.15,8] event plane orientation
#elif COMPRESSION > 0
  //medium compression, no precision loss expected`
  Double32_t    fZvtx;          //[0,10,12] rec vertex
  Double32_t    fEventPlane;    //[0,3.15,12] event plane orientation
#else  
  // no compression
  Double_t      fZvtx;          // rec vertex
  Double_t      fEventPlane;    //event plane orientation
#endif

  // transient members are not written to disk, they
  // can be optimized for speed
  Int_t         fEventID;       //! event identifier
  Bool_t        fUsed;          //! was event read from file
  Short_t       fMultiplicity;  //! event multiplicity

  virtual ~AliGMFTTreeHeader(); // default destructor

  ClassDef(AliGMFTTreeHeader, 1);    // Help class
};

#endif
	

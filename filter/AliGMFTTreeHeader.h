#ifndef COMPRESSION_LEVEL
#define COMPRESSION_LEVEL 0
#endif

#ifndef AliGMFTTreeEVENT_H
#define AliGMFTTreeEVENT_H

#include <TObject.h>

class AliGMFTTreeHeader : public TObject{

 public:

  AliGMFTTreeHeader();
  
  // manipulators - persistent
  void  SetRun(Int_t run)       {fRun   = run;}
  void  SetV0M(Float_t V0M)     {fV0M   = V0M;}
  void  SetZvtx(Float_t Zvtx)   {fZvtx  = Zvtx;}

  Int_t         GetRun() const  {return fRun;}
  Float_t       GetV0M() const  {return fV0M;}
  Float_t       GetZvtx() const {return fZvtx;}

  // manipulators - no persistent
  void SetEventID(Int_t id)     {fEventID = id;}
  void SetUsed(Bool_t used)     {fUsed = used;}

  Int_t         GetEventID() const      {return fEventID;}
  Bool_t        GetUsed() const         {return fUsed;}


 private:
  Short_t       fRun;        // run number
#if COMPRESSION_LEVEL > 1
  // maximum compression, some loss of precision may occur
  Double32_t    fV0M;        //[0,100,8] centrality V0
  Double32_t    fZvtx;       //[0,10,8] rec vertex
#elif COMPRESSION > 0
  //medium compression, no precision loss expected`
  Double32_t    fV0M;        //[0,100,12] centrality V0
  Double32_t    fZvtx;       //[0,10,12] rec vertex
#else  
  // no compression
  Double_t      fV0M;        // centrality V0
  Double_t      fZvtx;       // rec vertex
#endif

  // transient members for bookkeeping
  Int_t    fEventID;    //! event identifier
  Bool_t   fUsed;       //! was event read from file

  virtual ~AliGMFTTreeHeader(); // default destructor

  ClassDef(AliGMFTTreeHeader, 1);    // Help class
};

#endif
	

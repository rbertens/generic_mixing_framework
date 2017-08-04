#ifndef ALIANALYSISTTREEFILTER_H
#define ALIANALYSISTTREEFILTER_H

// AliRoot includes
#include "AliAnalysisTaskSE.h"

// forward declarations
class AliVEvent;
class AliVTrack;
class TTree;
class TClonesArray;

// framework classes
class AliGMFTTreeHeader;
class AliGMFEventCuts;
class AliGMFTrackCuts;
class AliGMFHistogramManager;

class AliAnalysisTaskTTreeFilter : public AliAnalysisTaskSE {

 public:
  AliAnalysisTaskTTreeFilter();
  AliAnalysisTaskTTreeFilter(const char *name);
  enum collisionPeriod    { kPbPb10h, kPbPb11h }; // collision type, kPbPb = 11h, kept for backward compatibilitiy
  enum eventPlaneDetector { kTPC, kVZEROA, kVZEROC, kVZEROComb};  // detector that was used for event plane

  virtual ~AliAnalysisTaskTTreeFilter();

  virtual void  UserCreateOutputObjects();
  virtual void  UserExec(Option_t *option);
  virtual void  Terminate(Option_t *); 
  Bool_t        Notify();

  // setters
  void          SetEventCuts(AliGMFEventCuts* ec)       {fEventCuts = ec;}
  void          SetTrackCuts(AliGMFTrackCuts* tc)       {fTrackCuts = tc;}
  void          SetEventPlaneN(Int_t n)                 {fEventPlaneN = n;}
  void          SetReferenceDetector(eventPlaneDetector type) {fDetectorType = type; }
  void          SetCollisionPeriod(collisionPeriod period)    {fCollisionPeriod = period; }
  void          SetDoQA(Bool_t qa)                      {fDoQA = qa;}


  // getters
  const AliGMFEventCuts*        GetEventCuts()  {return fEventCuts;}
  const AliGMFTrackCuts*        GetTrackCuts()  {return fTrackCuts;}

 private:
  Bool_t        ParseEvent(AliVEvent* event);
  void          ParseTracks(AliVEvent* event);
  void          PushToTTree();
  Bool_t        PassesCuts(AliVEvent* event);
  Bool_t        PassesCuts(AliVTrack* track);

  void          FillEventQA(Bool_t cutsApplied, AliVEvent* event);
  void          FillTrackQA(Bool_t cutsApplied, AliVTrack* track);

  Float_t       GetEventPlane(Bool_t useCache = kFALSE);
  void          CalculateEventPlaneVZERO(Double_t vzero[2][2]) const;
  void          CalculateEventPlaneCombinedVZERO(Double_t* comb) const;
  void          CalculateEventPlaneTPC(Double_t* tpc);
  void          CalculateQvectorVZERO(Double_t Qa2[2], Double_t Qc2[2], Double_t Qa3[2], Double_t Qc3[2]) const;
  void          CalculateQvectorCombinedVZERO(Double_t Q2[2], Double_t Q3[2]) const;
  void          ReadVZEROCalibration2010h();
  Int_t         GetVZEROCentralityBin() const;

  // configuration objects
  AliGMFEventCuts*      fEventCuts;     // event cuts
  AliGMFTrackCuts*      fTrackCuts;     // track cuts
  Int_t                 fEventPlaneN;   // event plane harmonic
  eventPlaneDetector    fDetectorType;  // detector used for event plane estimation
  collisionPeriod       fCollisionPeriod;       // period
  Bool_t                fDoQA;          // do qa
  Float_t               fCachedEP;      // cached EP

  // Output objects
  TTree*                fTree;          //! output data
  AliGMFTTreeHeader*    fEvent;         //! custom event
  TClonesArray*         fTrackArray;    //! custom tracks
  AliGMFHistogramManager*       fHistogramManager;      //! histogram manager



  // vzero event plane calibration cache
  Float_t               fMeanQ[9][2][2];                //! recentering
  Float_t               fWidthQ[9][2][2];               //! recentering
  Float_t               fMeanQv3[9][2][2];              //! recentering
  Float_t               fWidthQv3[9][2][2];             //! recentering
  TH1*                  fVZEROgainEqualization;         //! equalization histo
  Float_t               fVZEROApol;                     //! calibration info per disc
  Float_t               fVZEROCpol;                     //! calibration info per disc
  TFile*                fOADB;                          //! fOADB
  Int_t                 fRunNumber;     // runnumber
  
  ClassDef(AliAnalysisTaskTTreeFilter, 1);
};

#endif

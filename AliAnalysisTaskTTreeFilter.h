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


class AliAnalysisTaskTTreeFilter : public AliAnalysisTaskSE {

 public:
  AliAnalysisTaskTTreeFilter();
  AliAnalysisTaskTTreeFilter(const char *name);

  virtual ~AliAnalysisTaskTTreeFilter();

  virtual void  UserCreateOutputObjects();
  virtual void  UserExec(Option_t *option);
  virtual void  Terminate(Option_t *); 

  // setters
  void          SetEventCuts(AliGMFEventCuts* ec)       {fEventCuts = ec;}
  void          SetTrackCuts(AliGMFTrackCuts* tc)       {fTrackCuts = tc;}

  // getters
  const AliGMFEventCuts*        GetEventCuts()  {return fEventCuts;}
  const AliGMFTrackCuts*        GetTrackCuts()  {return fTrackCuts;}

 private:
  Bool_t        ParseEvent(AliVEvent* event);
  void          ParseTracks(AliVEvent* event);
  void          PushToTTree();
  Bool_t        PassesCuts(AliVEvent* event);
  Bool_t        PassesCuts(AliVTrack* track);


  // configuration objects
  AliGMFEventCuts*      fEventCuts;     // event cuts
  AliGMFTrackCuts*      fTrackCuts;     // track cuts
  // Output objects
  TTree*                fTree;          //! output data
  AliGMFTTreeHeader*    fEvent;         //! custom event
  TClonesArray*         fTrackArray;    //! custom tracks

  ClassDef(AliAnalysisTaskTTreeFilter, 1);
};

#endif

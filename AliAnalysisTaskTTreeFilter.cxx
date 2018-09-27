 // author: redmer alexander bertens (rbertens@cern.ch)

#include "AliAnalysisTaskTTreeFilter.h"

// ROOT includes
#include <TTree.h>
#include <TMath.h>
#include <TObject.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TFile.h>
#include <TF1.h>
#include <TProfile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGrid.h>

// AliRoot includes
#include <AliAnalysisManager.h>
#include <AliInputEventHandler.h>
#include <AliOADBContainer.h>
#include <AliLog.h>
#include <AliVEvent.h>
#include <AliVVertex.h>
#include <AliCentrality.h>
#include <AliMultSelection.h>
#include <AliVTrack.h>
#include <AliEventplane.h>
#include <AliVVZERO.h>

// local includes
#include "AliGMFTTreeHeader.h"
#include "AliGMFTTreeTrack.h"
#include "AliGMFEventCuts.h"
#include "AliGMFTrackCuts.h"
#include "AliGMFHistogramManager.h"

ClassImp(AliAnalysisTaskTTreeFilter)

//_____________________________________________________________________________
AliAnalysisTaskTTreeFilter::AliAnalysisTaskTTreeFilter():
    AliAnalysisTaskSE(),
    fEvent(0x0),
    fTrackArray(0x0),
    fHistogramManager(0x0),
    fEventCuts(0x0),
    fTrackCuts(0x0),
    fDetectorType(kTPC),
    fCollisionPeriod(kPbPb10h),
    fDoQA(kFALSE),
    fCachedEP(0),
    fCachedEP3(0),
    fOADB(0x0),
    fRunNumber(-1)
{
    // default constructor for root I/O
    for(Int_t i(0); i < 9; i++) {
        for(Int_t j(0); j < 2; j++) {
            for(Int_t k(0); k < 2; k++) {
                fMeanQ[i][j][k] = 0.; 
                fWidthQ[i][j][k] = 0.;  
                fMeanQv3[i][j][k] = 0.; 
                fWidthQv3[i][j][k] = 0.;
                fMQ[j][k][0] = 0;
                fWQ[j][k][0] = 0;
                fMQ[j][k][1] = 0;
                fWQ[j][k][1] = 0;
            }
        }
    }
}
//______________________________________________________________________________
AliAnalysisTaskTTreeFilter::AliAnalysisTaskTTreeFilter(const char *name):
    AliAnalysisTaskSE(name),
    fEvent(0x0),
    fTrackArray(0x0),
    fHistogramManager(0x0),
    fEventCuts(0x0),
    fTrackCuts(0x0),
    fDetectorType(kTPC),
    fCollisionPeriod(kPbPb10h),
    fDoQA(kFALSE),
    fCachedEP(0),
    fCachedEP3(0),
    fOADB(0x0),
    fRunNumber(-1)
{
    // constructor
    for(Int_t i(0); i < 9; i++) {
        for(Int_t j(0); j < 2; j++) {
            for(Int_t k(0); k < 2; k++) {
                fMeanQ[i][j][k] = 0.; 
                fWidthQ[i][j][k] = 0.;  
                fMeanQv3[i][j][k] = 0.; 
                fWidthQv3[i][j][k] = 0.;
                fMQ[j][k][0] = 0;
                fWQ[j][k][0] = 0;
                fMQ[j][k][1] = 0;
                fWQ[j][k][1] = 0;
            }
        }
    }
    DefineOutput(1, TTree::Class());
}
//_____________________________________________________________________________
AliAnalysisTaskTTreeFilter::~AliAnalysisTaskTTreeFilter()
{
    // destructor
    delete fTree;
    delete fEventCuts;
    delete fTrackCuts;
}

//_____________________________________________________________________________
Bool_t AliAnalysisTaskTTreeFilter::Notify()
{
    if(fRunNumber != InputEvent()->GetRunNumber()) {
        fRunNumber = InputEvent()->GetRunNumber();        // set the current run number
        if(fDetectorType != kTPC) {
            switch (fCollisionPeriod) {
                case kPbPb10h : {
                    ReadVZEROCalibration2010h(); 
                    return kTRUE;
                } break;
                case kPbPb15o : {
                     ReadVZEROCalibration2015o();
                     return kTRUE;
                } break;
                default : break;
            }
        }
    }
    return kTRUE;
}
//______________________________________________________________________________
void AliAnalysisTaskTTreeFilter::UserCreateOutputObjects()
{ 
    // check for manager
    AliAnalysisManager *man=AliAnalysisManager::GetAnalysisManager();
    AliInputEventHandler *inputHandler=static_cast<AliInputEventHandler*>(man->GetInputEventHandler());
    if (!inputHandler) AliFatal(" > no input detected - aborting <");

    // if qa is requested, initialize the manager
    if(fDoQA) {
        fHistogramManager = new AliGMFHistogramManager();
        fHistogramManager->BookTH1D("fHistBeforePt", "#it{p}_{T} (GeV/c)", 100, 0, 20);
        fHistogramManager->BookTH1D("fHistBeforeEta", "#eta", 100, -1, 1);
        fHistogramManager->BookTH1D("fHistBeforePhi", "#phi", 100, 0, TMath::TwoPi());
        fHistogramManager->BookTH2D("fHistBeforeEtaPhi", "#eta", "#phi", 100, -1, 1, 100, 0, TMath::TwoPi());
        fHistogramManager->BookTH1D("fHistBeforeVertex", "cm", 100, -12, 12);
        fHistogramManager->BookTH1D("fHistBeforeCentrality", "percentile", 100, 0, 100);
        fHistogramManager->BookTH1D("fHistBeforeEventPlane", "#Psi", 100, -4, 4);
        fHistogramManager->BookTH1D("fHistAfterPt", "#it{p}_{T} (GeV/c)", 100, 0, 20);
        fHistogramManager->BookTH1D("fHistAfterEta", "#eta", 100, -1, 1);
        fHistogramManager->BookTH1D("fHistAfterPhi", "#phi", 100, 0, TMath::TwoPi());
        fHistogramManager->BookTH2D("fHistAfterEtaPhi", "#eta", "#phi", 100, -1, 1, 100, 0, TMath::TwoPi());
        fHistogramManager->BookTH1D("fHistAfterVertex", "cm", 100, -12, 12);
        fHistogramManager->BookTH1D("fHistAfterCentrality", "percentile", 100, 0, 100);
        fHistogramManager->BookTH1D("fHistAfterEventPlane", "#Psi", 100, -4, 4);
    }


    // open file at slot 1 for large output to avoid buffer overflow
    AliAnalysisTask::OpenFile(1);

    // create the ttree
    fTree = new TTree("tree", "Event data");

    // init the custom event 
    fEvent = new AliGMFTTreeHeader();
    // add the event branch to the tree
    fTree->Branch("event", &fEvent);
  
    // init the track tclonesarray
    fTrackArray = new TClonesArray("AliGMFTTreeTrack", 1000);
    // add clones array as branch via bronch
    fTree->Bronch("track", "TClonesArray", &fTrackArray);

    // Post output data.
    PostData(1, fTree);
}
//______________________________________________________________________________
void AliAnalysisTaskTTreeFilter::UserExec(Option_t *) 
{
    // parse accepted input event
    if(ParseEvent(static_cast<AliVEvent*>(InputEvent()))) PostData(1, fTree);
    else return;
}

//________________________________________________________________________
Bool_t AliAnalysisTaskTTreeFilter::ParseEvent(AliVEvent* event) 
{
    // parse the input event
    if(!PassesCuts(event)) return kFALSE;
    if(fDoQA) FillEventQA(kTRUE, event);

    // store zvertex position
    fEvent->SetZvtx(event->GetPrimaryVertex()->GetZ());

    // store centrality
    AliMultSelection *multSelection = static_cast<AliMultSelection*>(InputEvent()->FindListObject("MultSelection"));
    if(multSelection) fEvent->SetCentrality(multSelection->GetMultiplicityPercentile("V0M"));
    else  fEvent->SetCentrality(InputEvent()->GetCentrality()->GetCentralityPercentile("V0M"));
   
    // store event plane orientation
    fEvent->SetEventPlane(GetEventPlane(kFALSE, 2));
    fEvent->SetEventPlane3(GetEventPlane(kFALSE, 3));

    // store the run number
    fEvent->SetRunNumber(InputEvent()->GetRunNumber());
    
    // parse the tracks
    ParseTracks(event);

    // write the tree and perform cleanup
    PushToTTree();
    
    // jay !
    return kTRUE;
}
//________________________________________________________________________
void AliAnalysisTaskTTreeFilter::ParseTracks(AliVEvent* event)
{
    // parse tracks
    for(Int_t i(0), acceptedTracks(0); i < event->GetNumberOfTracks(); i++) {
        // track loop
        AliVTrack* track(static_cast<AliVTrack*>(event->GetTrack(i)));
        if(!PassesCuts(track)) continue;

        if(fDoQA) FillTrackQA(kTRUE, track);
        // push accepted track to tree
        AliGMFTTreeTrack* acceptedTrack = new((*fTrackArray)[acceptedTracks]) AliGMFTTreeTrack();
        acceptedTracks++;
        // add info
        acceptedTrack->SetPt(track->Pt());
        acceptedTrack->SetEta(track->Eta());
        acceptedTrack->SetPhi(track->Phi());
        acceptedTrack->SetCharge(track->Charge());
    }
    return;
}
//________________________________________________________________________
Float_t AliAnalysisTaskTTreeFilter::GetEventPlane(Bool_t useCache, Int_t nHarm) {
    // return the n-th order event plane

    // cache value can be used when the EP is called
    // multiple times per event
    if(useCache && nHarm == 2) return fCachedEP;
    if(useCache && nHarm == 3) return fCachedEP3;

    // otherwise recalculate it
    Double_t vzero[2][2];
    Double_t tpc[2];
    Double_t vzeroComb[2];
    Double_t psi2(-1), psi3(-1);
    // arrays which will hold the fit parameters
    switch (fDetectorType) {
        case kTPC : { 
            CalculateEventPlaneTPC(tpc);
            psi2 = tpc[0];         
            psi3 = tpc[1]; 
        } break;
        case kVZEROA :{ 
            CalculateEventPlaneVZERO(vzero);
            psi2 = vzero[0][0];    
            psi3 = vzero[0][1]; 
        } break;
        case kVZEROC : { 
             CalculateEventPlaneVZERO(vzero);
             psi2 = vzero[1][0];    
             psi3 = vzero[1][1]; 
        }  break;
        case kVZEROComb : { 
            CalculateEventPlaneCombinedVZERO(vzeroComb);
            psi2 = vzeroComb[0]; 
            psi3 = vzeroComb[1];
        }  break;
        default : break;
    }

    fCachedEP = psi2;
    fCachedEP3 = psi3;

    if(nHarm == 2)  return psi2;
    if(nHarm == 3)  return psi3;

    return 0;
}
//_____________________________________________________________________________
void AliAnalysisTaskTTreeFilter::CalculateEventPlaneVZERO(Double_t vzero[2][2]) const 
{
    // get the vzero event plane (a and c separately)
    switch (fCollisionPeriod) {
        case kPbPb10h : {
            // for 10h data, get the calibrated q-vector from the database
            Double_t QA2[] = {-999., -999.};
            Double_t QA3[] = {-999., -999.};
            Double_t QC2[] = {-999., -999.};
            Double_t QC3[] = {-999., -999.};
            CalculateQvectorVZERO(QA2, QC2, QA3, QC3);
            vzero[0][0] = .5*TMath::ATan2(QA2[1], QA2[0]);
            vzero[1][0] = .5*TMath::ATan2(QC2[1], QC2[0]);
            vzero[0][1] = (1./3.)*TMath::ATan2(QA3[1], QA3[0]);
            vzero[1][1] = (1./3.)*TMath::ATan2(QC3[1], QC3[0]);
            return;     // paranoid return
        } break;
       case kPbPb15o : {
            Int_t VZEROcentralityBin(GetVZEROCentralityBin());
            Double_t Qxan = 0, Qyan = 0;
            Double_t Qxcn = 0, Qycn = 0;
            Double_t Qxa3 = 0, Qya3 = 0;
            Double_t Qxc3 = 0, Qyc3 = 0;
            Double_t Qxa3_raw = 0, Qya3_raw = 0;
            Double_t Qxc3_raw = 0, Qyc3_raw = 0;
            Double_t sumMa = 0, sumMc = 0;
            AliVVZERO* aodV0 = (InputEvent())->GetVZEROData();
            for (Int_t iV0 = 0; iV0 < 64; iV0++) {
                Double_t phiV0 = TMath::PiOver4()*(0.5 + iV0 % 8);
                Float_t multv0 = aodV0->GetMultiplicity(iV0);
                if (iV0 < 32){
                    Double_t multCorC = -10;
                    if (iV0 < 8) multCorC = multv0/fVZEROgainEqualization->GetBinContent(iV0+1)*fVZEROgainEqualization->GetBinContent(1);
                    else if (iV0 >= 8 && iV0 < 16) multCorC = multv0/fVZEROgainEqualization->GetBinContent(iV0+1)*fVZEROgainEqualization->GetBinContent(9);
                    else if (iV0 >= 16 && iV0 < 24) multCorC = multv0/fVZEROgainEqualization->GetBinContent(iV0+1)*fVZEROgainEqualization->GetBinContent(17);
                    else if (iV0 >= 24 && iV0 < 32) multCorC = multv0/fVZEROgainEqualization->GetBinContent(iV0+1)*fVZEROgainEqualization->GetBinContent(25);
                    Qxcn += TMath::Cos(2.*phiV0) * multCorC;
                    Qycn += TMath::Sin(2.*phiV0) * multCorC;
                    Qxc3 += TMath::Cos(3.*phiV0) * multCorC;
                    Qxc3_raw += TMath::Cos(3.*phiV0) * multv0;
                    Qyc3 += TMath::Sin(3.*phiV0) * multCorC;
                    Qyc3_raw += TMath::Sin(3.*phiV0) * multv0;
                    sumMc = sumMc + multCorC;
                } else {
                    Double_t multCorA = -10;
                    if (iV0 >= 32 && iV0 < 40) multCorA = multv0/fVZEROgainEqualization->GetBinContent(iV0+1)*fVZEROgainEqualization->GetBinContent(33);
                    else if (iV0 >= 40 && iV0 < 48) multCorA = multv0/fVZEROgainEqualization->GetBinContent(iV0+1)*fVZEROgainEqualization->GetBinContent(41);
                    else if (iV0 >= 48 && iV0 < 56) multCorA = multv0/fVZEROgainEqualization->GetBinContent(iV0+1)*fVZEROgainEqualization->GetBinContent(49);
                    else if (iV0 >= 56 && iV0 < 64) multCorA = multv0/fVZEROgainEqualization->GetBinContent(iV0+1)*fVZEROgainEqualization->GetBinContent(57);
                    Qxan += TMath::Cos(2.*phiV0) * multCorA;
                    Qyan += TMath::Sin(2.*phiV0) * multCorA;
                    Qxa3 += TMath::Cos(3.*phiV0) * multCorA;
                    Qxa3_raw += TMath::Cos(3.*phiV0) * multv0;
                    Qya3 += TMath::Sin(3.*phiV0) * multCorA;
                    Qya3_raw += TMath::Sin(3.*phiV0) * multv0;
                    sumMa = sumMa + multCorA;
                }
            }
            if (sumMa <=0 || sumMc <= 0) return;
            Double_t iCentSPD = fEvent->GetCentrality();
            Double_t QyanCor = (Qyan - fMQ[1][0][0]->GetBinContent(iCentSPD+1))/fWQ[1][0][0]->GetBinContent(iCentSPD+1);
            Double_t QycnCor = (Qycn - fMQ[1][1][0]->GetBinContent(iCentSPD+1))/fWQ[1][1][0]->GetBinContent(iCentSPD+1);
            Double_t QxanCor = (Qxan - fMQ[0][0][0]->GetBinContent(iCentSPD+1))/fWQ[0][0][0]->GetBinContent(iCentSPD+1);
            Double_t QxcnCor = (Qxcn - fMQ[0][1][0]->GetBinContent(iCentSPD+1))/fWQ[0][1][0]->GetBinContent(iCentSPD+1);
            vzero[0][0] = .5*TMath::ATan2(QyanCor,QxanCor);
            vzero[1][0] = .5*TMath::ATan2(QycnCor,QxcnCor);
            QyanCor = (Qya3 - fMQ[1][0][1]->GetBinContent(iCentSPD+1))/fWQ[1][0][1]->GetBinContent(iCentSPD+1);
            QycnCor = (Qyc3 - fMQ[1][1][1]->GetBinContent(iCentSPD+1))/fWQ[1][1][1]->GetBinContent(iCentSPD+1);
            QxanCor = (Qxa3 - fMQ[0][0][1]->GetBinContent(iCentSPD+1))/fWQ[0][0][1]->GetBinContent(iCentSPD+1);
            QxcnCor = (Qxc3 - fMQ[0][1][1]->GetBinContent(iCentSPD+1))/fWQ[0][1][1]->GetBinContent(iCentSPD+1);
            vzero[0][1] = (1./3.)*TMath::ATan2(QyanCor,QxanCor);
            vzero[1][1] = (1./3.)*TMath::ATan2(QycnCor,QxcnCor);
        } break;
        default: {
            // by default use the ep from the event header (make sure EP selection task is enabeled!)
            Double_t a(0), b(0), c(0), d(0), e(0), f(0), g(0), h(0);
            vzero[0][0] = InputEvent()->GetEventplane()->CalculateVZEROEventPlane(InputEvent(), 8, 2, a, b);
            vzero[1][0] = InputEvent()->GetEventplane()->CalculateVZEROEventPlane(InputEvent(), 9, 2, c, d);
            vzero[0][1] = InputEvent()->GetEventplane()->CalculateVZEROEventPlane(InputEvent(), 8, 3, e, f);
            vzero[1][1] = InputEvent()->GetEventplane()->CalculateVZEROEventPlane(InputEvent(), 9, 3, g, h);
            return;
        }
    }
}
//_____________________________________________________________________________
void AliAnalysisTaskTTreeFilter::CalculateEventPlaneCombinedVZERO(Double_t* comb) const
{
    // return the combined vzero event plane
 
    // define some placeholders
    Double_t Q2[] = {-999., -999.};            
    Double_t Q3[] = {-999., -999.};
      
    switch (fCollisionPeriod) {
        // for 10h data call calibration info
        case kPbPb10h : {
            // get the calibrated q-vectors
            CalculateQvectorCombinedVZERO(Q2, Q3);
            comb[0] = .5*TMath::ATan2(Q2[1], Q2[0]);
            comb[1] = (1./3.)*TMath::ATan2(Q3[1], Q3[0]);
        } break;
        default : {
            // for all other types use calibrated event plane from the event header
            //
            // note that the code is a bit messy here. for 10h data retrieving q-vectors of 
            // the separate vzero detectors and combining the q-vectors have dedicated functions. 
            // for 11h however this is all done in this function (the lines below) 
            // reason is that the procedure is much shorter as the calibration is done in another task
            //
            // define some pleaceholders to the values by reference
            Double_t qx2a(0), qy2a(0), qx2c(0), qy2c(0), qx3a(0), qy3a(0), qx3c(0), qy3c(0);
            // get the q-vectors by reference
            InputEvent()->GetEventplane()->CalculateVZEROEventPlane(InputEvent(), 8, 2, qx2a, qy2a);
            InputEvent()->GetEventplane()->CalculateVZEROEventPlane(InputEvent(), 9, 2, qx2c, qy2c);
            InputEvent()->GetEventplane()->CalculateVZEROEventPlane(InputEvent(), 8, 3, qx3a, qy3a);
            InputEvent()->GetEventplane()->CalculateVZEROEventPlane(InputEvent(), 9, 3, qx3c, qy3c);

            // combine the vzera and vzeroc signal
            Q2[0] = qx2a+qx2c;
            Q2[1] = qy2a+qy2c;
            Q3[0] = qx3a+qx3c;
            Q3[1] = qy3a+qy3c;

            comb[0] = .5*TMath::ATan2(Q2[1], Q2[0]);
            comb[1] = (1./3.)*TMath::ATan2(Q3[1], Q3[0]);
        }
    }
}
//_____________________________________________________________________________
void AliAnalysisTaskTTreeFilter::CalculateEventPlaneTPC(Double_t* tpc)
{
   // grab the TPC event plane
   Double_t qx2(0), qy2(0);     // for psi2
   Double_t qx3(0), qy3(0);     // for psi3
   AliVEvent* event = static_cast<AliVEvent*>(InputEvent());
   for(Int_t i(0); i < event->GetNumberOfTracks(); i++) {
       // track loop
       AliVTrack* track(static_cast<AliVTrack*>(event->GetTrack(i)));
       if(!PassesCuts(track)) continue;
       qx2+= TMath::Cos(2.*track->Phi());
       qy2+= TMath::Sin(2.*track->Phi());
       qx3+= TMath::Cos(3.*track->Phi());
       qy3+= TMath::Sin(3.*track->Phi());
   }
   tpc[0] = .5*TMath::ATan2(qy2, qx2);
   tpc[1] = (1./3.)*TMath::ATan2(qy3, qx3);
}

//_____________________________________________________________________________
void AliAnalysisTaskTTreeFilter::CalculateQvectorVZERO(Double_t Qa2[2], Double_t Qc2[2], Double_t Qa3[2], Double_t Qc3[2]) const
{
    // return the calibrated 2nd and 3rd order q-vectors for vzeroa and vzeroc
    // function takes arrays as arguments, which correspond to vzero info in the following way
    // 
    // Qa2[0] = Qx2 for vzero A         Qa2[1] = Qy2 for vzero A (etc)
    
    // placeholders 
    Double_t phi(-999.), mult(-999.); 
    // reset placeholders for Q-vector components
    Qa2[0] = 0.;    Qc2[0] = 0.;    Qa3[0] = 0.;    Qc3[0] = 0.;
    Qa2[1] = 0.;    Qc2[1] = 0.;    Qa3[1] = 0.;    Qc3[1] = 0.;
    // for qa purposes, save also raw signal
    Double_t QaX(0), QaY(0), QcX(0), QcY(0);
    for(Int_t i(0); i < 64; i++) {
        // loop over all scintillators, construct Q-vectors in the same loop
        phi     = TMath::PiOver4()*(0.5+i%8);
        mult    = InputEvent()->GetVZEROData()->GetMultiplicity(i);
        // note that disabled rings have already been excluded in ReadVZEROCalibration2010h
        if(i < 32) {    // v0c side
            // fill Q-vectors for v0c side
            Qc2[0] += mult*TMath::Cos(2.*phi)*fVZEROCpol/fVZEROgainEqualization->GetBinContent(1+i);
            Qc3[0] += mult*TMath::Cos(3.*phi)*fVZEROCpol/fVZEROgainEqualization->GetBinContent(1+i);
            Qc2[1] += mult*TMath::Sin(2.*phi)*fVZEROCpol/fVZEROgainEqualization->GetBinContent(1+i);
            Qc3[1] += mult*TMath::Sin(3.*phi)*fVZEROCpol/fVZEROgainEqualization->GetBinContent(1+i);
        } else {       // v0a side
            // fill Q-vectors for v0a side
            Qa2[0] += mult*TMath::Cos(2.*phi)*fVZEROApol/fVZEROgainEqualization->GetBinContent(1+i);
            Qa3[0] += mult*TMath::Cos(3.*phi)*fVZEROApol/fVZEROgainEqualization->GetBinContent(1+i);
            Qa2[1] += mult*TMath::Sin(2.*phi)*fVZEROApol/fVZEROgainEqualization->GetBinContent(1+i);
            Qa3[1] += mult*TMath::Sin(3.*phi)*fVZEROApol/fVZEROgainEqualization->GetBinContent(1+i);
        }
    }
    // get the cache index and read the correction terms from the cache
    Int_t VZEROcentralityBin(GetVZEROCentralityBin());

    Double_t Qx2amean = fMeanQ[VZEROcentralityBin][1][0];
    Double_t Qx2arms  = fWidthQ[VZEROcentralityBin][1][0];
    Double_t Qy2amean = fMeanQ[VZEROcentralityBin][1][1];
    Double_t Qy2arms  = fWidthQ[VZEROcentralityBin][1][1];

    Double_t Qx2cmean = fMeanQ[VZEROcentralityBin][0][0];
    Double_t Qx2crms  = fWidthQ[VZEROcentralityBin][0][0];
    Double_t Qy2cmean = fMeanQ[VZEROcentralityBin][0][1];
    Double_t Qy2crms  = fWidthQ[VZEROcentralityBin][0][1];      

    Double_t Qx3amean = fMeanQv3[VZEROcentralityBin][1][0];
    Double_t Qx3arms  = fWidthQv3[VZEROcentralityBin][1][0];
    Double_t Qy3amean = fMeanQv3[VZEROcentralityBin][1][1];
    Double_t Qy3arms  = fWidthQv3[VZEROcentralityBin][1][1];

    Double_t Qx3cmean = fMeanQv3[VZEROcentralityBin][0][0];
    Double_t Qx3crms  = fWidthQv3[VZEROcentralityBin][0][0];
    Double_t Qy3cmean = fMeanQv3[VZEROcentralityBin][0][1];
    Double_t Qy3crms  = fWidthQv3[VZEROcentralityBin][0][1];    

    // update the weighted q-vectors with the re-centered values
    Qa2[0] = (Qa2[0] - Qx2amean)/Qx2arms;
    Qa2[1] = (Qa2[1] - Qy2amean)/Qy2arms;
    Qc2[0] = (Qc2[0] - Qx2cmean)/Qx2crms;
    Qc2[1] = (Qc2[1] - Qy2cmean)/Qy2crms;

    Qa3[0] = (Qa3[0] - Qx3amean)/Qx3arms;
    Qa3[1] = (Qa3[1] - Qy3amean)/Qy3arms;
    Qc3[0] = (Qc3[0] - Qx3cmean)/Qx3crms;
    Qc3[1] = (Qc3[1] - Qy3cmean)/Qy3crms;
}
//_____________________________________________________________________________
void AliAnalysisTaskTTreeFilter::CalculateQvectorCombinedVZERO(Double_t Q2[2], Double_t Q3[2]) const
{
    // calculate calibrated q-vector of the combined vzeroa, vzeroc system
    // this is somewhat ugly as CalculateQvectorCombinedVZERO is called more than once per event
    // but for now it will have to do ...

    // first step: retrieve the q-vectors component-wise per vzero detector
    Double_t QA2[] = {-999., -999.};
    Double_t QA3[] = {-999., -999.};
    Double_t QC2[] = {-999., -999.};
    Double_t QC3[] = {-999., -999.};
    CalculateQvectorVZERO(QA2, QC2, QA3, QC3);

    // bookkkeep these guys
    Double_t qx2a(QA2[0]), qy2a(QA2[1]), qx2c(QC2[0]), qy2c(QC2[1]);  
    // combine the vzera and vzeroc signal
    Q2[0] = QA2[0]+QC2[0];
    Q2[1] = QA2[1]+QC2[1];
    Q3[0] = QA3[0]+QC3[0];
    Q3[1] = QA3[1]+QC3[1];

}
//________________________________________________________________________
void AliAnalysisTaskTTreeFilter::PushToTTree()
{
    // push info to tree and do cleanup for next iteration
    fTree->Fill();
    fTrackArray->Clear();
}
//________________________________________________________________________
Bool_t AliAnalysisTaskTTreeFilter::PassesCuts(AliVEvent* event)
{
    // check the event cuts
    if(fDoQA && event) FillEventQA(kFALSE, event);
    return fEventCuts->IsSelected(event);
}
//________________________________________________________________________
Bool_t AliAnalysisTaskTTreeFilter::PassesCuts(AliVTrack* track)
{
    // track cuts would go here
    if(fDoQA && track) FillTrackQA(kFALSE, track);
    return fTrackCuts->IsSelected(track);
}
//________________________________________________________________________
void AliAnalysisTaskTTreeFilter::Terminate(Option_t *)
{ 
    // terminate
    if(fDoQA) {
        fHistogramManager->StoreManager("TTreeFilterQA.root");
    }
}
//_____________________________________________________________________________
void AliAnalysisTaskTTreeFilter::ReadVZEROCalibration2010h()
{
    //  check if the database file is open, if not, open it
    if(!fOADB || fOADB->IsZombie()) fOADB = TFile::Open("$ALICE_PHYSICS/OADB/PWGCF/VZERO/VZEROcalibEP.root");
    if(fOADB->IsZombie()) {
        printf("OADB file $ALICE_PHYSICS/OADB/PWGCF/VZERO/VZEROcalibEP.root cannot be opened, CALIBRATION FAILED !");
        return;
    }

    AliOADBContainer *cont = (AliOADBContainer*) fOADB->Get("hMultV0BefCorr");
    if(!cont){
        // see if database is readable
        printf("OADB object hMultV0BefCorr is not available in the file\n");
        return; 
    }
    Int_t run(fRunNumber);
    if(!(cont->GetObject(run))){
        // if the run isn't recognized fall back to a default run
        printf("OADB object hMultVZEROBefCorr is not available for run %i (used default run 137366)\n",run);
        run = 137366;
    }
    // step get the proper multiplicity weights from the vzero signal
    fVZEROgainEqualization = ((TH2F*)cont->GetObject(run))->ProfileX();
    if(!fVZEROgainEqualization) {
        AliFatal(Form("%s: Fatal error, couldn't read fVZEROgainEqualization from OADB object < \n", GetName()));
        return;
    }

    TF1* fpol0 = new TF1("fpol0","pol0");
    fVZEROgainEqualization->Fit(fpol0, "N0", "", 0, 31);
    fVZEROCpol = fpol0->GetParameter(0);
    fVZEROgainEqualization->Fit(fpol0, "N0", "", 32, 64);
    fVZEROApol = fpol0->GetParameter(0);

    // step extract the information to re-weight the q-vectors 
    for(Int_t iside=0;iside<2;iside++){
        for(Int_t icoord=0;icoord<2;icoord++){
            for(Int_t i=0;i  < 9;i++){
                char namecont[100];
                if(iside==0 && icoord==0)
                  snprintf(namecont,100,"hQxc2_%i",i);
                else if(iside==1 && icoord==0)
                  snprintf(namecont,100,"hQxa2_%i",i);
                else if(iside==0 && icoord==1)
                  snprintf(namecont,100,"hQyc2_%i",i);
                else if(iside==1 && icoord==1)
                  snprintf(namecont,100,"hQya2_%i",i);

                cont = (AliOADBContainer*) fOADB->Get(namecont);
                if(!cont){
                    printf("OADB object %s is not available in the file\n",namecont);
                    return;     
                }
        
                if(!(cont->GetObject(run))){
                    printf("OADB object %s is not available for run %i (used run 137366)\n",namecont,run);
                    run = 137366;
                }

                // store info for all centralities to cache
                fMeanQ[i][iside][icoord] = ((TH1F *) cont->GetObject(run))->GetMean();
                fWidthQ[i][iside][icoord] = ((TH1F *) cont->GetObject(run))->GetRMS();

                //for v3
                if(iside==0 && icoord==0)
                  snprintf(namecont,100,"hQxc3_%i",i);
                else if(iside==1 && icoord==0)
                  snprintf(namecont,100,"hQxa3_%i",i);
                else if(iside==0 && icoord==1)
                  snprintf(namecont,100,"hQyc3_%i",i);
                else if(iside==1 && icoord==1)
                  snprintf(namecont,100,"hQya3_%i",i);

                cont = (AliOADBContainer*) fOADB->Get(namecont);
                if(!cont){
                    printf("OADB object %s is not available in the file\n",namecont);
                    return;     
                }
                
                if(!(cont->GetObject(run))){
                    printf("OADB object %s is not available for run %i (used run 137366)\n",namecont,run);
                    run = 137366;
                }
                // store info for all centralities to cache
                fMeanQv3[i][iside][icoord] = ((TH1F *) cont->GetObject(run))->GetMean();
                fWidthQv3[i][iside][icoord] = ((TH1F *) cont->GetObject(run))->GetRMS();
            }
        }
    }
    // cleanup. the opened file is closed in the destructor, otherwise fVZEROgainEqualization is no longer available
    delete fpol0;
    // for qa store the runnumber that is currently used for calibration purposes
}
//_____________________________________________________________________________
void AliAnalysisTaskTTreeFilter::ReadVZEROCalibration2015o() {
     #ifdef ALIANALYSISTASKJETV3_DEBUG_FLAG_1
        printf("__FILE__ = %s \n __LINE __ %i , __FUNC__ %s \n ", __FILE__, __LINE__, __func__);
    #endif
    if(!fOADB || fOADB->IsZombie()) {
        if (!gGrid) TGrid::Connect("alien");
        fOADB = TFile::Open("alien:///alice/cern.ch/user/r/rbertens/calibV0HIR.root");
    }
    if(fOADB->IsZombie()) {
	printf("OADB file could not be opened CALIBRATION FAILED !");
	return;
    }

    AliOADBContainer* cont = (AliOADBContainer*) fOADB->Get("hMultV0BefCorPfpx");
    fVZEROgainEqualization= ((TH1D*) cont->GetObject(fRunNumber));

    for(Int_t i(0); i < 2; i++) {
        Int_t fNHarm = i+2;

        AliOADBContainer* contQxnam = 0;
        if (fNHarm == 2)       contQxnam = (AliOADBContainer*) fOADB->Get("fqxa2m");
        else if (fNHarm == 3)  contQxnam = (AliOADBContainer*) fOADB->Get("fqxa3m");
        if(!contQxnam || !(contQxnam->GetObject(fRunNumber))) {
            printf("OADB object fqyanm is not available for run %i\n", fRunNumber);
            return;
        }
        fMQ[0][0][i] = ((TH1D*) contQxnam->GetObject(fRunNumber));

        AliOADBContainer* contQynam = 0;
        if (fNHarm == 2)       contQynam = (AliOADBContainer*) fOADB->Get("fqya2m");
        else if (fNHarm == 3)  contQynam = (AliOADBContainer*) fOADB->Get("fqya3m");
        if(!contQynam || !(contQynam->GetObject(fRunNumber))) {
            printf("OADB object fqyanm is not available for run %i\n", fRunNumber);
            return;
        }
        fMQ[1][0][i] = ((TH1D*) contQynam->GetObject(fRunNumber));
        
        AliOADBContainer* contQxnas = 0;
        if (fNHarm == 2)       contQxnas = (AliOADBContainer*) fOADB->Get("fqxa2s");
        else if (fNHarm == 3)  contQxnas = (AliOADBContainer*) fOADB->Get("fqxa3s");
        
        if(!contQxnas || !(contQxnas->GetObject(fRunNumber))) {
            printf("OADB object fqxans is not available for run %i\n", fRunNumber);
            return;
        }
        fWQ[0][0][i] = ((TH1D*) contQxnas->GetObject(fRunNumber));
        
        AliOADBContainer* contQynas = 0;
        if (fNHarm == 2)       contQynas = (AliOADBContainer*) fOADB->Get("fqya2s");
        else if (fNHarm == 3)  contQynas = (AliOADBContainer*) fOADB->Get("fqya3s");
        
        if(!contQynas || !(contQynas->GetObject(fRunNumber))){
            printf("OADB object fqyans is not available for run %i\n", fRunNumber);
            return;
        }
        fWQ[1][0][i] = ((TH1D*) contQynas->GetObject(fRunNumber));
        
        AliOADBContainer* contQxncm = 0;
        if (fNHarm == 2)       contQxncm = (AliOADBContainer*) fOADB->Get("fqxc2m");
        else if (fNHarm == 3)  contQxncm = (AliOADBContainer*) fOADB->Get("fqxc3m");
        
        if(!contQxncm || !(contQxncm->GetObject(fRunNumber))) {
            printf("OADB object fqxcnm is not available for run %i\n", fRunNumber);
            return;
        }
        fMQ[0][1][i] = ((TH1D*) contQxncm->GetObject(fRunNumber));
        
        AliOADBContainer* contQyncm = 0;
        if (fNHarm == 2)       contQyncm = (AliOADBContainer*) fOADB->Get("fqyc2m");
        else if (fNHarm == 3)  contQyncm = (AliOADBContainer*) fOADB->Get("fqyc3m");
        if(!contQyncm || !(contQyncm->GetObject(fRunNumber))) {
            printf("OADB object fqyc2m is not available for run %i\n", fRunNumber);
            return;
        }
        fMQ[1][1][i] = ((TH1D*) contQyncm->GetObject(fRunNumber));
        
        AliOADBContainer* contQxncs = 0;
        if (fNHarm == 2)        contQxncs = (AliOADBContainer*) fOADB->Get("fqxc2s");
        else if (fNHarm == 3)   contQxncs = (AliOADBContainer*) fOADB->Get("fqxc3s");
        if(!contQxncs || !(contQxncs->GetObject(fRunNumber))) {
            printf("OADB object fqxc2s is not available for run %i\n", fRunNumber);
            return;
        }
        fWQ[0][1][i] = ((TH1D*) contQxncs->GetObject(fRunNumber));
        
        AliOADBContainer* contQyncs = 0;
        if (fNHarm == 2)        contQyncs = (AliOADBContainer*) fOADB->Get("fqyc2s");
        else if (fNHarm == 3)   contQyncs = (AliOADBContainer*) fOADB->Get("fqyc3s");
        if(!contQyncs || !(contQyncs->GetObject(fRunNumber))){
            printf("OADB object fqycns is not available for run %i\n", fRunNumber);
            return;
        }
        fWQ[1][1][i] = ((TH1D*) contQyncs->GetObject(fRunNumber));
    }
}    
//_____________________________________________________________________________
Int_t AliAnalysisTaskTTreeFilter::GetVZEROCentralityBin() const
{
    // return cache index number corresponding to the event centrality
    Float_t v0Centr(InputEvent()->GetCentrality()->GetCentralityPercentile("V0M"));
    if(v0Centr < 5) return 0;
    else if(v0Centr < 10) return 1;
    else if(v0Centr < 20) return  2;
    else if(v0Centr < 30) return  3;
    else if(v0Centr < 40) return  4;
    else if(v0Centr < 50) return  5;
    else if(v0Centr < 60) return  6;
    else if(v0Centr < 70) return  7;
    else return 8;
}
//_____________________________________________________________________________
void AliAnalysisTaskTTreeFilter::FillEventQA(Bool_t cutsApplied, AliVEvent* event) {
    // fill event QA
    if(!fHistogramManager) return;

    if(!cutsApplied) {
        fHistogramManager->Fill("fHistBeforeVertex", event->GetPrimaryVertex()->GetZ());
        fHistogramManager->Fill("fHistBeforeCentrality", event->GetCentrality()->GetCentralityPercentile("V0M"));
        fHistogramManager->Fill("fHistBeforeEventPlane", GetEventPlane(kTRUE));
    } else {
        fHistogramManager->Fill("fHistAfterVertex", event->GetPrimaryVertex()->GetZ());
        fHistogramManager->Fill("fHistAfterCentrality", fEvent->GetCentrality());
        fHistogramManager->Fill("fHistAfterEventPlane", GetEventPlane(kTRUE));
    }
}
//_____________________________________________________________________________
void AliAnalysisTaskTTreeFilter::FillTrackQA(Bool_t cutsApplied, AliVTrack* track) {
    // fill track QA
    if(!fHistogramManager) return;

    if(!cutsApplied) {
        fHistogramManager->Fill("fHistBeforePt", track->Pt());
        fHistogramManager->Fill("fHistBeforeEta", track->Eta());
        fHistogramManager->Fill("fHistBeforePhi", track->Phi());
        fHistogramManager->Fill("fHistBeforeEtaPhi", track->Eta(), track->Phi());
    } else {
        fHistogramManager->Fill("fHistAfterPt", track->Pt());
        fHistogramManager->Fill("fHistAfterEta", track->Eta());
        fHistogramManager->Fill("fHistAfterPhi", track->Phi());
        fHistogramManager->Fill("fHistAfterEtaPhi", track->Eta(), track->Phi());
    }
}

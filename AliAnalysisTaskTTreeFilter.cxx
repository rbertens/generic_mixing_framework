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

// AliRoot includes
#include <AliAnalysisManager.h>
#include <AliInputEventHandler.h>
#include <AliOADBContainer.h>
#include <AliLog.h>
#include <AliVEvent.h>
#include <AliVVertex.h>
#include <AliCentrality.h>
#include <AliVTrack.h>
#include <AliEventplane.h>
#include <AliVVZERO.h>

// local includes
#include "AliGMFTTreeHeader.h"
#include "AliGMFTTreeTrack.h"
#include "AliGMFEventCuts.h"
#include "AliGMFTrackCuts.h"


ClassImp(AliAnalysisTaskTTreeFilter)

//_____________________________________________________________________________
AliAnalysisTaskTTreeFilter::AliAnalysisTaskTTreeFilter():
    AliAnalysisTaskSE(),
    fEvent(0x0),
    fTrackArray(0x0),
    fEventCuts(0x0),
    fTrackCuts(0x0),
    fEventPlaneN(2),
    fDetectorType(kVZEROComb),
    fCollisionPeriod(kPbPb10h),
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
            }
        }
    }
}
//______________________________________________________________________________
AliAnalysisTaskTTreeFilter::AliAnalysisTaskTTreeFilter(const char *name):
    AliAnalysisTaskSE(name),
    fEvent(0x0),
    fTrackArray(0x0),
    fEventCuts(0x0),
    fTrackCuts(0x0),
    fEventPlaneN(2),
    fDetectorType(kVZEROComb),
    fCollisionPeriod(kPbPb10h),
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
        switch (fCollisionPeriod) {
            case kPbPb10h : {
                ReadVZEROCalibration2010h(); 
                return kTRUE;
            } break;
            default : break;
        }
    }
}
//______________________________________________________________________________
void AliAnalysisTaskTTreeFilter::UserCreateOutputObjects()
{ 
    // check for manager
    AliAnalysisManager *man=AliAnalysisManager::GetAnalysisManager();
    AliInputEventHandler *inputHandler=static_cast<AliInputEventHandler*>(man->GetInputEventHandler());
    if (!inputHandler) AliFatal(" > no input detected - aborting <");

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

    // store some event info
    fEvent->SetZvtx(event->GetPrimaryVertex()->GetZ());
    
    // just some dummy value
    fEvent->SetEventPlane(GetEventPlane());
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
Float_t AliAnalysisTaskTTreeFilter::GetEventPlane() {
    // return the n-th order event plane
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
    if(fEventPlaneN == 2) return psi2;
    if(fEventPlaneN == 3) return psi3;

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
    return fEventCuts->IsSelected(event);
}
//________________________________________________________________________
Bool_t AliAnalysisTaskTTreeFilter::PassesCuts(AliVTrack* track)
{
    // track cuts would go here
    return fTrackCuts->IsSelected(track);
}
//________________________________________________________________________
void AliAnalysisTaskTTreeFilter::Terminate(Option_t *)
{ 
    // terminate
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


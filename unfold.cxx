
#if !defined(__CINT__) || defined(__MAKECINT__)
#include <iostream>
using std::cout;
using std::endl;

#include "TRandom.h"
#include "TH1D.h"

#include "/home/rbertens/Documents/CERN/jet-flow/results/2017/UNFOLDING/RooUnfold/include/RooUnfoldResponse.h"
#include "/home/rbertens/Documents/CERN/jet-flow/results/2017/UNFOLDING/RooUnfold/include/RooUnfoldBayes.h"
//#include "RooUnfoldSvd.h"
//#include "RooUnfoldTUnfold.h"
#endif

void unfold()
{
#ifdef __CINT__
    gSystem->Load("/home/rbertens/Documents/CERN/jet-flow/results/2017/UNFOLDING/RooUnfold/libRooUnfold");
#endif

    cout << "==================================== TRAIN SAMPLE ==============================" << endl;
    RooUnfoldResponse response (100, -30, 70);

    TFile *f = new TFile("/home/rbertens/Documents/CERN/jet-flow/results/2017/UNFOLDING/responses/leticia/TrainEmbeddingR02.root");
    TTree *t1 = (TTree*)f->Get("fTreeJetShape_MC_Merged");
    Float_t ptJet, ptJetMatch;
    Int_t ev;
    t1->SetBranchAddress("ptJet",&ptJet);
    t1->SetBranchAddress("ptJetMatch",&ptJetMatch);

    //read all entries and fill the histograms
    Long64_t nentries = t1->GetEntries();
    for (Long64_t i=0;i<nentries;i++) {
        t1->GetEntry(i);
        response.Fill(ptJet,ptJetMatch);
    }


    cout << "==================================== GET JETS ==================================" << endl;

    int r = 2;


    TFile meJets(Form("/home/rbertens/Documents/CERN/jet-flow/results/2017/SEPTEMBER/event_mixing/analyzed_events_OLD/ME/40_50/merged_myMixedJets_R0%i.root", r));
    TFile seJets(Form("/home/rbertens/Documents/CERN/jet-flow/results/2017/SEPTEMBER/event_mixing/analyzed_events_OLD/SE/40_50/merged_SE_jets_40_50_R0%i.root", r));

    TH1D* meHistJets = (TH1D*)meJets.Get(Form("fHistJetPtSubtracted__R%i", r));
    TH1D* seHistJets = (TH1D*)seJets.Get(Form("fHistJetPtSubtracted__R%i", r));

    TH1D* meHistCent = (TH1D*)meJets.Get(Form("fHistCentrality__R%i", r));
    Int_t meEvents = meHistCent->GetEntries();

    TH1D* seHistCent = (TH1D*)seJets.Get(Form("fHistCentrality__R%i", r));
    Int_t seEvents = seHistCent->GetEntries();

    meHistJets->SetTitle("ME jets");
    meHistJets->GetYaxis()->SetTitle("#frac{1}{N_{evt}}#frac{d#it{N}}{d#it{p}_{T}} (GeV/#it{c})^{-1}");
    meHistJets->SetLineColor(kRed);
    meHistJets->SetMarkerColor(kRed);

    seHistJets->SetTitle("SE jets");

    meHistJets->DrawCopy();
    seHistJets->DrawCopy("same");


    seHistJets->Add(meHistJets, -1);




    cout << "==================================== UNFOLD ===================================" << endl;
    RooUnfoldBayes   unfold (&response, seHistJets, 4);    // OR
    //RooUnfoldSvd     unfold (&response, hMeas, 20);   // OR
    //RooUnfoldTUnfold unfold (&response, hMeas);

    TH1D* hReco= (TH1D*) unfold.Hreco();

    unfold.PrintTable (cout, seHistJets);
    hReco->DrawCopy();
    seHistJets->DrawCopy("SAME");
}

#ifndef __CINT__
int main () { RooUnfoldExample(); return 0; }  // Main program when run stand-alone
#endif

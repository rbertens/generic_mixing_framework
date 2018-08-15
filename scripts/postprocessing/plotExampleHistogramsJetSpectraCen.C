void plotExampleHistogramsJetSpectraCen() {

    thesis();

    TFile meJets("ME/0_5/ME_jets_0_5.root");
    TFile seJets("SE/0_5/SE_jets_0_5.root");

    //TFile meJets("ME/40_50/ME_jets_40_50.root");
    //TFile seJets("SE/40_50/SE_jets_40_50.root");

    for(Int_t r = 2; r < 7; r++) {
        if(r==3||r==5) continue;
        TH1D* meHistJets = (TH1D*)meJets.Get(Form("fHistJetPtSubtracted__R%i", r));
        TH1D* seHistJets = (TH1D*)seJets.Get(Form("fHistJetPtSubtracted__R%i", r));

        TH1D* meHistCent = (TH1D*)meJets.Get(Form("fHistCentrality__R%i", r));
        Int_t meEvents = meHistCent->GetEntries();

        TH1D* seHistCent = (TH1D*)seJets.Get(Form("fHistCentrality__R%i", r));
        Int_t seEvents = seHistCent->GetEntries();

        cout << "seEvents" << seEvents << endl;
        cout << "meEvents" << meEvents << endl;


        meHistJets->SetTitle("ME jets");
        meHistJets->GetYaxis()->SetTitle("#frac{1}{N_{evt}}#frac{d#it{N}}{d#it{p}_{T}} (GeV/#it{c})^{-1}");
        meHistJets->SetLineColor(kRed);
        meHistJets->SetMarkerColor(kRed);

        seHistJets->SetTitle("SE jets");

        seHistJets->SetLineWidth(4);
        meHistJets->SetLineWidth(4);

        seHistJets->Scale(1./double(seEvents), "width");
        meHistJets->Scale(1./double(meEvents), "width");


        TCanvas* c = new TCanvas(Form("illustration R%i", r), Form("illustration R%i", r), 800, 400);
        c->Divide(2);
        c->cd(1);

        meHistJets->DrawCopy();
        seHistJets->DrawCopy("same");


        c->cd(2);
        //    seHistJets->Add(meHistJets, -1);
        //    seHistJets->GetYaxis()->SetTitle("difference (same events - mixed events)");



        TF1* line = new TF1("line", "pol0", -30, 70);
        line->SetParameter(0, 1);
        seHistJets->Divide(meHistJets);    
        seHistJets->GetYaxis()->SetTitle("same events / mixed events");


        seHistJets->DrawCopy();
        line->DrawCopy("same");
        /*
           TFile fMQA("mixingQA.root");
           TH1D* unmixedPt = (TH1D*)fMQA.Get("fHistAcceptedMultiplicity");
           TH1D* mixedPt = (TH1D*)fMQA.Get("fHistMixedMultiplicity");

           unmixedPt->Divide(mixedPt);
           TCanvas *c2 = new TCanvas("QA", "QA", 400,400);
           unmixedPt->DrawCopy();

*/
    }
}


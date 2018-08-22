////////////////////////////////////////////////////////////////////////////////


Double_t binsTrue[] = {0, 10, 20, 30, 40, 50, 60, 70, 80,90, 100, 200};
Double_t binsRec[] = {10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80};


////////////////////////////////////////////////////////////////////////////////



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

//_____________________________________________________________________________
TMatrixD* CalculatePearsonCoefficients(TMatrixD* covarianceMatrix) 
{
    // Calculate pearson coefficients from covariance matrix
    TMatrixD *pearsonCoefficients((TMatrixD*)covarianceMatrix->Clone("pearsonCoefficients"));
    Int_t nrows(covarianceMatrix->GetNrows()), ncols(covarianceMatrix->GetNcols());
    Double_t pearson(0.);
    if(nrows==0 && ncols==0) return 0x0;
    for(Int_t row = 0; row < nrows; row++) {
        for(Int_t col = 0; col<ncols; col++) {
            if((*covarianceMatrix)(row,row)!=0. && (*covarianceMatrix)(col,col)!=0.) pearson = (*covarianceMatrix)(row,col)/TMath::Sqrt((*covarianceMatrix)(row,row)*(*covarianceMatrix)(col,col));
            (*pearsonCoefficients)(row,col) = pearson;
        }
    }
    return pearsonCoefficients;
}

//_____________________________________________________________________________
TH1D* RebinTH1D(TH1D* histo, TArrayD* bins) {
    // return a TH1D with the supplied histogram rebinned to the supplied bins
    // the returned histogram is new, the original is deleted from the heap if kill is true
    if(!histo || !bins) {
        printf(" > RebinTH1D:: fatal error, NULL pointer passed < \n");
        return NULL;
    }
    // create the output histo
    TString name = histo->GetName();
    TH1D* rebinned = 0x0;
    // check if the histogram is normalized. if so, fall back to ROOT style rebinning
    // if not, rebin manually
    if(histo->GetBinContent(1) > 1 ) {
        rebinned = new TH1D(name.Data(), name.Data(), bins->GetSize()-1, bins->GetArray());
        for(Int_t i(0); i < histo->GetXaxis()->GetNbins(); i++) {
            // loop over the bins of the old histo and fill the new one with its data
            rebinned->Fill(histo->GetBinCenter(i+1), histo->GetBinContent(i+1));
        }
    } else rebinned = reinterpret_cast<TH1D*>(histo->Rebin(bins->GetSize()-1, name.Data(), bins->GetArray()));
    return rebinned;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TH2* RebinTH2D(TH2 *hRMFine, TH2 *hRM) {

    //
    // Rebin matrix hRMFine to dimensions of hRM
    // function returns matrix in TH2D format (hRM2) with dimensions from hRM
    //

    TH2 *hRM2 = (TH2*)hRM->Clone("hRM2");
    hRM2->Reset();

    //First normalize columns of input
    const Int_t nbinsNorm = hRM2->GetNbinsX();
    TArrayF *normVector = new TArrayF(nbinsNorm);

    for(int ix=1; ix<=hRM2->GetNbinsX(); ix++) {
        Double_t xLow = hRM2->GetXaxis()->GetBinLowEdge(ix);
        Double_t xUp = hRM2->GetXaxis()->GetBinUpEdge(ix);
        //cout << "xLow: " << xLow << " xUp: " << xUp << "\t center: " << hRM2->GetXaxis()->GetBinCenter(ix) << endl;
        Int_t binxLowFine = hRMFine->GetXaxis()->FindBin(xLow);
        Int_t binxUpFine = hRMFine->GetXaxis()->FindBin(xUp)-1;
        //cout << "xLowFine: " << hRMFine->GetXaxis()->GetBinLowEdge(binxLowFine) << "\txUpFine: " << hRMFine->GetXaxis()->GetBinUpEdge(binxUpFine) << endl;
        normVector->SetAt(hRMFine->Integral(binxLowFine,binxUpFine,1,hRMFine->GetYaxis()->GetNbins()),ix-1);
    }

    Double_t content, oldcontent = 0.;
    Int_t ixNew = 0;
    Int_t iyNew = 0;
    Double_t xvalLo, xvalUp, yvalLo, yvalUp;
    Double_t xmin = hRM2->GetXaxis()->GetXmin();
    Double_t ymin = hRM2->GetYaxis()->GetXmin();
    Double_t xmax = hRM2->GetXaxis()->GetXmax();
    Double_t ymax = hRM2->GetYaxis()->GetXmax();
    for(int ix=1; ix<=hRMFine->GetXaxis()->GetNbins(); ix++) {
        xvalLo = hRMFine->GetXaxis()->GetBinLowEdge(ix);
        xvalUp = hRMFine->GetXaxis()->GetBinUpEdge(ix);
        if(xvalLo<xmin || xvalUp>xmax) continue;
        ixNew = hRM2->GetXaxis()->FindBin(hRMFine->GetXaxis()->GetBinCenter(ix));
        for(int iy=1; iy<=hRMFine->GetYaxis()->GetNbins(); iy++) {
            yvalLo = hRMFine->GetYaxis()->GetBinLowEdge(iy);
            yvalUp = hRMFine->GetYaxis()->GetBinUpEdge(iy);
            if(yvalLo<ymin || yvalUp>ymax) continue;
            content = hRMFine->GetBinContent(ix,iy);
            iyNew = hRM2->GetYaxis()->FindBin(hRMFine->GetYaxis()->GetBinCenter(iy));
            oldcontent = hRM2->GetBinContent(ixNew,iyNew);

            Double_t weight = 1.;
            if(normVector->At(ixNew-1)>0.) {
                weight = 1./normVector->At(ixNew-1);
            }
            hRM2->SetBinContent(ixNew,iyNew,oldcontent+content*weight);
        }
    }

    if(normVector) delete normVector;

    return hRM2;

}



//_____________________________________________________________________________
TGraphErrors* GetRatio(TH1 *h1, TH1* h2) 
{
    // return ratio of h1 / h2
    // histograms can have different binning. errors are propagated as uncorrelated
    if(!(h1 && h2) ) {
        printf(" GetRatio called with NULL argument(s) \n ");
        return 0x0;
    }
    cout << h1->GetName() << endl;
    cout << h2->GetName() << endl;
    Int_t j(0);
    TGraphErrors *gr = new TGraphErrors();
    gr->GetXaxis()->SetTitle("p_{T, jet} [GeV/c]");
    Double_t binCent(0.), ratio(0.), error2(0.), binWidth(0.);
    TH1* dud((TH1*)h1->Clone("dud"));
    dud->Sumw2();
    h1->Sumw2();
    h2->Sumw2();
    if(!dud->Divide(h1, h2)) {
        printf(" > ROOT failed to divide two histogams, dividing manually \n < ");
        for(Int_t i = 1; i <= h1->GetNbinsX(); i++) {
            binCent = h1->GetXaxis()->GetBinCenter(i);
            j = h2->FindBin(binCent);
            binWidth = h1->GetXaxis()->GetBinWidth(i);
            if(h2->GetBinContent(j) != 0) { 
                ratio = h1->GetBinContent(i)/h2->GetBinContent(j);
                //  cout << "point " << i << " ratio " << ratio << endl;
                Double_t A = h1->GetBinError(i)/h1->GetBinContent(i);
                Double_t B = h2->GetBinError(i)/h2->GetBinContent(i);
                error2 = ratio*ratio*A*A+ratio*ratio*B*B;
                if(error2 > 0 ) error2 = TMath::Sqrt(error2);
                gr->SetPoint(i-1, binCent, ratio);
                gr->SetPointError(i-1, 0.5*binWidth, error2);
            }
        }
    } else {
        printf( " > using ROOT to divide two histograms < \n");
        for(Int_t i(1); i <= h1->GetNbinsX(); i++) {
            binCent = dud->GetXaxis()->GetBinCenter(i);
            binWidth = dud->GetXaxis()->GetBinWidth(i);
            gr->SetPoint(i-1,binCent,dud->GetBinContent(i));
            gr->SetPointError(i-1, 0.5*binWidth,dud->GetBinError(i));
        }
    }
    return gr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TH2* NormalizeResponsMatrixYaxisWithPrior(TH2 *h2RM, TH1 *hPrior) {
    //
    // Normalize such that the Y projection is the prior
    //

    //  TH1D *hProjRespY = (TH1D*)h2RM->ProjectionY("hProjRespY");
    double intPrior = hPrior->Integral();//"width");
    for (Int_t jbin = 1; jbin <= h2RM->GetNbinsY(); jbin++) {
        //    double corr = hPrior->GetBinContent(jbin)/hProjRespY->GetBinContent(jbin);
        for (Int_t ibin = 1; ibin <= h2RM->GetNbinsX(); ibin++) {
            double content = h2RM->GetBinContent(ibin,jbin);
            //      h2RM->SetBinContent(ibin,jbin,content*corr);
            h2RM->SetBinContent(ibin,jbin,hPrior->GetBinContent(jbin)/hPrior->GetBinWidth(jbin)/intPrior*content);
        }
    }

    return h2RM;
}


//--------------------------------------------------------------------------------------------------------------------------------------------------
TH1D* MultiplyResponseGenerated(TH1 *hGen, TH2 *hResponse,TH1 *hEfficiency,Bool_t bDrawSlices) {

    //
    // Multiply hGen with response matrix to obtain refolded spectrum
    // Efficiency must be given. In case efficiency is 1 use SetFlatEfficiency(1.) before calling function
    //

    if(!hEfficiency) {
        //    printf("Efficiency must be given. In case efficiency is 1 use SetFlatEfficiency(1.) before calling function. Aborting!");
        //    return 0;
        printf("Setting efficiency to 1 \n");
        hEfficiency = (TH1D*)hGen->Clone("hEfficiency");
        hEfficiency->Reset();
        for(int i=1; i<=hEfficiency->GetNbinsX(); i++) hEfficiency->SetBinContent(i,1.);
    }

    //For response
    //x-axis: generated
    //y-axis: reconstructed

    TH1D *hRec = hResponse->ProjectionY("hRec");
    hRec->Sumw2();
    hRec->Reset();
    hRec->SetTitle("hRec");
    hRec->SetName("hRec");

    for(int irec=1; irec<=hRec->GetNbinsX(); irec++)
        hRec->SetBinContent(irec,0);

    TH1D *hRecGenBin = 0x0;

    Double_t yieldMC = 0.;
    Double_t yieldMCerror = 0.;
    Double_t sumYield = 0.;
    const Int_t nbinsRec = hRec->GetNbinsX()+1;
    Double_t sumError2[999] = {0.};
    Double_t eff = 0.;

    for(int igen=1; igen<=hGen->GetNbinsX(); igen++) {
        //get pTMC
        sumYield = 0.;
        eff = hEfficiency->GetBinContent(igen);
        yieldMC = hGen->GetBinContent(igen)*eff;
        yieldMCerror = hGen->GetBinError(igen)*eff;

        if(bDrawSlices) {
            hRecGenBin = hResponse->ProjectionY(Form("hRecGenBin%d",igen));
            hRecGenBin->Sumw2();
            hRecGenBin->Reset();
            hRecGenBin->SetTitle(Form("hRecGenBin%d",igen));
            hRecGenBin->SetName(Form("hRecGenBin%d",igen));
        }

        for(int irec=1; irec<=hRec->GetNbinsX(); irec++) {
            hRec->AddBinContent(irec,yieldMC*hResponse->GetBinContent(igen,irec));
            sumYield+=hResponse->GetBinContent(igen,irec);
            //      Double_t A = yieldMC*hResponse->GetBinError(igen,irec);
            Double_t B = hResponse->GetBinContent(igen,irec)*yieldMCerror;
            //      Double_t tmp2 = A*A + B*B;
            //sumError2[irec-1] += tmp2 ;
            sumError2[irec-1] += B*B;

            if(bDrawSlices)
                hRecGenBin->SetBinContent(irec,yieldMC*hResponse->GetBinContent(igen,irec));

        }
        if(bDrawSlices) {
            cSlices->cd();
            hRecGenBin->SetLineColor(igen);
            if(igen==1) hRecGenBin->DrawCopy();      
            else hRecGenBin->DrawCopy("same");
        }

        if(hRecGenBin) delete hRecGenBin;

        cout << "igen: " << igen << "\tpTMC: " << hGen->GetXaxis()->GetBinCenter(igen) << "\teff:" << eff << "\tsumYield: " << sumYield << endl;
    }

    for(int i=0; i<nbinsRec; i++) {
        if(sumError2[i]>0.)
            hRec->SetBinError(i+1,TMath::Sqrt(sumError2[i]));
    }


    return hRec;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TH2* GetTransposeResponsMatrix(TH2 *h2RM) {
    //
    // Transpose response matrix
    //

    Printf("AliAnaChargedJetResponseMaker::GetTransposeResponsMatrix");

    //Initialize transposed response matrix h2RMTranspose
    // TArrayD *arrayX = (TArrayD*)h2RM->GetXaxis()->GetXbins();
    // for(int i=0; i<arrayX->GetSize(); i++) Printf("i: %d  %f", i,arrayX->At(i));
    // Double_t *xbinsArray = arrayX->GetArray();

    // TArrayD *arrayY = (TArrayD*)h2RM->GetYaxis()->GetXbins();
    // for(int i=0; i<arrayY->GetSize(); i++) Printf("i: %d  %f", i,arrayY->At(i));
    // Double_t *ybinsArray = arrayY->GetArray();


    Double_t *ybinsArray = new Double_t[h2RM->GetNbinsY()+1];
    for(int i=1; i<=h2RM->GetNbinsY(); i++) {
        ybinsArray[i-1] = h2RM->GetYaxis()->GetBinLowEdge(i);
        Printf("i=%d  %f   %f",i,ybinsArray[i-1],h2RM->GetYaxis()->GetBinLowEdge(i));
    }
    ybinsArray[h2RM->GetNbinsY()] = h2RM->GetYaxis()->GetBinUpEdge(h2RM->GetNbinsY());

    Double_t *xbinsArray = new Double_t[h2RM->GetNbinsX()+1];
    for(int i=1; i<=h2RM->GetNbinsX(); i++) 
        xbinsArray[i-1] = h2RM->GetXaxis()->GetBinLowEdge(i);
    xbinsArray[h2RM->GetNbinsX()] = h2RM->GetXaxis()->GetBinUpEdge(h2RM->GetNbinsX());


    TH2D *h2RMTranspose = new TH2D("h2RMTranspose","h2RMTranspose",h2RM->GetNbinsY(),ybinsArray,h2RM->GetNbinsX(),xbinsArray);

    //Fill tranposed response matrix
    for (Int_t ibin = 1; ibin <= h2RMTranspose->GetNbinsX(); ibin++) {
        for (Int_t jbin = 1; jbin <= h2RMTranspose->GetNbinsY(); jbin++) {
            h2RMTranspose->SetBinContent(ibin,jbin, h2RM->GetBinContent(jbin,ibin));
        }
    }


    return h2RMTranspose;

}

//_____________________________________________________________________________
TH2D* NormalizeTH2D(TH2D* histo) {
    // general method to normalize all vertical slices of a th2 to unity
    // i.e. get a probability matrix
    if(!histo) {
        printf(" > NormalizeTH2D:: NULL pointer passed, returning NULL < \n");
        return NULL;
    }
    Int_t binsX = histo->GetXaxis()->GetNbins();
    Int_t binsY = histo->GetYaxis()->GetNbins();

    // normalize all slices in x
    for(Int_t i(0); i < binsX; i++) {   // for each vertical slice
        Double_t weight = 0;
        for(Int_t j(0); j < binsY; j++) {       // loop over all the horizontal components
            weight+=histo->GetBinContent(i+1, j+1);
        }       // now we know the total weight
        for(Int_t j(0); j < binsY; j++) {
            if (weight <= 0 ) continue;
            histo->SetBinContent(1+i, j+1, histo->GetBinContent(1+i, j+1)/weight);
            //            if(noError) histo->SetBinError(  1+i, j+1, 0.);
            //            else 
            histo->SetBinError(  1+i, j+1, histo->GetBinError(  1+i, j+1)/weight);
        }
    }
    return histo;
}





void unfoldBayesSimple(int itmin = 1, int itmax = 20)
{
#ifdef __CINT__
    gSystem->Load("/home/rbertens/Documents/CERN/jet-flow/results/2017/UNFOLDING/RooUnfold/libRooUnfold");
#endif
    cout << "==================================== TRAIN SAMPLE ==============================" << endl;
    //    RooUnfoldResponse response (100, -30, 70);
    TArrayD* trueBins = new TArrayD(sizeof(binsTrue)/sizeof(binsTrue[0]), binsTrue);
    TArrayD* recBins = new TArrayD(sizeof(binsRec)/sizeof(binsRec[0]), binsRec);



    RooUnfoldResponse* response = 0x0;//(16, 0, 80);
    TFile* preCookedResponse = TFile::Open("RM.root");
    if(!preCookedResponse || preCookedResponse->IsZombie()) {
        cout << "    no RM found - cooking one up " << endl; 
        TFile *f = new TFile("/home/rbertens/Documents/CERN/jet-flow/results/2018/responses/R02/0_5/TrainEmbedding.root");
        TTree *t1 = (TTree*)f->Get("fTreeJetShape_MC_Merged");
        Float_t ptJet, ptJetMatch;
        Double_t weightPythiaFromPtHard;
        Int_t ev;
        t1->SetBranchAddress("ptJet",&ptJet);
        t1->SetBranchAddress("ptJetMatch",&ptJetMatch);
        t1->SetBranchAddress("weightPythiaFromPtHard", &weightPythiaFromPtHard);

        response = new RooUnfoldResponse(
                new TH1D("measured", "measured", recBins->GetSize()-1, recBins->GetArray()),
                new TH1D("truth", "truth", trueBins->GetSize()-1, trueBins->GetArray()),
                new TH2D("response", "response", recBins->GetSize()-1, recBins->GetArray(),trueBins->GetSize()-1, trueBins->GetArray()));
        //read all entries and fill the histograms
        Long64_t nentries = t1->GetEntries();
        for (Long64_t i=0;i<nentries;i++) {
            t1->GetEntry(i);
            response->Fill(ptJet,ptJetMatch,weightPythiaFromPtHard);
        }
        RooUnfoldResponse* responseCL = (RooUnfoldResponse*)response->Clone("responseCL");
        responseCL->SetNameTitle("response", "response");
        preCookedResponse = new TFile("RM.root", "RECREATE");
        preCookedResponse->ls();
        responseCL->Write();
        preCookedResponse->Close();
    } else {
        cout << "    using precooked response " << endl;
        response = (RooUnfoldResponse*)preCookedResponse->Get("response");
    }


    cout << "==================================== GET JETS ==================================" << endl;

    int r = 2;


    TFile meJets("/home/rbertens/Documents/CERN/jet-flow/results/2018/JUNE/ME/0_5/ME_jets_0_5.root");
    TFile seJets("/home/rbertens/Documents/CERN/jet-flow/results/2018/JUNE/SE/0_5/SE_jets_0_5.root");

    TH1D* meHistJets = (TH1D*)meJets.Get(Form("fHistJetPtSubtracted__R%i", r));
    TH1D* seHistJets = (TH1D*)seJets.Get(Form("fHistJetPtSubtracted__R%i", r));

    TH1D* meHistCent = (TH1D*)meJets.Get(Form("fHistCentrality__R%i", r));
    //    Int_t meEvents = meHistCent->GetEntries();
    Int_t meEvents = meHistJets->GetBinContent(132);
    TH1D* seHistCent = (TH1D*)seJets.Get(Form("fHistCentrality__R%i", r));
    //    Int_t seEvents = seHistCent->GetEntries();
    Int_t seEvents = seHistJets->GetBinContent(132);    

    //seHistJets->Scale(1./double(seEvents), "width");
    meHistJets->Scale(double(seEvents)/double(meEvents), "width");

    TH1D* seHistJetsClone = (TH1D*)seHistJets->Clone("same event jets");
    TH1D* meHistJetsClone = (TH1D*)meHistJets->Clone("mixed event jets");

    TH1D* seHistJetsClone2 = (TH1D*)seHistJets->Clone("same event jets 2");
    seHistJetsClone2->Divide(meHistJetsClone);





    meHistJets->SetTitle("ME jets");
    meHistJets->GetYaxis()->SetTitle("#frac{1}{N_{evt}}#frac{d#it{N}}{d#it{p}_{T}} (GeV/#it{c})^{-1}");
    meHistJets->SetLineColor(kRed);
    meHistJets->SetMarkerColor(kRed);

    seHistJets->SetTitle("SE jets");

    meHistJets->DrawCopy();
    seHistJets->DrawCopy("same");


    seHistJets->Add(meHistJets, -1);


    TH1D* seHistJetsDifference = seHistJets->Clone("difference");


    cout << "==================================== UNFOLD ===================================" << endl;

    TFile of(Form("Bayes_unfolded_DL%i-%i_PL%i-%i_KREG%i.root", (int)recBins->At(0), (int)recBins->At(recBins->GetSize()-1), (int)trueBins->At(0), (int)trueBins->At(trueBins->GetSize()-1)), "RECREATE"); 

    seHistJets = RebinTH1D(seHistJets, recBins);

    TH1D *unfoldedSVD = (TH1D*)seHistJets->Clone("unfoldedSVD");
    unfoldedSVD->Reset();
    TH1D *foldedSVD(0x0);
    RooUnfold::ErrorTreatment errorTreatment = RooUnfold::kCovToy; // RooUnfold::kCovariance;

    for(int iIterations = itmin; iIterations < itmax; iIterations++) {
        RooUnfoldBayes unfold(response, seHistJets, iIterations);
        unfoldedSVD = (TH1D*)unfold.Hreco();
        unfold.PrintTable(cout, unfoldedSVD);
        unfoldedSVD->SetNameTitle(Form("UnfoldedSpectrum_%i", iIterations),Form("unfolded spectrum it %i", iIterations));
        unfoldedSVD->Write(); 
 

        // get the pearson coefficients from the covariance matrix
        TMatrixD covarianceMatrix = unfold.Ereco(errorTreatment);
        TMatrixD *pearson = (TMatrixD*)CalculatePearsonCoefficients(&covarianceMatrix);
        TH2D* hPearson(0x0);
        if(pearson) {
            hPearson = new TH2D(*pearson);
            pearson->Print();
            hPearson->SetNameTitle(Form("PearsonCoefficients_%i", iIterations), Form("Pearson coefficients it %i", iIterations));
            hPearson->Write();
        }
    }
    /*
    // plot singular values and d_i vector
    TSVDUnfold_local* svdUnfold = unfoldSVD.Impl();
    TH1* hSVal = svdUnfold->GetSV();
    TH1D* hdi = svdUnfold->GetD();
    hSVal->SetNameTitle("SingularValuesOfAC", "Singular values of AC^{-1}");
    hSVal->SetXTitle("singular values");
    hSVal->Write();
    hdi->SetNameTitle("dVector", "d vector after orthogonal transformation");
    hdi->SetXTitle("|d_{i}^{kreg}|");
    hdi->Write();
    */
    // 7) refold the unfolded spectrum
    //    TH1D* foldedSVD = MultiplyResponseGenerated(unfoldedSVD, (TH2D*)response, kinematicEfficiency, kFALSE);
    /*
       TGraphErrors* ratio = GetRatio(seHistJetsClone, foldedSVD);
       ratio->SetNameTitle("RatioRefoldedMeasured", "Ratio measured / re-folded");
       ratio->GetXaxis()->SetTitle("p_{t}^{rec, rec} [GeV/ c]");
       ratio->GetYaxis()->SetTitle("ratio measured / re-folded");
       ratio->Write();
       */
    // write the measured, unfolded and re-folded spectra to the output directory
    seHistJets->SetNameTitle("InputSpectrum", "input spectrum (measured)");
    seHistJets->SetXTitle("p_{t}^{rec} [GeV/c]");
    seHistJets->Write(); // input spectrum

   seHistJetsClone->Write();
    meHistJetsClone->Write();
    seHistJetsDifference->Write();
    seHistJetsClone2->Write();


    of.Close();




}

void unfoldSVD(int itmin, int itmax)
{
#ifdef __CINT__
    gSystem->Load("/home/rbertens/Documents/CERN/jet-flow/results/2017/UNFOLDING/RooUnfold/libRooUnfold");
#endif






    TArrayD* trueBins = new TArrayD(sizeof(binsTrue)/sizeof(binsTrue[0]), binsTrue);
    TArrayD* recBins = new TArrayD(sizeof(binsRec)/sizeof(binsRec[0]), binsRec);



    RooUnfoldResponse* response = 0x0;//(16, 0, 80);
    TFile* preCookedResponse = TFile::Open("RM.root");
    if(!preCookedResponse || preCookedResponse->IsZombie()) {
        cout << "    no RM found - cooking one up " << endl; 
        TFile *f = new TFile("/home/rbertens/Documents/CERN/jet-flow/results/2018/responses/R02/0_5/TrainEmbedding.root");
        TTree *t1 = (TTree*)f->Get("fTreeJetShape_MC_Merged");
        Float_t ptJet, ptJetMatch;
        Double_t weightPythiaFromPtHard;
        Int_t ev;
        t1->SetBranchAddress("ptJet",&ptJet);
        t1->SetBranchAddress("ptJetMatch",&ptJetMatch);
        t1->SetBranchAddress("weightPythiaFromPtHard", &weightPythiaFromPtHard);

        response = new RooUnfoldResponse(
                new TH1D("measured", "measured", recBins->GetSize()-1, recBins->GetArray()),
                new TH1D("truth", "truth", trueBins->GetSize()-1, trueBins->GetArray()),
                new TH2D("response", "response", recBins->GetSize()-1, recBins->GetArray(),trueBins->GetSize()-1, trueBins->GetArray()));
        //read all entries and fill the histograms
        Long64_t nentries = t1->GetEntries();
        for (Long64_t i=0;i<nentries;i++) {
            t1->GetEntry(i);
            response->Fill(ptJet,ptJetMatch,weightPythiaFromPtHard);
        }
        RooUnfoldResponse* responseCL = (RooUnfoldResponse*)response->Clone("responseCL");
        responseCL->SetNameTitle("response", "response");
        preCookedResponse = new TFile("RM.root", "RECREATE");
        preCookedResponse->ls();
        responseCL->Write();
        preCookedResponse->Close();
    } else {
        cout << "    using precooked response " << endl;
        response = (RooUnfoldResponse*)preCookedResponse->Get("response");
    }


    cout << "==================================== GET JETS ==================================" << endl;

    int r = 2;


    TFile meJets("/home/rbertens/Documents/CERN/jet-flow/results/2018/JUNE/ME/0_5/ME_jets_0_5.root");
    TFile seJets("/home/rbertens/Documents/CERN/jet-flow/results/2018/JUNE/SE/0_5/SE_jets_0_5.root");

    TH1D* meHistJets = (TH1D*)meJets.Get(Form("fHistJetPtSubtracted__R%i", r));
    TH1D* seHistJets = (TH1D*)seJets.Get(Form("fHistJetPtSubtracted__R%i", r));

    TH1D* meHistCent = (TH1D*)meJets.Get(Form("fHistCentrality__R%i", r));
    //    Int_t meEvents = meHistCent->GetEntries();
    Int_t meEvents = meHistJets->GetBinContent(132);
    TH1D* seHistCent = (TH1D*)seJets.Get(Form("fHistCentrality__R%i", r));
    //    Int_t seEvents = seHistCent->GetEntries();
    Int_t seEvents = seHistJets->GetBinContent(132);    

    //seHistJets->Scale(1./double(seEvents), "width");
    meHistJets->Scale(double(seEvents)/double(meEvents), "width");

    TH1D* seHistJetsClone = (TH1D*)seHistJets->Clone("same event jets");
    TH1D* meHistJetsClone = (TH1D*)meHistJets->Clone("mixed event jets");

    TH1D* seHistJetsClone2 = (TH1D*)seHistJets->Clone("same event jets 2");
    seHistJetsClone2->Divide(meHistJetsClone);





    meHistJets->SetTitle("ME jets");
    meHistJets->GetYaxis()->SetTitle("#frac{1}{N_{evt}}#frac{d#it{N}}{d#it{p}_{T}} (GeV/#it{c})^{-1}");
    meHistJets->SetLineColor(kRed);
    meHistJets->SetMarkerColor(kRed);

    seHistJets->SetTitle("SE jets");

    meHistJets->DrawCopy();
    seHistJets->DrawCopy("same");


    seHistJets->Add(meHistJets, -1);


    TH1D* seHistJetsDifference = seHistJets->Clone("difference");


    cout << "==================================== UNFOLD ===================================" << endl;

    TFile of(Form("SVD_unfolded_DL%i-%i_PL%i-%i_KREG%i.root", (int)recBins->At(0), (int)recBins->At(recBins->GetSize()-1), (int)trueBins->At(0), (int)trueBins->At(trueBins->GetSize()-1)), "RECREATE"); 

    seHistJets = RebinTH1D(seHistJets, recBins);

    TH1D *unfoldedSVD = (TH1D*)seHistJets->Clone("unfoldedSVD");
    unfoldedSVD->Reset();
    TH1D *foldedSVD(0x0);
    RooUnfold::ErrorTreatment errorTreatment = RooUnfold::kCovToy; // RooUnfold::kCovariance;

    for(int iIterations = itmin; iIterations < itmax; iIterations++) {
        RooUnfoldSvd unfold(response, seHistJets, iIterations);
        unfoldedSVD = (TH1D*)unfold.Hreco();
        unfold.PrintTable(cout, unfoldedSVD);
        unfoldedSVD->SetNameTitle(Form("UnfoldedSpectrum_%i", iIterations),Form("unfolded spectrum it %i", iIterations));
        unfoldedSVD->Write(); 
 

        // get the pearson coefficients from the covariance matrix
        TMatrixD covarianceMatrix = unfold.Ereco(errorTreatment);
        TMatrixD *pearson = (TMatrixD*)CalculatePearsonCoefficients(&covarianceMatrix);
        TH2D* hPearson(0x0);
        if(pearson) {
            hPearson = new TH2D(*pearson);
            pearson->Print();
            hPearson->SetNameTitle(Form("PearsonCoefficients_%i", iIterations), Form("Pearson coefficients it %i", iIterations));
            hPearson->Write();
        }
    }

    // plot singular values and d_i vector
    TSVDUnfold_local* svdUnfold = unfold.Impl();
    TH1* hSVal = svdUnfold->GetSV();
    TH1D* hdi = svdUnfold->GetD();
    hSVal->SetNameTitle("SingularValuesOfAC", "Singular values of AC^{-1}");
    hSVal->SetXTitle("singular values");
    hSVal->Write();
    hdi->SetNameTitle("dVector", "d vector after orthogonal transformation");
    hdi->SetXTitle("|d_{i}^{kreg}|");
    hdi->Write();

    seHistJets->SetNameTitle("InputSpectrum", "input spectrum (measured)");
    seHistJets->SetXTitle("p_{t}^{rec} [GeV/c]");
    seHistJets->Write(); // input spectrum

    seHistJetsClone->Write();
    meHistJetsClone->Write();
    seHistJetsDifference->Write();
    seHistJetsClone2->Write();


    of.Close();

    ///////////////////////////////

}







void unfoldLegacy(int regularization = 4)
{
#ifdef __CINT__
    gSystem->Load("/home/rbertens/Documents/CERN/jet-flow/results/2017/UNFOLDING/RooUnfold/libRooUnfold");
#endif

    cout << "==================================== TRAIN SAMPLE ==============================" << endl;
    //    RooUnfoldResponse response (100, -30, 70);

    TH2D* response = 0x0;
    TFile* preCookedResponse = TFile::Open("RM.root");
    if(!preCookedResponse || preCookedResponse->IsZombie()) {
        cout << "    no RM found - cooking one up " << endl; 
        TFile *f = new TFile("/home/rbertens/Documents/CERN/jet-flow/results/2018/responses/R02/0_5/TrainEmbedding.root");
        TTree *t1 = (TTree*)f->Get("fTreeJetShape_MC_Merged");
        Float_t ptJet, ptJetMatch;
        Double_t weightPythiaFromPtHard;
        Int_t ev;
        t1->SetBranchAddress("ptJet",&ptJet);
        t1->SetBranchAddress("ptJetMatch",&ptJetMatch);
        t1->SetBranchAddress("weightPythiaFromPtHard", &weightPythiaFromPtHard);

        response = new TH2D("ptJet", "ptJetMatch", 1000, -300, 700, 1000, -300, 700);
        //read all entries and fill the histograms
        Long64_t nentries = t1->GetEntries();
        for (Long64_t i=0;i<nentries;i++) {
            t1->GetEntry(i);
            response->Fill(ptJetMatch,ptJet,weightPythiaFromPtHard);
        }
        TH2D* responseCL = (TH2D*)response->Clone("responseCL");
        responseCL->SetNameTitle("response", "response");
        preCookedResponse = new TFile("RM.root", "RECREATE");
        preCookedResponse->ls();
        responseCL->Write();
        preCookedResponse->Close();
    } else {
        cout << "    using precooked response " << endl;
        response = (TH2D*)preCookedResponse->Get("response");
    }



    cout << "==================================== GET JETS ==================================" << endl;

    int r = 2;


    TFile meJets("/home/rbertens/Documents/CERN/jet-flow/results/2018/JUNE/ME/0_5/ME_jets_0_5.root");
    TFile seJets("/home/rbertens/Documents/CERN/jet-flow/results/2018/JUNE/SE/0_5/SE_jets_0_5.root");

    TH1D* meHistJets = (TH1D*)meJets.Get(Form("fHistJetPtSubtracted__R%i", r));
    TH1D* seHistJets = (TH1D*)seJets.Get(Form("fHistJetPtSubtracted__R%i", r));

    TH1D* meHistCent = (TH1D*)meJets.Get(Form("fHistCentrality__R%i", r));
    //    Int_t meEvents = meHistCent->GetEntries();
    Int_t meEvents = meHistJets->GetBinContent(132);
    TH1D* seHistCent = (TH1D*)seJets.Get(Form("fHistCentrality__R%i", r));
    //    Int_t seEvents = seHistCent->GetEntries();
    Int_t seEvents = seHistJets->GetBinContent(132);    

    //seHistJets->Scale(1./double(seEvents), "width");
    meHistJets->Scale(double(seEvents)/double(meEvents), "width");

    TH1D* seHistJetsClone = (TH1D*)seHistJets->Clone("same event jets");
    TH1D* meHistJetsClone = (TH1D*)meHistJets->Clone("mixed event jets");

    TH1D* seHistJetsClone2 = (TH1D*)seHistJets->Clone("same event jets 2");
    seHistJetsClone2->Divide(meHistJetsClone);





    meHistJets->SetTitle("ME jets");
    meHistJets->GetYaxis()->SetTitle("#frac{1}{N_{evt}}#frac{d#it{N}}{d#it{p}_{T}} (GeV/#it{c})^{-1}");
    meHistJets->SetLineColor(kRed);
    meHistJets->SetMarkerColor(kRed);

    seHistJets->SetTitle("SE jets");

    meHistJets->DrawCopy();
    seHistJets->DrawCopy("same");


    seHistJets->Add(meHistJets, -1);


    TH1D* seHistJetsDifference = seHistJets->Clone("difference");


    cout << "==================================== UNFOLD ===================================" << endl;

    TString drInputName = "/home/rbertens/Documents/CERN/jet-flow/response/temp_task_bookkeeping/3gev_5gev_plus_reduced_eff/3gev_R02/pythia_frag_merge.root";
    printf("- Reading file %s ... \n", drInputName.Data());
    TFile drInput(drInputName.Data());          // detector response input matrix
    if(drInput.IsZombie()) {
        printf(" > read error ! < \n");
        return;
    }
    TList* responseList = (TList*)drInput.Get("detector_response_R02_histos");
    TH2D* detres = (TH2D*)responseList->FindObject("fHistDetectorResponse");
    TH1D* prior = (TH1D*)seHistJets->Clone("prior_from_data");    //detres->ProjectionX();
    TArrayD* trueBins = new TArrayD(sizeof(binsTrue)/sizeof(binsTrue[0]), binsTrue);
    TArrayD* recBins = new TArrayD(sizeof(binsRec)/sizeof(binsRec[0]), binsRec);



    TFile of(Form("unfolded_DL%i-%i_PL%i-%i_KREG%i.root", (int)recBins->At(0), (int)recBins->At(recBins->GetSize()-1), (int)trueBins->At(0), (int)trueBins->At(trueBins->GetSize()-1), regularization), "RECREATE"); 
    /*
       cout << (int)recBins->At(0) << endl;
       cout << (int)recBins->At(recBins->GetSize()-1) << endl;
       cout << (int)trueBins->At(0) <<endl;
       cout << (int)trueBins->At(trueBins->GetSize()-1) << endl;
       cout <<regularization << endl;
       */
    TH2D* resizedResponse = new TH2D("ptjet", "ptJetMatch", trueBins->GetSize()-1, trueBins->GetArray(), recBins->GetSize()-1, recBins->GetArray());
    TH2D* resizedEffResponse = new TH2D("ptjet", "ptJetMatch", trueBins->GetSize()-1, trueBins->GetArray(), recBins->GetSize()-1, recBins->GetArray());
    seHistJets = RebinTH1D(seHistJets, recBins);
    prior = RebinTH1D(prior, trueBins);

    // routine for the kinematic efficiency
    TH2D* effResponse = (TH2D*)response->Clone("effResponseClone");
    TH2D* responseNorm = NormalizeTH2D(effResponse);
    resizedEffResponse = (TH2D*)RebinTH2D(responseNorm, resizedEffResponse);
    TH1D* kinematicEfficiency = resizedEffResponse->ProjectionX();

    for(Int_t iB = 0; iB < kinematicEfficiency->GetNbinsX(); iB++) {
        kinematicEfficiency->SetBinError(iB, 0);
    }



    // prepare response matrix: all slides in truth normalized to 1
    resizedResponse = (TH2D*)RebinTH2D(response, resizedResponse);
    //    resizedResponse = NormalizeTH2D(resizedResponse);//FIXME


    //    resizedResponse = response;




    TH1D *unfoldedSVD(0x0)
        TH1D *foldedSVD(0x0);
    RooUnfold::ErrorTreatment errorTreatment = RooUnfold::kCovToy; // RooUnfold::kCovariance;



    // REMMEBER REMEMBER REMEMBER
    // RooUnfold expects Fill(measured, true), that's why you transpose your matrix here
    //
    TH2* responseMatrixTransposePrior = resizedResponse;
    //GetTransposeResponsMatrix(resizedResponse);
    //    responseMatrixTransposePrior = NormalizeResponsMatrixYaxisWithPrior(responseMatrixTransposePrior, prior);//FIXME
    RooUnfoldResponse responseSVD(0, 0, responseMatrixTransposePrior);

    // note to self: if you introduce the prior, it is multplied with the transposed, hence the trickery above
    //    RooUnfoldResponse responseSVD(0, 0, resizedResponse);

    //    RooUnfoldSvd unfoldSVD(&responseSVD, seHistJets, regularization);// FIXME
    //    unfoldedSVD = (TH1D*)unfoldSVD.Hreco(errorTreatment);//FIXME


    RooUnfoldResponse responseBayes(0, 0, responseMatrixTransposePrior);
    RooUnfoldBayes unfoldSVD(&responseBayes, seHistJets, regularization);
    unfoldedSVD = (TH1D*)unfoldSVD.Hreco();


    unfoldedSVD->Divide(kinematicEfficiency);




    // get the pearson coefficients from the covariance matrix
    TMatrixD covarianceMatrix = unfoldSVD.Ereco(errorTreatment);
    TMatrixD *pearson = (TMatrixD*)CalculatePearsonCoefficients(&covarianceMatrix);
    TH2D* hPearson(0x0);
    if(pearson) {
        hPearson = new TH2D(*pearson);
        pearson->Print();
        hPearson->SetNameTitle("PearsonCoefficients", "Pearson coefficients");
        hPearson->Write();
    }
    /*
    // plot singular values and d_i vector
    TSVDUnfold_local* svdUnfold = unfoldSVD.Impl();
    TH1* hSVal = svdUnfold->GetSV();
    TH1D* hdi = svdUnfold->GetD();
    hSVal->SetNameTitle("SingularValuesOfAC", "Singular values of AC^{-1}");
    hSVal->SetXTitle("singular values");
    hSVal->Write();
    hdi->SetNameTitle("dVector", "d vector after orthogonal transformation");
    hdi->SetXTitle("|d_{i}^{kreg}|");
    hdi->Write();
    */
    // 7) refold the unfolded spectrum
    TH1D* foldedSVD = MultiplyResponseGenerated(unfoldedSVD, resizedResponse, kinematicEfficiency, kFALSE);
    /*
       TGraphErrors* ratio = GetRatio(seHistJetsClone, foldedSVD);
       ratio->SetNameTitle("RatioRefoldedMeasured", "Ratio measured / re-folded");
       ratio->GetXaxis()->SetTitle("p_{t}^{rec, rec} [GeV/ c]");
       ratio->GetYaxis()->SetTitle("ratio measured / re-folded");
       ratio->Write();
       */
    // write the measured, unfolded and re-folded spectra to the output directory
    seHistJets->SetNameTitle("InputSpectrum", "input spectrum (measured)");
    seHistJets->SetXTitle("p_{t}^{rec} [GeV/c]");
    seHistJets->Write(); // input spectrum
    unfoldedSVD->SetNameTitle("UnfoldedSpectrum","unfolded spectrum");
    TH1D* unfoldedSVDScaled = (TH1D*)unfoldedSVD->Clone("unfolded, scaled");
    unfoldedSVDScaled->Scale(1./double(seEvents));
    unfoldedSVD->Write(); 
    unfoldedSVDScaled->Write();
    foldedSVD->SetNameTitle("RefoldedSpectrum", "refoldedSpectrum");
    foldedSVD->Write();    // re-folded spectrum

    // save more general bookkeeeping histograms to the output directory
    responseMatrixTransposePrior->SetNameTitle("TransposeResponseMatrix", "Transpose of response matrix, normalize with prior");
    responseMatrixTransposePrior->SetXTitle("p_{T, jet}^{true} [GeV/c]");
    responseMatrixTransposePrior->SetYTitle("p_{T, jet}^{rec} [GeV/c]");
    responseMatrixTransposePrior->Write();
    prior->SetNameTitle("PriorOriginal", "Prior, original");
    prior->SetXTitle("p_{t} [GeV/c]");
    prior->Write();

    resizedResponse->Write();
    response->Write();
    kinematicEfficiency->Write();

    seHistJetsClone->Write();
    meHistJetsClone->Write();
    seHistJetsDifference->Write();
    seHistJetsClone2->Write();


    of.Close();





    ///////////////////////////////

}



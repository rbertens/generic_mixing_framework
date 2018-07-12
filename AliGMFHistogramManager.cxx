// Redmer Alexander Bertens, 2017 (UTK, CERN)


#include "TH1D.h"
#include "TH2D.h"
#include "TH3F.h"
#include "THashList.h"
#include "TFile.h"
#include "TH1.h"

#include "AliGMFHistogramManager.h"

ClassImp(AliGMFHistogramManager)

using namespace std;

//_____________________________________________________________________________
AliGMFHistogramManager::AliGMFHistogramManager(TString name) : TObject(),
    fOutputList(0x0),
    fManagerName("") {
    // default constructor
    fOutputList = new THashList();
    fOutputList->SetOwner(kTRUE);
    fManagerName += name;

}

   //_____________________________________________________________________________
TH1D* AliGMFHistogramManager::BookTH1D(const char* name, const char* x, Int_t bins, Double_t min, Double_t max, Bool_t append)
{
    // book a TH1D and connect it to the output container
    if(append && !fOutputList) return 0x0;
    TString title(name);
    title += Form(";%s;[counts]", x);
    TH1D* histogram = new TH1D(Form("%s_%s", name, fManagerName.Data()), title.Data(), bins, min, max);
    histogram->Sumw2();
    if(append) fOutputList->Add(histogram);
    return histogram;   
}
//_____________________________________________________________________________
TH2D* AliGMFHistogramManager::BookTH2D(const char* name, const char* x, const char* y, Int_t binsx, Double_t minx, Double_t maxx, Int_t binsy, Double_t miny, Double_t maxy, Bool_t append)
{
    // book a TH2D and connect it to the output container
    if(append && !fOutputList) return 0x0;
    TString title(name);
    title += Form(";%s;%s", x, y);
    TH2D* histogram = new TH2D(Form("%s_%s", name, fManagerName.Data()), title.Data(), binsx, minx, maxx, binsy, miny, maxy);
    histogram->Sumw2();
    if(append) fOutputList->Add(histogram);
    return histogram;   
}
//_____________________________________________________________________________
TH3F* AliGMFHistogramManager::BookTH3F(const char* name, const char* x, const char* y, const char* z, Int_t binsx, Double_t minx, Double_t maxx, Int_t binsy, Double_t miny, Double_t maxy, Int_t binsz, Double_t minz, Double_t maxz, Bool_t append)
{
    // book a TH2D and connect it to the output container
    if(append && !fOutputList) return 0x0;
    TString title(name);
    title += Form(";%s;%s;%s", x, y, z);
    TH3F* histogram = new TH3F(Form("%s_%s", name, fManagerName.Data()), title.Data(), binsx, minx, maxx, binsy, miny, maxy, binsz, minz, maxz);
    histogram->Sumw2();
    if(append) fOutputList->Add(histogram);
    return histogram;   
}
//_____________________________________________________________________________
TH1* AliGMFHistogramManager::GetHistogram(const TString &name) {
    // return a pointer to a histogram named 'name'
    return static_cast<TH1*>(fOutputList->FindObject(Form("%s_%s", name.Data(), fManagerName.Data())));
}
//_____________________________________________________________________________
Bool_t AliGMFHistogramManager::StoreRatio(const TString &a, const TString &b, const TString &name) {
    // take the ratio of histograms a and b, and add them to the manager
    TH1* _ha = static_cast<TH1*>(GetHistogram(a));
    TH1* _hb = static_cast<TH1*>(GetHistogram(b));
    TH1* clone = static_cast<TH1*>(_ha->Clone(name.Data()));
    if(clone && _hb) {
        clone->Divide(_hb);
        fOutputList->Add(clone);
        return kTRUE;
    } else return kFALSE; 
}
//_____________________________________________________________________________
void AliGMFHistogramManager::StoreManager(TFile* of) {
    // create output file and write the manager
    of->cd();
    fOutputList->Write();
}
//____________________________________________________________________________
void AliGMFHistogramManager::StoreManager(const char* title) {
    // create output file and write the manager
     TFile* of(new TFile(title, "RECREATE"));
     fOutputList->Write();
     of->Close();
}
//_____________________________________________________________________________
Bool_t  AliGMFHistogramManager::Fill(const TString &name, const Double_t &valx) {
    // find and fill histogram
    TH1* _temp = static_cast<TH1*>(GetHistogram(name));
    if (_temp) {
        _temp->Fill(valx);
        return kTRUE;
    }
    return kFALSE;
}
//_____________________________________________________________________________
Bool_t  AliGMFHistogramManager::Fill(const TString &name, const Double_t &valx, const Double_t &valy) {
    // find and fill histogram
    TH2* _temp = static_cast<TH2*>(GetHistogram(name));
    if (_temp) {
        _temp->Fill(valx, valy);
        return kTRUE;
    }
    return kFALSE;
}
//_____________________________________________________________________________
void AliGMFHistogramManager::WeighAllHistograms(const Double_t weight) {
    // weigh all histograms in this manager by a constant factor
    for(Int_t i = 0; i < fOutputList->GetSize(); i++) {
        TH1* _temp = static_cast<TH1*>(fOutputList->At(i));
        if(_temp) _temp->Scale(weight);
    }
}
//_____________________________________________________________________________

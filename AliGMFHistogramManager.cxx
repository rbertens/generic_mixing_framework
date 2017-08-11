// Redmer Alexander Bertens, 2017 (UTK, CERN)


#include "TH1D.h"
#include "TH2D.h"
#include "TH3F.h"
#include "TList.h"
#include "TFile.h"
#include "TObject.h"

#include "AliGMFHistogramManager.h"

ClassImp(AliGMFHistogramManager)

using namespace std;

//_____________________________________________________________________________
AliGMFHistogramManager::AliGMFHistogramManager() : TObject(),
    fOutputList(0x0) {
    // default constructor
    fOutputList = new TList();
    fOutputList->SetOwner(kTRUE);
}

   //_____________________________________________________________________________
TH1D* AliGMFHistogramManager::BookTH1D(const char* name, const char* x, Int_t bins, Double_t min, Double_t max, Bool_t append)
{
    // book a TH1D and connect it to the output container
    if(append && !fOutputList) return 0x0;
    TString title(name);
    title += Form(";%s;[counts]", x);
    TH1D* histogram = new TH1D(name, title.Data(), bins, min, max);
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
    TH2D* histogram = new TH2D(name, title.Data(), binsx, minx, maxx, binsy, miny, maxy);
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
    TH3F* histogram = new TH3F(name, title.Data(), binsx, minx, maxx, binsy, miny, maxy, binsz, minz, maxz);
    histogram->Sumw2();
    if(append) fOutputList->Add(histogram);
    return histogram;   
}
//_____________________________________________________________________________
TObject* AliGMFHistogramManager::GetHistogram(TString name) {
    // return a pointer to a histogram named 'name'
    return (dynamic_cast<TH1*>(fOutputList->FindObject(name.Data())));
}

//_____________________________________________________________________________
void AliGMFHistogramManager::StoreManager(TString title) {
    // create output file and write the manager
    TFile* of(new TFile(title.Data(), "RECREATE"));
    fOutputList->Write();
    of->Close();
}
//_____________________________________________________________________________
Bool_t  AliGMFHistogramManager::Fill(TString name, Double_t valx) {
    // find and fill histogram
    TH1* _temp = dynamic_cast<TH1*>(GetHistogram(name));
    if (_temp) {
        _temp->Fill(valx);
        return kTRUE;
    }
    return kFALSE;
}
//_____________________________________________________________________________
Bool_t  AliGMFHistogramManager::Fill(TString name, Double_t valx, Double_t valy) {
    // find and fill histogram
    TH2* _temp = dynamic_cast<TH2*>(GetHistogram(name));
    if (_temp) {
        _temp->Fill(valx, valy);
        return kTRUE;
    }
    return kFALSE;
}

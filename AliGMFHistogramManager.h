#ifndef ALIGMFHISTOGRAMMANAGER_H
#define ALIGMFHISTOGRAMMANAGER_H

class TH1D;
class TH2D;
class TH3D;
class THashList;
class TH1;
class TFile;

class AliGMFHistogramManager : public TObject {

    public:
        AliGMFHistogramManager(TString name = "");

        // adding histograms to the list
        TH1D*   BookTH1D(const char* name, const char* x, Int_t bins, Double_t min, Double_t max, Bool_t append = kTRUE);
        TH2D*   BookTH2D(const char* name, const char* x, const char* y, Int_t binsx, Double_t minx, Double_t maxx, Int_t binsy, Double_t miny, Double_t maxy, Bool_t append = kTRUE);
        TH3D*   BookTH3D(const char* name, const char* x, const char* y, const char* z, Int_t binsx, Double_t minx, Double_t maxx, Int_t binsy, Double_t miny, Double_t maxy, Int_t binsz, Double_t minz, Double_t maxz, Bool_t append = kTRUE); 


        // manipulation of object
        void            StoreManager(TFile* of);
        void            StoreManager(const char* title);
        TH1*            GetHistogram(const TString &name);
        Bool_t          Fill(const TString &name, const Double_t &valx);
        Bool_t          Fill(const TString &name, const Double_t &valx, const Double_t &valy);
        Bool_t          Fill(const TString &name, const Double_t &valx, const Double_t &valy, const Double_t &valz);
        Bool_t          StoreRatio(const TString &a, const TString &b, const TString &name);
        void            WeighAllHistograms(const Double_t weight); 

    private:

        // histograms
        THashList*      fOutputList;                   // list of histograms
        TString         fManagerName;                  // manager name

        ClassDef(AliGMFHistogramManager, 1);

};

#endif

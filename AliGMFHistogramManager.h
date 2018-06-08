#ifndef ALIGMFHISTOGRAMMANAGER_H
#define ALIGMFHISTOGRAMMANAGER_H

class TH1D;
class TH2D;
class TH3F;
class TList;
class TObject;
class TFile;

class AliGMFHistogramManager : public TObject {

    public:
        AliGMFHistogramManager(TString name = "");

        // adding histograms to the list
        TH1D*   BookTH1D(const char* name, const char* x, Int_t bins, Double_t min, Double_t max, Bool_t append = kTRUE);
        TH2D*   BookTH2D(const char* name, const char* x, const char* y, Int_t binsx, Double_t minx, Double_t maxx, Int_t binsy, Double_t miny, Double_t maxy, Bool_t append = kTRUE);
        TH3F*   BookTH3F(const char* name, const char* x, const char* y, const char* z, Int_t binsx, Double_t minx, Double_t maxx, Int_t binsy, Double_t miny, Double_t maxy, Int_t binsz, Double_t minz, Double_t maxz, Bool_t append = kTRUE); 


        // manipulation of object
        void            StoreManager(TFile* of);
        void            StoreManager(const char* title);
        TObject*        GetHistogram(TString name);
        Bool_t          Fill(TString name, Double_t valx);
        Bool_t          Fill(TString name, Double_t valx, Double_t valy);
        Bool_t          StoreRatio(TString a, TString b, TString name);

    private:

        // histograms
        TList*   fOutputList;                   // list of histograms
        TString  fManagerName;                  // manager name

        ClassDef(AliGMFHistogramManager, 1);

};

#endif

#ifndef ALIGMFHISTOGRAMMANAGER_H
#define ALIGMFHISTOGRAMMANAGER_H

class TH1F;
class TH2F;
class TH3F;
class TList;
class TObject;

class AliGMFHistogramManager : public TObject {

    public:
        AliGMFHistogramManager();

        // adding histograms to the list
        TH1F*   BookTH1F(const char* name, const char* x, Int_t bins, Double_t min, Double_t max, Bool_t append = kTRUE);
        TH2F*   BookTH2F(const char* name, const char* x, const char* y, Int_t binsx, Double_t minx, Double_t maxx, Int_t binsy, Double_t miny, Double_t maxy, Bool_t append = kTRUE);
        TH3F*   BookTH3F(const char* name, const char* x, const char* y, const char* z, Int_t binsx, Double_t minx, Double_t maxx, Int_t binsy, Double_t miny, Double_t maxy, Int_t binsz, Double_t minz, Double_t maxz, Bool_t append = kTRUE); 


        // manipulation of object
        Bool_t          StoreManager(TString title);
        TObject*        GetHistogram(TString name);
        Bool_t          Fill(TString name, Double_t valx);
        Bool_t          Fill(TString name, Double_t valx, Double_t valy);

    private:

        // histograms
        TList*   fOutputList;                     // list of histograms

        ClassDef(AliGMFHistogramManager, 1);

};

#endif

#ifndef ALIGMFDUMMYJETFINDER_H
#define ALIGMFDUMMYJETFINDER_H

class TH1D;
class AliGMFEventContainer;
class AliGMFHistogramManager;
class AliGMFTTreeEventCuts;

class AliGMFSimpleJetFinder : public TObject {

    public:
        AliGMFSimpleJetFinder();

        // 'task' type members
        Bool_t Initialize();
        Bool_t AnalyzeEvent(AliGMFEventContainer* event);
        Bool_t Finalize(TString name);

        // setters
        void    DoBackgroundSubtraction(Bool_t b)       { fDoBackgroundSubtraction = b; }
        void    SetJetResolution(Float_t r)             { fJetResolution = r; }
        void    SetLeadingHadronPt(Float_t l)           { fLeadingHadronPt = l; }
        void    SetEventCuts(AliGMFTTreeEventCuts* c)   { fEventCuts = c;}

    private:
        Bool_t  fDoBackgroundSubtraction;       // do background subtraction
        Float_t fJetResolution;                 // jet resolution parameter
        Float_t fLeadingHadronPt;               // leading hadron pt
        AliGMFTTreeEventCuts*        fEventCuts;       // event cuts
        // histograms
        AliGMFHistogramManager* fHistogramManager;      // histogram manager

        ClassDef(AliGMFSimpleJetFinder, 1);

};

#endif

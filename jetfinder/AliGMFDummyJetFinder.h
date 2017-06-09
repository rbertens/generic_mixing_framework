#ifndef ALIGMFDUMMYJETFINDER_H
#define ALIGMFDUMMYJETFINDER_H

class TH1D;
class AliGMFEventContainer;
class AliGMFHistogramManager;

class AliGMFDummyJetFinder : public TObject {

    public:
        AliGMFDummyJetFinder();

        // 'task' type members
        Bool_t Initialize();
        Bool_t AnalyzeEvent(AliGMFEventContainer* event);
        Bool_t Finalize();

        // setters
        void    DoBackgroundSubtraction(Bool_t b)       { fDoBackgroundSubtraction = b; }
        void    SetJetResolution(Float_t r)             { fJetResolution = r; }
        void    SetLeadingHadronPt(Float_t l)           { fLeadingHadronPt = l; }

    private:
        Bool_t  fDoBackgroundSubtraction;       // do background subtraction
        Float_t fJetResolution;                 // jet resolution parameter
        Float_t fLeadingHadronPt;               // leading hadron pt

        // histograms
        AliGMFHistogramManager* fHistogramManager;      // histogram manager
        TH1F*                   fHistJetPt;             // dummy histo for jet pt

        ClassDef(AliGMFDummyJetFinder, 1);

};

#endif

#ifndef ALIGMFDUMMYJETFINDER_H
#define ALIGMFDUMMYJETFINDER_H

class TH1D;
class AliGMFEventContainer;
class AliGMFHistogramManager;
class AliGMFSimpleEventCuts;
class AliGMFSimpleTrackCuts;

#include "TF1.h"
#include "AliGMFSimpleTrackCuts.h"

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
        void    SetLeadingHadronMaxPt(Float_t l)        { fLeadingHadronMaxPt = l; }

        void    SetRandomizeEtaPhi(Bool_t r)            { fRandomizeEtaPhi = r; }
        void    SetImprintV2(TF1* v2)                   { fImprintV2 = v2; delete fImprintV3; fImprintV3 = 0x0; }
        void    SetImprintV3(TF1* v3)                   { fImprintV3 = v3; delete fImprintV2; fImprintV2 = 0x0; }
        void    SetEventCuts(AliGMFSimpleEventCuts* c)  { fEventCuts = c;}
        void    SetTrackCuts(AliGMFSimpleTrackCuts* c)  { delete fTrackCuts; fTrackCuts = c;}

        void    SetRejectNHardestJets(Int_t n)          { fRejectNHardestJets = n;}
        void    SetSplittingForTracksWithPtHigherThan(Double_t pt) {
            fSplittingThreshold = pt;
        }
        void    SetSplitTrackPt(Double_t pt) {
            fSplitTrackPt = pt;
        }
        void    SetRandomizeSplitTrackEtaPhi(Bool_t r) {
            fRandomizeSplitTrack = r;
        }

    private:
        Bool_t  fDoBackgroundSubtraction;       // do background subtraction
        Float_t fJetResolution;                 // jet resolution parameter
        Int_t   fNCones;                        // number of random cones
        Float_t fLeadingHadronPt;               // min leading hadron pt
        Float_t fLeadingHadronMaxPt;            // max pt for leading hadron
        Bool_t  fRandomizeEtaPhi;               // shuffle tracks randomly in eta, phi
        TF1*    fImprintV2;                     // imprint pt differential v2 on event
        TF1*    fImprintV3;                     // imprint pt differnetial v3 on event
        Double_t        fSplittingThreshold;    // split tracks with pt higher than this
        Double_t        fSplitTrackPt;          // split tracks have maximally this pt
        Bool_t          fRandomizeSplitTrack;   // split tracks collinearly or give them random positions
        Int_t           fRejectNHardestJets;    // reject N hardest jets from rho estimate
         
         
        AliGMFSimpleEventCuts*        fEventCuts;       // event cuts
        AliGMFSimpleTrackCuts*        fTrackCuts;       // track cuts
        // histograms
        AliGMFHistogramManager* fHistogramManager;      // histogram manager

//        void GetFlowFluctuation(Double_t& vn) const {
//            vn += TMath::Sqrt(2*(vn*.25)*(vn*.25))*TMath::ErfInverse(2*(gRandom->Uniform(0, fFlowFluctuations))-1); 
//        }
        static Double_t PhaseShift(Double_t x) {  
            while (x >= TMath::TwoPi()) x-= TMath::TwoPi();
            while (x < 0.) x += TMath::TwoPi();
            return x; 
        }

        void     GenerateV2(Double_t &phi, Double_t &pt) const;
        void     GenerateV3(Double_t &phi, Double_t &pt) const;
        Bool_t   GetRandomCone(AliGMFEventContainer* event, Float_t &pt, Float_t &eta, Float_t &phi, Float_t etaJet = -2, Float_t phiJet = -10);

        ClassDef(AliGMFSimpleJetFinder, 1);

};

#endif

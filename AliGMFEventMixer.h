#ifndef ALIGMFEVENTMIXER_H
#define ALIGMFEVENTMIXER_H

class TFile;
class TClonesArray;
class TH1D;
class AliGMFEventContainer;
class AliGMFHistogramManager;


class AliGMFEventMixer : public TObject {

    public:
        AliGMFEventMixer();

        // 'task' type members
        Bool_t  Initialize();
        Bool_t  AnalyzeEvent(AliGMFEventContainer* event);
        Bool_t  Finalize();
        void    ParseTracks(AliVEvent* event);
        void    PushToTTree();



    private:

        // histograms
        AliGMFHistogramManager* fHistogramManager;      // histogram manager
        TFile*                  fOutputFile;            // output file
        AliGMFEventContainer    fEvent;                 //! event container
        TClonesArray*           fTrackArray;            //! tracks array

        ClassDef(AliGMFEventMixer, 1);

};

#endif

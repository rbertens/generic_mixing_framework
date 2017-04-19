#ifndef ALIGMFEVENTREADER_H
#define ALIGMFEVENTREADER_H

class AliGMFEventContainer;
class TChain;
class TClonesArray;
class AliGMFTTreeHeader;


class AliGMFEventReader : public TObject {

    public: 
        AliGMFEventReader();
        virtual ~AliGMFEventReader() {;}

        // interface
        Bool_t Initialize();
        AliGMFEventContainer* GetEvent(Int_t i);

        void PrintEventSummary();
        void SetInputChain(TChain* c); 
    
    private:
        TChain*                 fInputChain;            //! input chain
        TClonesArray*           fTracks;                //! tracks
        AliGMFTTreeHeader*      fHeader;                //! header
        Int_t                   fCurrentEvent;          //! current event
        Int_t                   fEvents;                //! total number of events

        ClassDef(AliGMFEventReader, 1)
};

#endif


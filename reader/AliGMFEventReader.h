#ifndef ALIGMFEVENTREADER_H
#define ALIGMFEVENTREADER_H

class AliGMFEventContainer;
class TChain;
class TClonesArray;
class AliGMFTTreeHeader;


class AliGMFEventReader : public TObject {

    public: 
        AliGMFEventReader();
        AliGMFEventReader(TChain* c);
        virtual ~AliGMFEventReader() {;}

        // interface
        Bool_t Initialize();
        AliGMFEventContainer* GetEvent(Int_t i);
        void TouchEvent(Int_t i);

        void SetInputChain(TChain* c); 
    
    private:
        TChain*                 fInputChain;            //! input chain
        TClonesArray*           fTracks;                //! tracks
        AliGMFTTreeHeader*      fHeader;                //! header
        AliGMFEventContainer*   fEventContainer;        //! current event container
        Int_t                   fCurrentEvent;          //! current event
        Int_t                   fEvents;                //! total number of events

        void MoveChainBufferTo(Int_t i);
        
        ClassDef(AliGMFEventReader, 1)
};

#endif


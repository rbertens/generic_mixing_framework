#ifndef ALIGMFEVENTREADER_H
#define ALIGMFEVENTREADER_H

class AliGMFEventContainer;
class TChain;
class TClonesArray;
class AliGMFTTreeHeader;


class AliGMFEventReader : public TObject {

    public: 
        AliGMFEventReader();
        virtual ~AliGMFEventReader() {}

        Bool_t Initialize();
        AliGMFEventContainer* GetEvent(Int_t i);
    private:

        TChain* fInputChain;    //! input chain
        TClonesArray* fTracks;  //! tracks
        AliGMFTTreeHeader* fHeader;     //! header

        ClassDef(AliGMFEventReader, 1)
};

#endif


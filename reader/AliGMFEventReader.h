#ifndef ALIGMFEVENTREADER_H
#define ALIGMFEVENTREADER_H

class aliGMFEventcontainer;

class AliGMFEventReader : public TObject {

    public: 
        AliGMFEventReader();
        virtual ~AliGMFEventReader() {}

        Bool_t Initialize();
        AliGMFEventContainer* GetEvent(Int_t i);

        ClassDef(AliGMFEventReader, 1)
};

#endif


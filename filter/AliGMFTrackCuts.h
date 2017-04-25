#ifndef AliGMFTrackCuts_H
#define AliGMFTrackCuts_H

#include <TObject.h>
class AliVTrack;

class AliGMFTrackCuts : public TObject {

 public:
    AliGMFTrackCuts();
    Bool_t IsSelected(AliVTrack* track);
     
 private:
    ClassDef(AliGMFTrackCuts, 1);    // Help class

};

#endif

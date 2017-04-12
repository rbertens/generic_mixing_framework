// AliGMFEventContainer: Event container
// origin:   Redmer Alexander Bertens, rbertens@cern.ch


//forward declarations
class AliGMFTTreeHeader;
class TClonesArray;

class AliGMFEventContainer : public TObject {

   public:
       AliGMFEventContainer();
       AliGMFEventContainer( 
               AliGMFTTreeHeader* event,
               TClonesArray* array);
       virtual  ~AliGMFEventContainer() {}


       // members
       TClonesArray* fTracks;           //! tclones array with tracks
       AliGMFTTreeHeader* fHeader;        //! event header




       ClassDef(AliGMFEventContainer,1)
};


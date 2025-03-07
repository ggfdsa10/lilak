#ifndef GETLAYEREDFRAME
#define GETLAYEREDFRAME

#include "GETLayerHeader.h"
#include "GETBasicFrame.h"

#include "TClonesArray.h"

class GETLayeredFrame : public GETLayerHeader {
  public:
    GETLayeredFrame();

            Int_t  GetNumFrames();
     TClonesArray *GetFrames();
    GETBasicFrame *GetFrame(Int_t index);

             void  Clear(Option_t * = "");
             void  Read(ifstream &stream);

  private:
     TClonesArray *fFrames;

  ClassDef(GETLayeredFrame, 1);
};

#endif

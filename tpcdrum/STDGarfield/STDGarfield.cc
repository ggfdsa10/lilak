
#include "STDGarfieldInterface.h"

int main()
{
    STDGarfieldInterface* interface = new STDGarfieldInterface();

    // Note: It is important coodinate shift values from Elmer to Local Pad
    // TPC-Drum local pad origin is first layer's center of pad; Elmer origin of coordinate is center of pad plane and shifted height to 9 mm
    // So, we must correct this with shift values.
    double PadHeight = 1.19; // [cm] pad height
    double PadGap = 0.01; //[cm] pad gap between pads
    double LayerNum = 12; // number of layer
    double yShift = (PadHeight+PadGap)*(LayerNum/2.-1.) + (PadHeight+PadGap)/2.;
    double zShift = 0.9; // [cm] for triple GEM surface height
    interface -> SetLocalPadCoordinateShift(0., yShift, zShift); 

    interface -> SetConvertFieldMap(); // default 0.1 cm
    interface -> SetOutputNameTag("3000V");
    
    interface -> Init();
    interface -> Calculate();
    interface -> Finish();

    return 0;
}
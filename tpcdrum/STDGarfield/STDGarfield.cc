
#include "STDGarfieldInterface.h"

int main()
{
    STDGarfieldInterface* interface = new STDGarfieldInterface();

    // Note: It is important coodinate shift values from Elmer to Local Pad
    //       TPC-Drum local pad origin is first layer's center of pad (y pos), PadPlane surface(z pos)
    //       Elmer origin of coordinate is center of pad plane (y pos), first fieldcage layer (z pos)
    //       FieldMap conversion have to convert the coordinates from Elemer to local TPCDrum

    double PadHeight = 1.19; // [cm] pad height
    double PadGap = 0.01; //[cm] pad gap between pads
    double LayerNum = 12; // number of layer
    double yShift = (PadHeight+PadGap)*(LayerNum/2.-1.) + (PadHeight+PadGap)/2.; // [cm]

    double elmerAnodeZ = -0.3; // [cm] world z boundary (-1, 16)cm, anode placed at (-0.6, -0.3) w.r.t. elmer coordinate
    double GEMSurZ = 0.9; // [cm] triple GEM  surface height w.r.t. TPCDrum local coordinate 
    double zShift = GEMSurZ + fabs(elmerAnodeZ); // [cm] Elemr z-pos origin (edge of first layer wall) should be 12 mm in TPCDrum origin

    interface -> SetLocalPadCoordinateShift(0., yShift, zShift); 

    interface -> SetConvertFieldMap(); // default 0.1 cm
    interface -> SetOutputNameTag("3000V");
    
    interface -> Init();
    interface -> Calculate();
    interface -> Finish();

    return 0;
}
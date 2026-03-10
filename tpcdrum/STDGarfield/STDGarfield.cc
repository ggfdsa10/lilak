
#include "STDGarfieldInterface.h"

int main()
{
    STDGarfieldInterface* interface = new STDGarfieldInterface();
    interface -> SetConvertFieldMap(); // default 0.1 cm
    interface -> SetOutputNameTag("3000V");
    
    interface -> Init();
    interface -> Calculate();
    interface -> Finish();

    return 0;
}
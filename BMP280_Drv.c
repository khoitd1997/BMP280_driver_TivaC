#include "BMP280_Drv.h"
#include "TivaC_I2C.h"

void bmp280Init(bmp280* sensor, bmp280_measureSettings settings, bmp280_comProtocol protocol)
{
    //values in settings obtained from BMP280 datasheet page 19
    sensor->protocol=protocol;
    switch(settings)
    {
        case handLow:
            sensor-> mode=Normal;
            sensor-> tempSamp= x2;
            sensor-> presSamp= x16;
            sensor-> samplSet= UltraHigh;
            sensor->iirFilter=x4;
            sensor->standbyTime=62.5;
            break;
        case handDynamic:
            sensor-> mode=Normal;
            sensor-> tempSamp= x1;
            sensor-> presSamp= x4;
            sensor-> samplSet= Standard;
            sensor->iirFilter=x16;
            sensor->standbyTime=0.5;
            break;
        case weatherStat:
            sensor-> mode=Forced;
            sensor-> tempSamp= x1;
            sensor-> presSamp= x1;
            sensor-> samplSet= UltraLow;
            sensor->iirFilter= x0;
            //0.05 is placeholder for now
            sensor->standbyTime=0.05;
            break;
        case elevDetec:
            sensor-> mode=Normal;
            sensor-> tempSamp= x1;
            sensor-> presSamp= x4;
            sensor-> samplSet= Standard;
            sensor->iirFilter= x4;
            sensor->standbyTime=125; 
            break;
        case dropDetec:
            sensor-> mode=Normal;
            sensor-> tempSamp= x1;
            sensor-> presSamp= x2;
            sensor-> samplSet= Low;
            sensor->iirFilter= x0;
            sensor->standbyTime=0.5;
            break;
        case indoorNav:
            sensor-> mode=Normal;
            sensor-> tempSamp= x2;
            sensor-> presSamp= x16;
            sensor-> samplSet= UltraHigh;
            sensor->iirFilter=x16;
            sensor->standbyTime=0.5;
            break;
        default:
            for(;;)
            {
                //stop the mcu if none of these options for now
            }
    }
}

uint8_t bmp280_getID(bmp280)
{
    i2c0_open();
    
    i2c0_close();
}

void bmp280_reset(bmp280);
void bmp280_init(bmp280);

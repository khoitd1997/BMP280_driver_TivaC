#include "BMP280_Drv.h"
#include "TivaC_I2C.h"

typedef enum{status=0, ctrl_meas, config, press_msb, press_lsb, press_xlsb, temp_msb, temp_lsb, temp_xlsb} bmp280_regName;

//added with the bmp280_regName to get the correct address
static uint16_t bmp280_baseRegAddr=0xf3;

//the two special addresses
static uint16_t bmp280_resAddr=0xe0;
static uint16_t bmp280_idAddr=0xd0;

//initialize the bmp280 with predefined value in the datasheet
void bmp280Init(bmp280* sensor, bmp280_measureSettings settings, bmp280_comProtocol protocol)
{
    bmp280->address=0x77;
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

//get manufacturer ID of the bmp280
uint8_t bmp280_getID(bmp280*)
{
    unint8_t ID;
    i2c0_open();
    //write data with no stop signal
    i2c0_single_data_write(bmp280->address, bmp280_idAddr, 0);
    //repeat start and then read the ID 
    ID=i2c0_single_data_read(bmp280->address, 0,1);
    i2c0_close();
    return ID;
}

//reset the bmp280 with a power-on reset
void bmp280_reset(bmp280*)
{
    uint8_t resSeq[2];
    resSeq[0]=bmp280_resAddr;

    //obtain from page 24 datasheet
    resSeq[0]=0xb6;
    i2c0_open();
    i2c0_multiple_data_byte_write(bmp280->address, resSeq, 2);
    i2c0_close();
}


#ifndef _BMP280_DRV_
#define _BMP280_DRV_

#include <stdint.h>


//enum of all the sensors' settings
typedef enum {SPI, I2C} bmp280_comProtocol;

//measurement settings based on the datasheet of BMP280
//may just ask user to enter the settings
typedef enum {handLow=0, handDynamic, weatherStat, elevDetec, dropDetec, indoorNav} bmp280_measureSettings;
typedef enum {Sleep=0, Forced, Normal} bmp280_operMode;
typedef enum {UltraLow=0, Low, Standard, UltraHigh} bmp280_samplSettings;
//used for IIR filter, temp and pressure oversampling coefficience
typedef enum{x0=0, x1, x2, x4, x8, x16} bmp280_Coeff;

//use log for array indexing in oversampling

//used for getting the address of the register in BMp280

typedef struct bmp280Sensor bmp280;

//represent one bmp280 sensor along with its settings
struct bmp280Sensor 
{
    uint8_t ID;
    uint8_t address;
    bmp280_comProtocol protocol;
    //I2C or SPI controller is it assigned to
    uint8_t portNum;

    //temperature and pressure oversampling settings
    bmp280_Coeff tempSamp;
    bmp280_Coeff presSamp;
    bmp280_Coeff iirFilter;
    bmp280_samplSettings samplSet;
    bmp280_operMode mode;

    //unit is ms 
    float standbyTime;
};

//initialize a bmp280 struct
//will be use in a big switch statement for initializing based on
//both predefined option and customizable value 
bmp280* bmp280Init( bmp280_measureSettings,bmp280_comProtocol);
uint8_t bmp280_getID(bmp280*);
void bmp280_reset(bmp280*);



#endif
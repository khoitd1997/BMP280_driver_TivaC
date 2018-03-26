#ifndef _BMP280_DRV_
#define _BMP280_DRV_

#include <stdint.h>

// enum of all the sensors' settings
typedef enum { SPI, I2C } bmp280_comProtocol;

// measurement settings based on the datasheet of BMP280
// may just ask user to enter the settings
typedef enum {
  handLow = 0,
  handDynamic,
  weatherStat,
  elevDetec,
  dropDetec,
  indoorNav,
  custom
} bmp280_measureSettings;
// power mode of the bmp280 sensor
typedef enum { Uninitialized = -1, Sleep, Forced, Normal } bmp280_operMode;

typedef enum {
  Uninitialized = -1,
  UltraLow,
  Low,
  Standard,
  UltraHigh
} bmp280_samplSettings;
// used for IIR filter, temp and pressure oversampling coefficience
typedef enum { Uninitialized = -1, x0, x1, x2, x4, x8, x16 } bmp280_Coeff;

typedef enum {
  ERR_NO_ERR,
  ERR_PORT_NOT_OPEN,
  ERR_SETTING_UNITIALIZED,
  ERR_SETTING_UNRECOGNIZED
} bmp280_errCode;

typedef struct bmp280Sensor bmp280;

// represent one bmp280 sensor along with its settings
struct bmp280Sensor
{
  int     portOpened;  // status flag to see if the I2C/SPI port is open
  uint8_t ID;
  uint8_t address;
  bmp280_comProtocol protocol;
  uint8_t            portNum;  // I2C or SPI controller is it assigned to

  // temperature and pressure oversampling settings
  bmp280_Coeff         tempSamp;
  bmp280_Coeff         presSamp;
  bmp280_Coeff         iirFilter;
  bmp280_samplSettings samplSet;
  bmp280_operMode      mode;

  // unit is ms
  float standbyTime;  // check the data sheet for list of allowed values
};

// initialize a bmp280 struct
// will be use in a big switch statement for initializing based on
// both predefined option and customizable value
void bmp280_init(bmp280*                sensor,
                 bmp280_measureSettings settings,
                 bmp280_comProtocol     protocol);

void bmp280_open(bmp280*         sensor,
                 bmp280_errCode* errCode);  // open the communication channel on
                                            // I2C/SPI bus, all setting must be
                                            // initialized
void bmp280_close(bmp280* sensor, bmp280_errCode* errCode);

// functions used for customizing the setting of the bmp280
// calling these functions will cause a write to the actual hardware
void bmp280_set_temp(bmp280* sensor, bmp280_Coeff tempSetting, bmp280_errCode* errCode);
void bmp280_set_pres(bmp280* sensor, bmp280_Coeff presSetting, bmp280_errCode* errCode);
void bmp280_set_filter(bmp280* sensor, bmp280_Coeff iirSetting, bmp280_errCode* errCode);
void bmp280_set_sampling(bmp280* sensor, bmp280_samplSettings samplingSetting, bmp280_errCode* errCode);
void bmp280_set_mode(bmp280* sensor, bmp280_operMode powerMode, bmp280_errCode* errCode);

uint8_t bmp280_getID(bmp280* sensor, bmp280_errCode* errCode);
void    bmp280_reset(bmp280* sensor, bmp280_errCode* errCode);

#endif
/**
 * Main API file for the driver, settings and register names are mostly taken
 * from the BMP280 datasheet
 */
#ifndef _BMP280_DRV_
#define _BMP280_DRV_

#include <stdint.h>

/*enum of all the sensors' settings and error code*/
typedef enum { SPI, I2C } bmp280_comProtocol;

// predefined settings given by datasheet
typedef enum {
  HandLow = 0,
  HandDynamic,
  WeatherStat,
  ElevDetec,
  DropDetec,
  IndoorNav,
  Custom
} bmp280_measureSettings;
// power mode of the bmp280 sensor
typedef enum { Uninitialized_mode = -1, Sleep, Forced, Normal } bmp280_operMode;

// name of predefined oversampling settings
typedef enum { Uninitialized = -1, UltraLow, Low, Standard, UltraHigh } bmp280_samplSettings;

// used for IIR filter, temp and pressure oversampling coefficience
typedef enum { Uninitialized_coeff = -1, x0, x1, x2, x4, x8, x16 } bmp280_Coeff;

// error code for the bmp280
typedef enum {
  ERR_NO_ERR,  // report that there is no error
  ERR_PORT_NOT_OPEN,
  ERR_SETTING_UNITIALIZED,
  ERR_SETTING_UNRECOGNIZED,
  ERR_SENSOR_UNITIALIZED  // indicate that bmp280 struct is not valid
} bmp280_errCode;

typedef struct bmp280Sensor bmp280;

/*data structure of a bmp280*/
struct bmp280Sensor {
  // protocol information
  uint8_t            ID;
  uint8_t            address;
  bmp280_comProtocol protocol;
  uint8_t            portNum;

  // oversampling settings
  bmp280_Coeff         tempSamp;
  bmp280_Coeff         presSamp;
  bmp280_Coeff         iirFilter;
  bmp280_samplSettings samplSet;
  bmp280_operMode      mode;

  float standbyTime;  // unit is ms, check the data sheet for list of allowed
                      // values
};

/*functions used for beginning or wrapping up communications*/
// initialized the bmp280 struct with either predefined settings or customized
// calling this functions will not result in a write to the hardware
void bmp280_init(bmp280*                sensor,
                 bmp280_measureSettings settings,
                 bmp280_comProtocol     protocol,
                 bmp280_errCode*        errCode);

// open the communication channel on SPI/I2C, all settings must have been
// initialized, will write settings to hardware
void bmp280_open(bmp280* sensor, bmp280_errCode* errCode);

// clean up after communication
void bmp280_close(bmp280* sensor, bmp280_errCode* errCode);

/*functions used for customizing the setting of the bmp280 like
temperature, pressure, general oversampling and power mode
calling these functions will not a write to the actual hardware*/
void bmp280_set_temp(bmp280* sensor, bmp280_Coeff tempSetting, bmp280_errCode* errCode);
void bmp280_set_pres(bmp280* sensor, bmp280_Coeff presSetting, bmp280_errCode* errCode);
void bmp280_set_filter(bmp280* sensor, bmp280_Coeff iirSetting, bmp280_errCode* errCode);
void bmp280_set_sampling(bmp280*              sensor,
                         bmp280_samplSettings samplingSetting,
                         bmp280_errCode*      errCode);
void bmp280_set_mode(bmp280* sensor, bmp280_operMode powerMode, bmp280_errCode* errCode);
// set standby time between read, will be checked against predefined value
void bmp280_set_standby(bmp280* sensor, float standbyTime, bmp280_errCode* errCode);

/*write settings to the actual hardware, settings must have already been
intialized*/
void bmp280_writeMeasSetting(bmp280*, bmp280_errCode* errCode);  // write to the measure register
void bmp280_writeConfSetting(bmp280*, bmp280_errCode* errCode);  // write to the config register

/*functions used for obtaining data or controlling the bmp280*/
uint8_t  bmp280_getID(bmp280* sensor, bmp280_errCode* errCode);
uint8_t  bmp280_getTemp(bmp280* sensor, bmp280_errCode* errCode);
uint32_t bmp280_getPres(bmp280* sensor, bmp280_errCode* errCode);
void     bmp280_reset(bmp280* sensor, bmp280_errCode* errCode);

#endif
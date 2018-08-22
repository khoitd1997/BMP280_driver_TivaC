/**
 * Main API file for the driver, settings and register names are mostly taken
 * from the BMP280 datasheet
 */
#ifndef _BMP280_DRV_
#define _BMP280_DRV_

#include <stdbool.h>
#include <stdint.h>

#include "include/BMP280_Ware.h"

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

typedef struct {
  bool isMeasuring;
  bool isUpdating;
} bmp280Status;

/*data structure of a bmp280*/
typedef struct bmp280Sensor {
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

  bmp280Status lastKnowStatus;
} bmp280;

/*functions used for beginning or wrapping up communications*/
// initialized the bmp280 struct with either predefined settings or customized
// calling this functions will not result in a write to the hardware
bmp280_errCode bmp280_create_predefined_settings(bmp280* sensor, bmp280_measureSettings settings);
bmp280_errCode bmp280_init(bmp280* sensor, bmp280_comProtocol protocol, uint8_t address);

// open the communication channel on SPI/I2C, all settings must have been
// initialized, will write settings to hardware
bmp280_errCode bmp280_open(bmp280* sensor);

// clean up after communication
bmp280_errCode bmp280_close(bmp280* sensor);

/*write new settings to the actual hardware, settings must have already been
intialized*/
// TODO: see if can implement changing individual settings like change mode from sleep to normal
bmp280_errCode bmp280_update_setting(bmp280* sensor);

/*functions used for obtaining data or controlling the bmp280*/
bmp280_errCode bmp280_getID(bmp280* sensor, uint8_t* ID);
bmp280_errCode bmp280_get_temp(bmp280* sensor, float* temperature);
bmp280_errCode bmp280_get_press(bmp280* sensor, float* pressure);
bmp280_errCode bmp280_get_temp_press(bmp280*            sensor,
                                     float*             temperatureC,
                                     float*             pressPa,
                                     bmp280_calib_param calibParam);
bmp280_errCode bmp280_reset(bmp280* sensor);
bmp280_errCode bmp280_get_ctr_meas(bmp280* sensor, uint8_t* ctrlMeasRtr);
bmp280_errCode bmp280_get_config(bmp280* sensor, uint8_t* configReturn);
bmp280_errCode bmp280_get_calibration_data(bmp280* sensor, bmp280_calib_param* calibParam);
bmp280_errCode bmp280_get_status(bmp280* sensor);

#endif
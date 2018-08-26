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
typedef enum { SPI, I2C } Bmp280ComProtocol;

// predefined settings given by datasheet
typedef enum {
  HandLow = 0,
  HandDynamic,
  WeatherStat,
  ElevDetec,
  DropDetec,
  IndoorNav,
  Custom
} Bmp280MeasureSettings;
// power mode of the bmp280 sensor
typedef enum { Uninitialized_mode = -1, Sleep, Forced, Normal } Bmp280OperMode;

// name of predefined oversampling settings
typedef enum { Uninitialized = -1, UltraLow, Low, Standard, UltraHigh } Bmp280SamplSettings;

// used for IIR filter, temp and pressure oversampling coefficience
typedef enum { Uninitialized_coeff = -1, x0, x1, x2, x4, x8, x16 } Bmp280Coeff;

// error code for the bmp280
typedef enum {
  ERR_NO_ERR,  // report that there is no error
  ERR_PORT_NOT_OPEN,
  ERR_SETTING_UNITIALIZED,
  ERR_SETTING_UNRECOGNIZED,
  ERR_SENSOR_UNITIALIZED  // indicate that bmp280 struct is not valid
} Bmp280ErrCode;

typedef struct {
  bool isMeasuring;
  bool isUpdating;
} Bmp280Status;

/*data structure of a bmp280*/
typedef struct bmp280Sensor {
  // protocol information
  uint8_t           ID;
  uint8_t           address;
  Bmp280ComProtocol protocol;

  // oversampling settings
  Bmp280Coeff         tempSamp;
  Bmp280Coeff         presSamp;
  Bmp280Coeff         iirFilter;
  Bmp280SamplSettings samplSet;
  Bmp280OperMode      mode;

  float standbyTime;  // unit is ms, check the data sheet for list of allowed
                      // values

  Bmp280Status lastKnowStatus;
} bmp280;

/*functions used for beginning or wrapping up communications*/
// initialized the bmp280 struct with either predefined settings or customized
// calling this functions will not result in a write to the hardware
Bmp280ErrCode bmp280_create_predefined_settings(bmp280*                     sensor,
                                                const Bmp280MeasureSettings settings);
Bmp280ErrCode bmp280_init(bmp280* sensor, const Bmp280ComProtocol protocol, const uint8_t address);

// open the communication channel on SPI/I2C, all settings must have been
// initialized, will write settings to hardware
Bmp280ErrCode bmp280_open(bmp280* sensor);

// clean up after communication
Bmp280ErrCode bmp280_close(bmp280* sensor);

/*write new settings to the actual hardware, settings must have already been
intialized*/
// TODO: see if can implement changing individual settings like change mode from sleep to normal
Bmp280ErrCode bmp280_update_setting(bmp280* sensor);

/*functions used for obtaining data or controlling the bmp280*/
Bmp280ErrCode bmp280_get_id(bmp280* sensor, uint8_t* ID);
Bmp280ErrCode bmp280_get_temp(bmp280* sensor, float* temperature);
Bmp280ErrCode bmp280_get_press(bmp280* sensor, float* pressure);
Bmp280ErrCode bmp280_get_temp_press(bmp280*          sensor,
                                    float*           temperatureC,
                                    float*           pressPa,
                                    Bmp280CalibParam calibParam);
Bmp280ErrCode bmp280_reset(bmp280* sensor);
Bmp280ErrCode bmp280_get_ctr_meas(bmp280* sensor, uint8_t* ctrlMeasRtr);
Bmp280ErrCode bmp280_get_config(bmp280* sensor, uint8_t* configReturn);
Bmp280ErrCode bmp280_get_calibration_data(bmp280* sensor, Bmp280CalibParam* calibParam);
Bmp280ErrCode bmp280_get_status(bmp280* sensor);
Bmp280ErrCode bmp280_create_custom_setting(bmp280*                   sensor,
                                           const Bmp280Coeff         tempSamp,
                                           const Bmp280Coeff         presSamp,
                                           const Bmp280Coeff         iirFilter,
                                           const Bmp280SamplSettings samplSet,
                                           const Bmp280OperMode      mode,
                                           const float               standbyTime);
#endif
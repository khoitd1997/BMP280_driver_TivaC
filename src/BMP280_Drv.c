#include "include/BMP280_Drv.h"

#include <assert.h>
#include <stdbool.h>

#include "external/TivaC_Utils/include/bit_manipulation.h"
#include "include/BMP280_Utils.h"
#include "include/BMP280_Ware.h"
#include "include/TivaC_I2C.h"
#include "include/TivaC_SPI.h"

// offset from BMP280 base register
typedef enum {
  Status = 0,
  Ctrl_meas,
  Config,
  Press_msb = 4,
  Press_lsb,
  Press_xlsb,
  Temp_msb,
  Temp_lsb,
  Temp_xlsb
} bmp280_regName;

// added with the bmp280_regName to get the correct address
#define BMP280_BASEADDR 0xF3

#define RAW_TEM_TOTAL_BIT 3
#define RAW_PRESS_TOTAL_BIT 3
#define BMP280_CALIB_START_ADDR 0x88

// the two special addresses
#define BMP280_RESADDR 0xE0
#define BMP280_IDARR 0xD0

#define BMP280_MEASURING_MASK 0x8
#define BMP280_UPDATING_MASK 0x1

// initialize the bmp280 with predefined value in the datasheet
Bmp280ErrCode bmp280_create_predefined_settings(bmp280*                     sensor,
                                                const Bmp280MeasureSettings settings) {
  switch (settings) {
    case HandLow:
      sensor->mode        = Normal;
      sensor->tempSamp    = x2;
      sensor->presSamp    = x16;
      sensor->samplSet    = UltraHigh;
      sensor->iirFilter   = x4;
      sensor->standbyTime = 62.5;
      break;

    case HandDynamic:
      sensor->mode        = Normal;
      sensor->tempSamp    = x1;
      sensor->presSamp    = x4;
      sensor->samplSet    = Standard;
      sensor->iirFilter   = x16;
      sensor->standbyTime = 0.5;
      break;

    case WeatherStat:
      sensor->mode        = Forced;
      sensor->tempSamp    = x1;
      sensor->presSamp    = x1;
      sensor->samplSet    = UltraLow;
      sensor->iirFilter   = x0;
      sensor->standbyTime = 0.5;  // 0.05 is placeholder for now
      break;

    case ElevDetec:
      sensor->mode        = Normal;
      sensor->tempSamp    = x1;
      sensor->presSamp    = x4;
      sensor->samplSet    = Standard;
      sensor->iirFilter   = x4;
      sensor->standbyTime = 125;
      break;

    case DropDetec:
      sensor->mode        = Normal;
      sensor->tempSamp    = x1;
      sensor->presSamp    = x2;
      sensor->samplSet    = Low;
      sensor->iirFilter   = x0;
      sensor->standbyTime = 0.5;
      break;

    case IndoorNav:
      sensor->mode        = Normal;
      sensor->tempSamp    = x2;
      sensor->presSamp    = x16;
      sensor->samplSet    = UltraHigh;
      sensor->iirFilter   = x16;
      sensor->standbyTime = 0.5;
      break;

    default:
      return ERR_SETTING_UNRECOGNIZED;
  }
  return ERR_NO_ERR;
}

Bmp280ErrCode bmp280_create_custom_setting(bmp280*                   sensor,
                                           const Bmp280Coeff         tempSamp,
                                           const Bmp280Coeff         presSamp,
                                           const Bmp280Coeff         iirFilter,
                                           const Bmp280SamplSettings samplSet,
                                           const Bmp280OperMode      mode,
                                           const float               standbyTime) {
  sensor->mode        = mode;
  sensor->tempSamp    = tempSamp;
  sensor->presSamp    = presSamp;
  sensor->samplSet    = samplSet;
  sensor->iirFilter   = iirFilter;
  sensor->standbyTime = standbyTime;
  BMP280_TRY_FUNC(bmp280_check_setting(sensor));
  return ERR_NO_ERR;
}

Bmp280ErrCode bmp280_init(bmp280* sensor, const Bmp280ComProtocol protocol, const uint8_t address) {
  if (NULL == sensor) { return ERR_SENSOR_UNITIALIZED; }
  BMP280_TRY_FUNC(bmp280_check_setting(sensor));

  sensor->portNum  = 0;  // hard coded for 0
  sensor->address  = address;
  sensor->protocol = protocol;  // settings obtained in datasheet pg 19

  return ERR_NO_ERR;
}

Bmp280ErrCode bmp280_open(bmp280* sensor) {
  BMP280_TRY_FUNC(bmp280_check_setting(sensor));
  bmp280_open_i2c_spi(sensor);
  return ERR_NO_ERR;
}

Bmp280ErrCode bmp280_close(bmp280* sensor) { bmp280_close_i2c_spi(sensor); }

Bmp280ErrCode bmp280_get_id(bmp280* sensor, uint8_t* returnID) {
  bmp280_get_register(sensor, BMP280_IDARR, returnID, 1);
  sensor->ID = *returnID;
  return ERR_NO_ERR;
}

Bmp280ErrCode bmp280_reset(bmp280* sensor) {
  BMP280_TRY_FUNC(bmp280_port_prep(sensor));

  uint8_t resetRegister[1];
  uint8_t resetData[1];
  resetRegister[0] = BMP280_RESADDR;
  resetData[0]     = 0xB6;  // obtain from page 24 datasheet

  bmp280_write_register(sensor, resetRegister, 1, resetData);
  return ERR_NO_ERR;
}

Bmp280ErrCode bmp280_update_setting(bmp280* sensor) {
  // preparing data byte for writing to bmp280 register
  uint8_t i2cRegisterList[2];
  uint8_t i2cRegisterData[2];

  // handle control byte first
  i2cRegisterList[0] = BMP280_BASEADDR + Ctrl_meas;
  bmp280_make_ctrl_byte(sensor, &i2cRegisterData[0]);

  // handle config byte
  i2cRegisterList[1] = BMP280_BASEADDR + Config;
  bmp280_make_cfg_byte(sensor, &i2cRegisterData[1]);
  bmp280_write_register(sensor, i2cRegisterList, 2, i2cRegisterData);

  return ERR_NO_ERR;
}

Bmp280ErrCode bmp280_get_ctr_meas(bmp280* sensor, uint8_t* ctrlMeasReturn) {
  bmp280_get_register(sensor, BMP280_BASEADDR + Ctrl_meas, ctrlMeasReturn, 1);
  return ERR_NO_ERR;
}

Bmp280ErrCode bmp280_get_config(bmp280* sensor, uint8_t* configReturn) {
  bmp280_get_register(sensor, BMP280_BASEADDR + Config, configReturn, 1);
  return ERR_NO_ERR;
}

Bmp280ErrCode bmp280_get_status(bmp280* sensor) {
  uint8_t statusReturn;
  bmp280_get_register(sensor, BMP280_BASEADDR + Status, &statusReturn, 1);
  bit_get(statusReturn, BMP280_MEASURING_MASK) ? (sensor->lastKnowStatus.isMeasuring = true)
                                               : (sensor->lastKnowStatus.isMeasuring = false);
  bit_get(statusReturn, BMP280_UPDATING_MASK) ? (sensor->lastKnowStatus.isUpdating = true)
                                              : (sensor->lastKnowStatus.isUpdating = false);
  return ERR_NO_ERR;
}

Bmp280ErrCode bmp280_get_temp_press(bmp280*          sensor,
                                    float*           temperatureC,
                                    float*           pressPa,
                                    Bmp280CalibParam calibParam) {
  BMP280_TRY_FUNC(bmp280_port_prep(sensor));
  uint8_t rawData[RAW_TEM_TOTAL_BIT + RAW_PRESS_TOTAL_BIT + 4];
  bmp280_get_register(
      sensor, BMP280_BASEADDR + Press_msb, rawData, RAW_TEM_TOTAL_BIT + RAW_PRESS_TOTAL_BIT + 4);
  int32_t rawPress = (int32_t)((((uint32_t)(rawData[0])) << 12) | (((uint32_t)(rawData[1])) << 4) |
                               ((uint32_t)rawData[2] >> 4));

  int32_t rawTemp = (int32_t)((((int32_t)(rawData[3])) << 12) | (((int32_t)(rawData[4])) << 4) |
                              (((int32_t)(rawData[5])) >> 4));
  *temperatureC   = (float)bmp280_compensate_T_int32(rawTemp, &calibParam) * 0.01;
  *pressPa        = (float)bmp280_compensate_P_int64(rawPress, &calibParam) / 256.0;
  return ERR_NO_ERR;
}

Bmp280ErrCode bmp280_get_calibration_data(bmp280* sensor, Bmp280CalibParam* calibParam) {
  BMP280_TRY_FUNC(bmp280_port_prep(sensor));
  uint8_t rawCalibData[BMP280_CALIB_DATA_SIZE + 5];

  // LSB bits are at lower addr compared to MSB so the first number read will be LSB
  bmp280_get_register(sensor, BMP280_CALIB_START_ADDR, rawCalibData, BMP280_CALIB_DATA_SIZE + 5);
  bmp280_get_calib_param(rawCalibData, calibParam);

  return ERR_NO_ERR;
}

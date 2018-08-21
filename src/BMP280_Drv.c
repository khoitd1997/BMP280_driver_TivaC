/**
 * This is the main driver file for bmp280 but need i2c file from tivaC to work
 *
 */

#include "include/BMP280_Drv.h"

#include <assert.h>

#include "include/BMP280_Utils.h"
#include "include/BMP280_Ware.h"
#include "include/TivaC_I2C.h"

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

// initialize the bmp280 with predefined value in the datasheet
bmp280_errCode bmp280_create_predefined_settings(bmp280* sensor, bmp280_measureSettings settings) {
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

bmp280_errCode bmp280_init(bmp280* sensor, bmp280_comProtocol protocol) {
  if (sensor == NULL) { return ERR_SENSOR_UNITIALIZED; }
  BMP280_TRY_FUNC(bmp280_check_setting(sensor));

  sensor->portNum  = 0;         // 0 for now
  sensor->address  = 0x77;      // value pretty much hardcoded due to hardware
  sensor->protocol = protocol;  // settings obtained in datasheet pg 19

  return ERR_NO_ERR;
}

bmp280_errCode bmp280_open(bmp280* sensor) {
  BMP280_TRY_FUNC(bmp280_check_setting(sensor));

  if (sensor->protocol == I2C) {
    // preparing data byte for writing to bmp280 register
    uint8_t i2cWriteBytes[4];  // 2 for register addresses and 2 for the
                               // setting byte

    // handle control byte first
    i2cWriteBytes[0] = BMP280_BASEADDR + Ctrl_meas;
    bmp280_createCtrlByte(sensor, &i2cWriteBytes[1]);

    // handle config byte
    i2cWriteBytes[2] = BMP280_BASEADDR + Config;
    bmp280_createConfigByte(sensor, &i2cWriteBytes[3]);

    i2c0_open();
  }

  return ERR_NO_ERR;
}

bmp280_errCode bmp280_getID(bmp280* sensor, uint8_t* returnID) {
  uint8_t ID;
  bmp280_get_one_register(sensor, BMP280_IDARR, &ID);
  sensor->ID = ID;
  return ERR_NO_ERR;
}

bmp280_errCode bmp280_reset(bmp280* sensor) {
  BMP280_TRY_FUNC(bmp280_port_prep(sensor));

  uint8_t resSeq[2];
  resSeq[0] = BMP280_RESADDR;
  resSeq[1] = 0xB6;  // obtain from page 24 datasheet

  if (sensor->protocol == I2C) { i2c0_multiple_data_byte_write(sensor->address, resSeq, 2); }
  // TODO: do SPI

  return ERR_NO_ERR;
}

bmp280_errCode bmp280_update_setting(bmp280* sensor) {
  if (sensor->protocol == I2C) {
    // preparing data byte for writing to bmp280 register
    uint8_t i2cWriteBytes[4];  // 2 for register addresses and 2 for the
                               // setting byte

    // handle control byte first
    i2cWriteBytes[0] = BMP280_BASEADDR + Ctrl_meas;
    // bmp280_createCtrlByte(sensor, &i2cWriteBytes[1]);
    i2cWriteBytes[1] = 0x3F;

    // handle config byte
    i2cWriteBytes[2] = BMP280_BASEADDR + Config;
    // bmp280_createConfigByte(sensor, &i2cWriteBytes[3]);
    i2c0_multiple_data_byte_write(sensor->address, i2cWriteBytes, 2);
  }

  return ERR_NO_ERR;
}

bmp280_errCode bmp280_get_ctr_meas(bmp280* sensor, uint8_t* ctrlMeasReturn) {
  bmp280_get_one_register(sensor, BMP280_BASEADDR + Ctrl_meas, ctrlMeasReturn);
  return ERR_NO_ERR;
}

bmp280_errCode bmp280_get_config(bmp280* sensor, uint8_t* configReturn) {
  bmp280_get_one_register(sensor, BMP280_BASEADDR + Config, configReturn);
  return ERR_NO_ERR;
}

bmp280_errCode bmp280_get_temp_press(bmp280*            sensor,
                                     float*             temperatureC,
                                     float*             pressPa,
                                     bmp280_calib_param calibParam) {
  BMP280_TRY_FUNC(bmp280_port_prep(sensor));
  uint8_t rawData[RAW_TEM_TOTAL_BIT + RAW_PRESS_TOTAL_BIT + 4];
  bmp280_get_multiple_register(
      sensor, BMP280_BASEADDR + Press_msb, rawData, RAW_TEM_TOTAL_BIT + RAW_PRESS_TOTAL_BIT + 4);
  int32_t rawPress = (int32_t)((((uint32_t)(rawData[0])) << 12) | (((uint32_t)(rawData[1])) << 4) |
                               ((uint32_t)rawData[2] >> 4));

  int32_t rawTemp = (int32_t)((((int32_t)(rawData[3])) << 12) | (((int32_t)(rawData[4])) << 4) |
                              (((int32_t)(rawData[5])) >> 4));
  *temperatureC   = (float)bmp280_compensate_T_int32(rawTemp, &calibParam) * 0.01;
  *pressPa        = (float)bmp280_compensate_P_int64(rawPress, &calibParam) / 256.0;
  return ERR_NO_ERR;
}

bmp280_errCode bmp280_get_calibration_data(bmp280* sensor, bmp280_calib_param* calibParam) {
  BMP280_TRY_FUNC(bmp280_port_prep(sensor));
  uint8_t rawCalibData[BMP280_CALIB_DATA_SIZE + 5];

  // LSB bits are at lower addr compared to MSB so the first number read will be LSB
  bmp280_get_multiple_register(
      sensor, BMP280_CALIB_START_ADDR, rawCalibData, BMP280_CALIB_DATA_SIZE + 5);
  bmp280_get_calib_param(rawCalibData, calibParam);

  return ERR_NO_ERR;
}

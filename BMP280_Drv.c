/**
 * This is the main driver file for bmp280 but need i2c file from tivaC to work
 *
 */

#include "BMP280_Drv.h"
#include "BMP280_Utils.h"
#include "TivaC_I2C.h"

// offset from BMP280 base register
typedef enum {
  Status = 0,
  Ctrl_meas,
  Config,
  Cress_msb,
  Press_lsb,
  Press_xlsb,
  Temp_msb,
  Temp_lsb,
  Temp_xlsb
} bmp280_regName;

// added with the bmp280_regName to get the correct address
#define BMP280_BASEADDR 0xF3

// the two special addresses
#define BMP280_RESADDR 0xE0
#define BMP280_IDARR 0xD0

// initialize the bmp280 with predefined value in the datasheet
void bmp280_init(bmp280*                sensor,
                 bmp280_measureSettings settings,
                 bmp280_comProtocol     protocol,
                 bmp280_errCode*        errCode)
{
  if (sensor == NULL)
    {
      *errCode = ERR_SENSOR_UNITIALIZED;
      return;
    }
    
  sensor->portNum  = 0;         // 0 for now
  sensor->address  = 0x77;      // value pretty much hardcoded due to hardware
  sensor->protocol = protocol;  // settings obtained in datasheet pg 19
  switch (settings)
    {
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

      case Custom:
        sensor->mode        = Uninitialized_mode;
        sensor->tempSamp    = Uninitialized_coeff;
        sensor->presSamp    = Uninitialized_coeff;
        sensor->samplSet    = Uninitialized_coeff;
        sensor->iirFilter   = Uninitialized_coeff;
        sensor->standbyTime = -1;

      default:
        for (;;)
          {
            // stop the mcu if none of these options for now
          }
    }
  *errCode = ERR_NO_ERR;
}

void bmp280_open(bmp280* sensor, bmp280_errCode* errCode)
{
  if (bmp280_check_setting(sensor, errCode)) return;

  if (sensor->protocol == I2C)
    {
      // preparing data byte for writing to bmp280 register
      uint8_t i2cWriteBytes[4];  // 2 for register addresses and 2 for the
                                 // setting byte

      // handle control byte first
      i2cWriteBytes[0] = BMP280_BASEADDR + Ctrl_meas;
      i2cWriteBytes[1] = bmp280_createCtrlByte(sensor, errCode);

      // handle config byte
      i2cWriteBytes[2] = BMP280_BASEADDR + Config;
      i2cWriteBytes[3] = bmp280_createConfigByte(sensor, errCode);

      if (*errCode != ERR_NO_ERR)
        {
          return;
        }
      i2c0_open();
      i2c0_multiple_data_byte_write(sensor->address, i2cWriteBytes, 4);
    }

  *errCode = ERR_NO_ERR;
}

uint8_t bmp280_getID(bmp280* sensor, bmp280_errCode* errCode)
{
  if (bmp280_portPrep(sensor, errCode)) return 1;

  uint8_t ID;
  if (sensor->protocol == I2C)
    {
      i2c0_waitBusy();
      // write data with no stop signal
      i2c0_single_data_write(sensor->address, BMP280_IDARR, 1);
      i2c0_waitBusy();
      // repeat start and then read the ID
      ID = i2c0_single_data_read(sensor->address, 1, 1, 1);
      i2c0_waitBusy();
      i2c0_stop();
      i2c0_waitBusy();
    }
  sensor->ID = ID;
  return ID;
}

void bmp280_reset(bmp280* sensor, bmp280_errCode* errCode)
{
  if (bmp280_portPrep(sensor, errCode)) return;

  uint8_t resSeq[2];
  resSeq[0] = BMP280_RESADDR;
  resSeq[1] = 0xB6;  // obtain from page 24 datasheet

  if (sensor->protocol == I2C)
    {
      i2c0_waitBusy();
      i2c0_stop();
      i2c0_multiple_data_byte_write(sensor->address, resSeq, 2);
    }
  // TODO: do SPI
}

void bmp280_set_temp(bmp280*         sensor,
                     bmp280_Coeff    tempSetting,
                     bmp280_errCode* errCode)
{
  if (sensor == NULL)
    {
      errCode = ERR_SENSOR_UNITIALIZED;
      return;
    }
  sensor->tempSamp = tempSetting;
  *errCode         = ERR_NO_ERR;
}
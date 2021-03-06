/**
 * @brief Utils files contain helper functions for BMP280_Drv.c files as well as the glues to work
 * with I2C and SPI specific stuffs
 *
 * @file BMP280_Utils.c
 * @author Khoi Trinh
 * @date 2018-08-25
 */

#include "include/BMP280_Utils.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "include/BMP280_Drv.h"
#include "include/TivaC_I2C.h"
#include "include/TivaC_SPI.h"

/**
 * @brief check user settings to make sure they are among the supported options
 *
 */
Bmp280ErrCode bmp280_check_setting(bmp280* sensor) {
  if (sensor == NULL) {
    return ERR_SENSOR_UNITIALIZED;
  }

  else {
    if (sensor->mode == Uninitialized_mode || sensor->tempSamp == Uninitialized_coeff ||
        sensor->presSamp == Uninitialized_coeff || sensor->samplSet == Uninitialized ||
        sensor->iirFilter == Uninitialized_coeff || sensor->standbyTime == -1) {
      return ERR_SETTING_UNITIALIZED;

    } else {
      return ERR_NO_ERR;
    }
  }
}

/**
 * @brief port check that are run before data communication
 *
 */
Bmp280ErrCode bmp280_port_check(bmp280* sensor) {
  if ((I2C == sensor->protocol && (I2C0_NO_ERR != i2c0_check_master_enabled())) ||
      ((SPI == sensor->protocol) && (SPI_ERR_NO_ERR != spi_check_spi_enabled()))) {
    return ERR_PORT_NOT_OPEN;
  }

  return ERR_NO_ERR;
}

/**
 * @brief create a byte corresponding to user option to be sent to the control register on the
 * BMP280
 *
 */
Bmp280ErrCode bmp280_make_ctrl_byte(bmp280* sensor, uint8_t* controlByte) {
  // start with all 1 and zero out the needed bits
  uint8_t tempByte = 0xFF;
  switch (sensor->tempSamp) {
    case x0:
      tempByte &= 0x1F;
      break;

    case x1:
      tempByte &= 0x3F;
      break;

    case x2:
      tempByte &= 0x5F;
      break;

    case x4:
      tempByte &= 0x7F;
      break;

    case x8:
      tempByte &= 0x9F;
      break;

    case x16:
      tempByte &= 0xFF;
      break;

    default:
      return ERR_SETTING_UNRECOGNIZED;
      break;
  }

  switch (sensor->presSamp) {
    case x0:
      tempByte &= 0xE3;
      break;

    case x1:
      tempByte &= 0xE7;
      break;

    case x2:
      tempByte &= 0xEB;
      break;

    case x4:
      tempByte &= 0xEF;
      break;

    case x8:
      tempByte &= 0xF3;
      break;

    case x16:
      tempByte &= 0xFF;
      break;

    default:
      return ERR_SETTING_UNRECOGNIZED;
      break;
  }

  switch (sensor->mode) {
    case Sleep:
      tempByte &= 0xFC;
      break;

    case Forced:
      tempByte &= 0xFD;
      break;

    case Normal:
      tempByte &= 0xFF;
      break;

    default:
      return ERR_SETTING_UNRECOGNIZED;
      break;
  }

  *controlByte = tempByte;
  return ERR_NO_ERR;
}

/**
 * @brief create a byte corresponding to user option to be sent to the config register on the
 * BMP280
 *
 */
uint8_t bmp280_make_cfg_byte(bmp280* sensor, uint8_t* returnByte) {
  // start with all 1 and zero out the needed bits
  uint8_t tempByte = 0xFF;

  // handle standby time
  if (sensor->standbyTime == 0.5) {
    tempByte &= 0x1F;
  } else if (sensor->standbyTime == 62.5) {
    tempByte &= 0x3F;
  } else if (sensor->standbyTime == 125) {
    tempByte &= 0x5F;
  } else if (sensor->standbyTime == 250) {
    tempByte &= 0x7F;
  } else if (sensor->standbyTime == 500) {
    tempByte &= 0x9F;
  } else if (sensor->standbyTime == 1000) {
    tempByte &= 0xBF;
  } else if (sensor->standbyTime == 2000) {
    tempByte &= 0xDF;
  } else if (sensor->standbyTime == 4000) {
    tempByte &= 0xFF;
  } else {
    return ERR_SETTING_UNRECOGNIZED;
  }

  // handle iir filter sampling
  switch (sensor->iirFilter) {
    case x0:
      tempByte &= 0xE3;
      break;

    case x2:
      tempByte &= 0xEB;
      break;

    case x4:
      tempByte &= 0xEF;
      break;

    case x8:
      tempByte &= 0xF3;
      break;

    case x16:
      tempByte &= 0xFF;
      break;

    default:
      return ERR_SETTING_UNRECOGNIZED;
      return 1;
      break;
  }

  // set protocol to be I2C or SPI
  if (sensor->protocol == I2C) { tempByte &= 0xFE; }

  return ERR_NO_ERR;
  *returnByte = tempByte;
}

/**
 * @brief open spi or i2c communications
 *
 */
Bmp280ErrCode bmp280_open_i2c_spi(bmp280* sensor) {
  if (I2C == sensor->protocol) {
    i2c0_open();
  } else if (SPI == sensor->protocol) {
    const SpiSettings spiSetting = {.spiBitRateMbits = 0.3,
                                    .cpuClockMHz     = 16,
                                    .cpol            = 1,
                                    .cpha            = 1,
                                    .operMode        = Freescale,
                                    .isLoopBack      = false,
                                    .transferSizeBit = 8,
                                    .role            = Master,
                                    .clockSource     = Systemclock};

    spi_open(spiSetting);
  }
  return ERR_NO_ERR;
}

/**
 * @brief close i2c or spi communications
 *
 */
Bmp280ErrCode bmp280_close_i2c_spi(bmp280* sensor) {
  if (I2C == sensor->protocol) {
    i2c0_close();
  } else if (SPI == sensor->protocol) {
    spi_close();
  }
  return ERR_NO_ERR;
}

/**
 * @brief things to run to prep the i2c/spi port for communication
 *
 */
Bmp280ErrCode bmp280_port_prep(bmp280* sensor) {
  BMP280_TRY_FUNC(bmp280_port_check(sensor));
  return ERR_NO_ERR;
}

/**
 * @brief protocol agnostic function to get data from one or multiple register
 *
 */
Bmp280ErrCode bmp280_get_register(bmp280*       sensor,
                                  const uint8_t startAddr,
                                  uint8_t*      regData,
                                  const uint8_t totalRegister) {
  if (I2C == sensor->protocol) {
    I2C0_TRY_FUNC(i2c0_wait_bus());
    if (totalRegister > 1) {
      // write data with no stop signal
      i2c0_single_data_write(sensor->address, startAddr, true);
      i2c0_multiple_data_byte_read(sensor->address, regData, totalRegister);
    } else if (totalRegister == 1) {
      // write data with no stop signal
      i2c0_single_data_write(sensor->address, startAddr, true);
      I2C0_TRY_FUNC(i2c0_wait_bus());

      // read 3 bytes bc reading single byte seems to create weird behaviours with this sensor
      uint8_t inputBuffer[3];
      i2c0_multiple_data_byte_read(sensor->address, inputBuffer, 3);
      *regData = inputBuffer[0];
    }
  } else if (SPI == sensor->protocol) {
    const SpiSettings spiSetting = {.spiBitRateMbits = 0.3,
                                    .cpuClockMHz     = 16,
                                    .cpol            = 1,
                                    .cpha            = 1,
                                    .operMode        = Freescale,
                                    .isLoopBack      = false,
                                    .transferSizeBit = 8,
                                    .role            = Master,
                                    .clockSource     = Systemclock};
    uint8_t           addressList[1];
    addressList[0] = startAddr;
    spi_transfer(spiSetting, addressList, 1, regData, totalRegister);
  }

  return ERR_NO_ERR;
}

/**
 * @brief protocol agnostic function to write data to one or multiple register
 *
 */
Bmp280ErrCode bmp280_write_register(bmp280*        sensor,
                                    const uint8_t* registerList,
                                    const uint8_t  totalRegister,
                                    const uint8_t* registerDataList) {
  if (I2C == sensor->protocol) {
    uint8_t regDataPair[2];
    for (int regIndex = 0; regIndex < totalRegister; ++regIndex) {
      regDataPair[0] = registerList[regIndex];
      regDataPair[1] = registerDataList[regIndex];
      i2c0_multiple_data_byte_write(sensor->address, regDataPair, 2);
    }
  } else if (SPI == sensor->protocol) {
    const SpiSettings spiSetting = {.spiBitRateMbits = 0.3,
                                    .cpuClockMHz     = 16,
                                    .cpol            = 1,
                                    .cpha            = 1,
                                    .operMode        = Freescale,
                                    .isLoopBack      = false,
                                    .transferSizeBit = 8,
                                    .role            = Master,
                                    .clockSource     = Systemclock};
    uint8_t           regDataPair[2];
    for (int regIndex = 0; regIndex < totalRegister; ++regIndex) {
      regDataPair[0] = registerList[regIndex];
      regDataPair[1] = registerDataList[regIndex];
      spi_transfer(spiSetting, regDataPair, 2, NULL, 0);
    }
  }
  return ERR_NO_ERR;
}

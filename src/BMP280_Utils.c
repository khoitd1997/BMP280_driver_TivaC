#include "include/BMP280_Utils.h"

#include <assert.h>
#include <stdio.h>

#include "include/BMP280_Drv.h"
#include "include/TivaC_I2C.h"

bmp280_errCode bmp280_check_setting(bmp280* sensor) {
  if (sensor == NULL) {
    return ERR_SENSOR_UNITIALIZED;
  }

  else {
    if (sensor->mode == Uninitialized_mode || sensor->tempSamp == Uninitialized_coeff ||
        sensor->presSamp == Uninitialized_coeff || sensor->samplSet == Uninitialized_coeff ||
        sensor->iirFilter == Uninitialized_coeff || sensor->standbyTime == -1) {
      return ERR_SETTING_UNITIALIZED;

    } else {
      return ERR_NO_ERR;
    }
  }
}

bmp280_errCode bmp280_checkPortOpened(bmp280* sensor) {
  if (sensor->protocol == I2C) {
    if (!i2c0_check_master_enabled()) {
      return ERR_PORT_NOT_OPEN;
    } else {
      return ERR_NO_ERR;
    }
  }
}

bmp280_errCode bmp280_createCtrlByte(bmp280* sensor, uint8_t* controlByte) {
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

uint8_t bmp280_createConfigByte(bmp280* sensor, uint8_t* returnByte) {
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

bmp280_errCode bmp280_port_prep(bmp280* sensor) {
  if (sensor == NULL) {
    return ERR_SENSOR_UNITIALIZED;
  } else {
    BMP280_TRY_FUNC(bmp280_checkPortOpened(sensor));

    if (sensor->protocol == I2C) { i2c0_waitBusy(); }
    return ERR_NO_ERR;
  }
}

bmp280_errCode bmp280_get_one_register(bmp280*       sensor,
                                       const uint8_t regAddr,
                                       uint8_t*      registerData) {
  BMP280_TRY_FUNC(bmp280_port_prep(sensor));

  uint8_t ID;
  if (sensor->protocol == I2C) {
    i2c0_waitBusy();
    // write data with no stop signal
    i2c0_single_data_write(sensor->address, regAddr, 1);
    i2c0_waitBusy();

    // read 3 bytes bc reading single byte seems to create a bug
    uint8_t inputBuffer[3];
    i2c0_multiple_data_byte_read(sensor->address, inputBuffer, 3);
    inputBuffer[0] = *registerData;
  }
  return ERR_NO_ERR;
}

bmp280_errCode bmp280_get_multiple_register(bmp280*       sensor,
                                            const uint8_t startAddr,
                                            uint8_t*      regData,
                                            uint8_t       dataLen) {
  if (sensor->protocol == I2C) {
    i2c0_waitBusy();
    // write data with no stop signal
    i2c0_single_data_write(sensor->address, startAddr, 1);
    i2c0_multiple_data_byte_read(sensor->address, regData, dataLen);
  }
  return ERR_NO_ERR;
}
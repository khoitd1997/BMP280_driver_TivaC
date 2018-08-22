/**
 * Files provided auxilarry functions not called by the user
 */
#ifndef _BMP280_UTILS_H
#define _BMP280_UTILS_H
#include <stdint.h>

#include "include/BMP280_Drv.h"

#define BMP280_TRY_FUNC(funcToExecute)             \
  do {                                             \
    Bmp280ErrCode errCode;                         \
    errCode = funcToExecute;                       \
    if (errCode != ERR_NO_ERR) { return errCode; } \
  } while (0)

/* functions used for creating data byte to write to bmp280 based on bmp280
 settings */
Bmp280ErrCode bmp280_make_ctrl_byte(bmp280* sensor, uint8_t* controlByte);
uint8_t       bmp280_make_cfg_byte(bmp280* sensor, uint8_t* returnByte);

/* error checking */
// check for unitialized value in sensor settings
Bmp280ErrCode bmp280_check_setting(bmp280* sensor);
// check if port is opened
Bmp280ErrCode bmp280_port_check(bmp280* sensor);

/* port prepping, including error checking and prepping for data transfer return
 true for having errors typically call at beginning of any port operation */
Bmp280ErrCode bmp280_port_prep(bmp280* sensor);

Bmp280ErrCode bmp280_get_one_register(bmp280* sensor, const uint8_t regAddr, uint8_t* registerData);

Bmp280ErrCode bmp280_get_multiple_register(bmp280*       sensor,
                                           const uint8_t startAddr,
                                           uint8_t*      regData,
                                           const uint8_t dataLen);

#endif
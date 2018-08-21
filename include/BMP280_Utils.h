/**
 * Files provided auxilarry functions not called by the user
 */
#ifndef _BMP280_UTILS_H
#define _BMP280_UTILS_H
#include <stdint.h>

#include "include/BMP280_Drv.h"

#define BMP280_TRY_FUNC(funcToExecute)             \
  do {                                             \
    bmp280_errCode errCode;                        \
    errCode = funcToExecute;                       \
    if (errCode != ERR_NO_ERR) { return errCode; } \
  } while (0)

/* functions used for creating data byte to write to bmp280 based on bmp280
 settings */
bmp280_errCode bmp280_createCtrlByte(bmp280* sensor, uint8_t* controlByte);
uint8_t        bmp280_createConfigByte(bmp280* sensor, uint8_t* returnByte);

/* error checking */
// check for unitialized value in sensor settings
bmp280_errCode bmp280_check_setting(bmp280* sensor);
// check if port is opened
bmp280_errCode bmp280_checkPortOpened(bmp280* sensor);

/* port prepping, including error checking and prepping for data transfer return
 true for having errors typically call at beginning of any port operation */
bmp280_errCode bmp280_port_prep(bmp280* sensor);

bmp280_errCode bmp280_get_one_register(bmp280*       sensor,
                                       const uint8_t regAddr,
                                       uint8_t*      registerData);

#endif
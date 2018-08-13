/**
 * Files provided auxilarry functions not called by the user
 */
#ifndef _BMP280_UTILS_H
#define _BMP280_UTILS_H
#include <stdint.h>

#include "include/BMP280_Drv.h"

/* functions used for creating data byte to write to bmp280 based on bmp280
 settings */
uint8_t bmp280_createCtrlByte(bmp280* sensor, bmp280_errCode* errCode);
uint8_t bmp280_createConfigByte(bmp280* sensor, bmp280_errCode* errCode);

/* error checking */
// check for unitialized value in sensor settings
int bmp280_check_setting(bmp280* sensor, bmp280_errCode* errC);
// check if port is opened
int bmp280_checkPortOpened(bmp280* sensor, bmp280_errCode* errCode);

/* port prepping, including error checking and prepping for data transfer return
 true for having errors typically call at beginning of any port operation */
int bmp280_portPrep(bmp280* sensor, bmp280_errCode* errCode);

#endif
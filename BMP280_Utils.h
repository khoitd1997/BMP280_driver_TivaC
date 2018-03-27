/**
 * Files provided auxilarry functions not called by the user
 */
#ifndef _BMP280_UTILS_H
#define _BMP280_UTILS_H
#include <stdint.h>

#include "BMP280_Drv.h"

// functions used for creating data byte to write to based on given setting
uint8_t bmp280_bmp280_createCtrlByte(bmp280* sensor,
                       bmp280_errCode* errCode);
uint8_t bmp280_createConfigByte(bmp280* sensor, 
                         bmp280_errCode*    errCode);

// error checking
int bmp280_checkUnitialized(bmp280*         sensor, bmp280_errCode* errC);  // check for unitialized value in sensor settings
int bmp280_checkPortOpened(bmp280* sensor, bmp280_errCode* errCode);  // check if port is opened

// port prepping, including error checking and prepping for data transfer return true for having errors
int bmp280_portPrep(bmp280* sensor, bmp280_errCode* errCode);

#endif
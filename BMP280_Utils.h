/**
 * Files provided auxilarry functions not called by the user
 */
#ifndef _BMP280_UTILS_H
#define _BMP280_UTILS_H
#include <stdint.h>

#include "BMP280_Drv.h"

// functions used for creating data byte to write to based on given setting
uint8_t createCtrlByte(bmp280* sensor,
                       bmp280_errCode* errCode);
uint8_t createConfigByte(bmp280* sensor, 
                         bmp280_errCode*    errCode);

// error checking
int checkUnitialized(
    bmp280*         sensor,
    bmp280_errCode* errC);  // check for unitialized value in sensor settings
int checkPortOpened(bmp280*         sensor,
                    bmp280_errCode* errC);  // check if port is opened

#endif
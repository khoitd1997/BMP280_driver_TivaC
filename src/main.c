
/*
 * Written by Khoi Dinh Trinh on 12/13/2017
 * BMP280 breakout board from Adafruit:
 * https://www.adafruit.com/product/2651
 * Reference:
 * https://github.com/BoschSensortec/BMP280_driver
 * https://github.com/adafruit/Adafruit_BMP280_Library
 *
 * This file is used for testing the Tiva C with BMP280 BoschSensortec
 * API will be in a separate file
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "external/TivaC_Utils/include/TivaC_Other_Utils.h"
#include "include/BMP280_Drv.h"
#include "include/TivaC_I2C.h"

#define TRUE 1
#define FALSE 0

// testing macro area

// basic test
#define OUTPUT_BUFFER_LENGTH 4
#define INPUT_BUFFER_LENGTH 4
#define BMP280_CONFIG_ADDRESS 0xF4
#define BMP280_ADDR 0x77

// i2c test
#define I2C_TEST

// interrupt void interruptHandler(void);
// int main(void) {
//   pll_enable(80);

//   redled_init();

//   uint8_t bmp280_obuffer[OUTPUT_BUFFER_LENGTH];
//   uint8_t bmp280_ibuffer[INPUT_BUFFER_LENGTH];

//   bmp280_obuffer[0] = 0xF4;
//   bmp280_obuffer[1] = 0x01;
//   bmp280_obuffer[2] = 0xF5;
//   bmp280_obuffer[3] = 0x88;

// #ifdef BASIC_TEST

// // for (int output_index = 0; output_index < OUTPUT_BUFFER_LENGTH;
// //      ++output_index)
// //   {
// //     if(output_index%2){
// //       output[output_index]=
// //     }
// //     output[output_index] = 246 + output_index;
// //   }
// // i2c0_open();
// // i2c0_multiple_data_byte_write(BMP280_ADDR, bmp280_obuffer,
// // OUTPUT_BUFFER_LENGTH);
// #endif

// #ifdef I2C_TEST
//   bmp280         sensor280;
//   bmp280_errCode errCode;
//   bmp280_init(&sensor280, WeatherStat, I2C, &errCode);
//   i2c0_open();
// #endif

//   for (;;) {

// #ifdef BASIC_TEST
//     // i2c0_single_data_write(119, 246);
//     i2c0_open();
//     i2c0_single_data_write(BMP280_ADDR, 0xF6, REMAIN_TRANSMIT);
//     i2c0_multiple_data_byte_read(BMP280_ADDR, bmp280_ibuffer, INPUT_BUFFER_LENGTH);
//     // delayms(10);
//     // i2c0_close();
// #endif

// #ifdef I2C_TEST
//     i2c0_open();

//     i2c0_stop();

//     redled_on();
//     delayms(100);
//     bmp280_set_temp(&sensor280, x4, &errCode);
//     redled_off();
//     i2c0_close();
//     delayms(100);
// #endif
//   }
// }

/**************************** End of file ****************************/

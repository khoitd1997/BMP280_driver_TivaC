
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
#include "TM4C123.h"
#include "TivaC_GEN.h"
#include "TivaC_I2C.h"

#define TRUE 1
#define FALSE 0

#define OUTPUT_BUFFER_LENGTH 4
#define INPUT_BUFFER_LENGTH 4
#define BMP280_CONFIG_ADDRESS 0xF4
#define BMP280_ADDR 0x77
// interrupt void interruptHandler(void);
int main(void)
{
  pll_enable(80);
  uint8_t bmp280_obuffer[OUTPUT_BUFFER_LENGTH];
  uint8_t bmp280_ibuffer[INPUT_BUFFER_LENGTH];

  bmp280_obuffer[0] = 0xF4;
  bmp280_obuffer[1] = 0x01;
  bmp280_obuffer[2] = 0xF5;
  bmp280_obuffer[3] = 0x88;
  // for (int output_index = 0; output_index < OUTPUT_BUFFER_LENGTH;
  //      ++output_index)
  //   {
  //     if(output_index%2){
  //       output[output_index]=
  //     }
  //     output[output_index] = 246 + output_index;
  //   }
  // i2c0_open();
  // i2c0_multiple_data_byte_write(BMP280_ADDR, bmp280_obuffer,
  // OUTPUT_BUFFER_LENGTH);
  while (1)
    {
      // i2c0_single_data_write(119, 246);
      i2c0_open();
      i2c0_single_data_write(BMP280_ADDR, 0xF6, REMAIN_TRANSMIT);
      i2c0_multiple_data_byte_read(BMP280_ADDR, bmp280_ibuffer, INPUT_BUFFER_LENGTH);
      delayms(10);
      // i2c0_close();
    }
}

/*************************** End of file ****************************/


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

#define OUTPUT_BUFFER_LENGTH 10

// interrupt void interruptHandler(void);
int main(void)
{
  pll_enable(80);
  uint8_t output[OUTPUT_BUFFER_LENGTH];
  for (int output_index = 0,
           output_index < OUTPUT_BUFFER_LENGTH,
           ++output_index)
    {
      output[output_index] = 246 + output_index;
    }
  i2c0_open();
  while (1)
    {
      i2c0_multiple_data_byte_write(119, output, OUTPUT_BUFFER_LENGTH);
      delayms(500);
      // i2c0_close();
    }
}

/*************************** End of file ****************************/

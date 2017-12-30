
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

// interrupt void interruptHandler(void);
int main(void)
{
  pll_enable(80);

  while (1)
    {
      delayms(500);
    }
}

/*************************** End of file ****************************/

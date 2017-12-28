/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co. KG                 *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 2014 - 2017  SEGGER Microcontroller GmbH & Co. KG        *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Generic application start


/**
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



#include <stdio.h>
#include <stdlib.h>

/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.
*/
void main(void) {
  int i;

  for (i = 0; i < 100; i++) {
    printf("Hello World %d!\n", i);
  }
  do {
    i++;
  } while (1);
}

/*************************** End of file ****************************/
dsadasd
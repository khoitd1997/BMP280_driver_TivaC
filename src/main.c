/**
 * @brief used for testing and also serve as an example
 *
 * @file main.c
 * @author Khoi Trinh
 * @date 2018-08-25
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "external/TivaC_Utils/include/TivaC_LED.h"
#include "external/TivaC_Utils/include/TivaC_Other_Utils.h"
#include "include/BMP280_Drv.h"
#include "include/BMP280_Ware.h"
#include "include/TivaC_I2C.h"

#define BMP280_ADDR 0x77

int main(void) {
  bmp280           sensor280;
  Bmp280CalibParam calibParam;
  float            temperature;
  float            pressure;
  uint8_t          ID;

  /*Prepare the ports as well as data structure for data BMP280 operation*/

  bmp280_create_predefined_settings(&sensor280, HandDynamic);
  bmp280_init(&sensor280, I2C, BMP280_ADDR);
  bmp280_open(&sensor280);
  bmp280_reset(&sensor280);
  bmp280_get_calibration_data(&sensor280, &calibParam);
  bmp280_update_setting(&sensor280);

  for (;;) {
    printf("\n------------------------------");
    bmp280_get_temp_press(&sensor280, &temperature, &pressure, calibParam);
    printf("\nTemperature is %f, Pressure is %f", temperature, pressure);

    bmp280_get_id(&sensor280, &ID);
    printf("\nThe ID is %d", ID);
    delayms(1000);
  }
}

/**************************** End of file ****************************/

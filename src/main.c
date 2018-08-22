
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

#include "external/TivaC_Utils/include/TivaC_LED.h"
#include "external/TivaC_Utils/include/TivaC_Other_Utils.h"
#include "include/BMP280_Drv.h"
#include "include/BMP280_Ware.h"
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

int main(void) {
  bmp280           sensor280;
  Bmp280ErrCode    errCode;
  Bmp280CalibParam calibParam;
  float            temperature;
  float            pressure;
  uint8_t          ID;
  uint8_t          ctrlMeas;
  uint8_t          configReturn;

  bmp280_create_predefined_settings(&sensor280, WeatherStat);
  bmp280_init(&sensor280, I2C, 0x77);
  bmp280_open(&sensor280);
  bmp280_reset(&sensor280);
  delayms(5);
  bmp280_get_calibration_data(&sensor280, &calibParam);
  bmp280_update_setting(&sensor280);
  greenled_init();
  greenled_on();

  for (;;) {
    // bmp280_get_ctr_meas(&sensor280, &ctrlMeas);
    // bmp280_get_config(&sensor280, &configReturn);
    // bmp280_get_calibration_data(&sensor280, &calibParam);
    bmp280_get_temp_press(&sensor280, &temperature, &pressure, calibParam);
    printf("\nTemperature is %f, Pressure is %f", temperature, pressure);
    delayms(500);

    // bmp280_reset(&sensor280);
    // delayms(30);
    // bmp280_get_id(&sensor280, &ID);
    // bmp280_get_ctr_meas(&sensor280, &ctrlMeas);
    delayms(30);
    // bmp280_update_setting(&sensor280);
    // delayms(30);
    // bmp280_get_ctr_meas(&sensor280, &ctrlMeas);
    // bmp280_get_id(&sensor280, &ID);
  }
}

/**************************** End of file ****************************/

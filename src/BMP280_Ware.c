/**
 * Copyright (C) 2017 - 2018 Bosch Sensortec GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holder nor the names of the
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 * The information provided is believed to be accurate and reliable.
 * The copyright holder assumes no responsibility
 * for the consequences of use
 * of such information nor for any infringement of patents or
 * other rights of third parties which may result from its use.
 * No license is granted by implication or otherwise under any patent or
 * patent rights of the copyright holder.
 *
 *
 */

#include "include/BMP280_Ware.h"

#include <stdint.h>

// Returns rawCalibDataerature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23
// DegC. calData.t_fine carries fine rawCalibDataerature as global value
int32_t bmp280_compensate_T_int32(int32_t adc_T, const bmp280_calib_param calData) {
  int32_t var1, var2, T;
  var1 = ((((adc_T >> 3) - ((int32_t)calData.dig_t1 << 1))) * ((int32_t)calData.dig_t2)) >> 11;
  var2 =
      (((((adc_T >> 4) - ((int32_t)calData.dig_t1)) * ((adc_T >> 4) - ((int32_t)calData.dig_t1))) >>
        12) *
       ((int32_t)calData.dig_t3)) >>
      14;
  calData.t_fine = var1 + var2;
  T              = (calData.t_fine * 5 + 128) >> 8;
  return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8
// fractional bits). Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
uint32_t bmp280_compensate_P_int64(int32_t adc_P, const bmp280_calib_param calData) {
  int64_t var1, var2, p;
  var1 = ((int64_t)calData.t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)calData.dig_p6;
  var2 = var2 + ((var1 * (int64_t)calData.dig_p5) << 17);
  var2 = var2 + (((int64_t)calData.dig_p4) << 35);
  var1 = ((var1 * var1 * (int64_t)calData.dig_p3) >> 8) + ((var1 * (int64_t)calData.dig_p2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)calData.dig_p1) >> 33;
  if (var1 == 0) {
    return 0;  // avoid exception caused by division by zero
  }
  p    = 1048576 - adc_P;
  p    = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)calData.dig_p9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)calData.dig_p8) * p) >> 19;
  p    = ((p + var1 + var2) >> 8) + (((int64_t)calData.dig_p7) << 4);
  return (uint32_t)p;
}

int8_t get_calib_param(uint8_t rawCalibData, bmp280_calib_param *outputParam) {
  outputParam->dig_t1 = (uint16_t)(((uint16_t)rawCalibData[BMP280_DIG_T1_MSB_POS] << 8) |
                                   ((uint16_t)rawCalibData[BMP280_DIG_T1_LSB_POS]));
  outputParam->dig_t2 = (int16_t)(((int16_t)rawCalibData[BMP280_DIG_T2_MSB_POS] << 8) |
                                  ((int16_t)rawCalibData[BMP280_DIG_T2_LSB_POS]));
  outputParam->dig_t3 = (int16_t)(((int16_t)rawCalibData[BMP280_DIG_T3_MSB_POS] << 8) |
                                  ((int16_t)rawCalibData[BMP280_DIG_T3_LSB_POS]));
  outputParam->dig_p1 = (uint16_t)(((uint16_t)rawCalibData[BMP280_DIG_P1_MSB_POS] << 8) |
                                   ((uint16_t)rawCalibData[BMP280_DIG_P1_LSB_POS]));
  outputParam->dig_p2 = (int16_t)(((int16_t)rawCalibData[BMP280_DIG_P2_MSB_POS] << 8) |
                                  ((int16_t)rawCalibData[BMP280_DIG_P2_LSB_POS]));
  outputParam->dig_p3 = (int16_t)(((int16_t)rawCalibData[BMP280_DIG_P3_MSB_POS] << 8) |
                                  ((int16_t)rawCalibData[BMP280_DIG_P3_LSB_POS]));
  outputParam->dig_p4 = (int16_t)(((int16_t)rawCalibData[BMP280_DIG_P4_MSB_POS] << 8) |
                                  ((int16_t)rawCalibData[BMP280_DIG_P4_LSB_POS]));
  outputParam->dig_p5 = (int16_t)(((int16_t)rawCalibData[BMP280_DIG_P5_MSB_POS] << 8) |
                                  ((int16_t)rawCalibData[BMP280_DIG_P5_LSB_POS]));
  outputParam->dig_p6 = (int16_t)(((int16_t)rawCalibData[BMP280_DIG_P6_MSB_POS] << 8) |
                                  ((int16_t)rawCalibData[BMP280_DIG_P6_LSB_POS]));
  outputParam->dig_p7 = (int16_t)(((int16_t)rawCalibData[BMP280_DIG_P7_MSB_POS] << 8) |
                                  ((int16_t)rawCalibData[BMP280_DIG_P7_LSB_POS]));
  outputParam->dig_p8 = (int16_t)(((int16_t)rawCalibData[BMP280_DIG_P8_MSB_POS] << 8) |
                                  ((int16_t)rawCalibData[BMP280_DIG_P8_LSB_POS]));
  outputParam->dig_p9 = (int16_t)(((int16_t)rawCalibData[BMP280_DIG_P9_MSB_POS] << 8) |
                                  ((int16_t)rawCalibData[BMP280_DIG_P9_LSB_POS]));

  return 0;
}
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

#ifndef _BMP280_WARE_H
#define _BMP280_WARE_H

#include <stdint.h>

/*! @name Calibration parameters' structure */
typedef struct {
  uint16_t dig_t1;
  int16_t  dig_t2;
  int16_t  dig_t3;
  uint16_t dig_p1;
  int16_t  dig_p2;
  int16_t  dig_p3;
  int16_t  dig_p4;
  int16_t  dig_p5;
  int16_t  dig_p6;
  int16_t  dig_p7;
  int16_t  dig_p8;
  int16_t  dig_p9;
  int32_t  t_fine;
} Bmp280CalibParam;

float bmp280_compensate_T_int32(int32_t adc_T, Bmp280CalibParam* calData);
float bmp280_compensate_P_int64(int32_t adc_P, Bmp280CalibParam* calData);

#define BMP280_DIG_T1_LSB_POS UINT8_C(0)
#define BMP280_DIG_T1_MSB_POS UINT8_C(1)
#define BMP280_DIG_T2_LSB_POS UINT8_C(2)
#define BMP280_DIG_T2_MSB_POS UINT8_C(3)
#define BMP280_DIG_T3_LSB_POS UINT8_C(4)
#define BMP280_DIG_T3_MSB_POS UINT8_C(5)
#define BMP280_DIG_P1_LSB_POS UINT8_C(6)
#define BMP280_DIG_P1_MSB_POS UINT8_C(7)
#define BMP280_DIG_P2_LSB_POS UINT8_C(8)
#define BMP280_DIG_P2_MSB_POS UINT8_C(9)
#define BMP280_DIG_P3_LSB_POS UINT8_C(10)
#define BMP280_DIG_P3_MSB_POS UINT8_C(11)
#define BMP280_DIG_P4_LSB_POS UINT8_C(12)
#define BMP280_DIG_P4_MSB_POS UINT8_C(13)
#define BMP280_DIG_P5_LSB_POS UINT8_C(14)
#define BMP280_DIG_P5_MSB_POS UINT8_C(15)
#define BMP280_DIG_P6_LSB_POS UINT8_C(16)
#define BMP280_DIG_P6_MSB_POS UINT8_C(17)
#define BMP280_DIG_P7_LSB_POS UINT8_C(18)
#define BMP280_DIG_P7_MSB_POS UINT8_C(19)
#define BMP280_DIG_P8_LSB_POS UINT8_C(20)
#define BMP280_DIG_P8_MSB_POS UINT8_C(21)
#define BMP280_DIG_P9_LSB_POS UINT8_C(22)
#define BMP280_DIG_P9_MSB_POS UINT8_C(23)
#define BMP280_CALIB_DATA_SIZE UINT8_C(24)

int8_t bmp280_get_calib_param(uint8_t* rawCalibData, Bmp280CalibParam* outputParam);

#endif
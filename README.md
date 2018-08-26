# BMP280 Driver using TivaC

Library used for interacting the bmp280 with tivaC tm4c123gh6pm processor, the dev board used is the TivaC line of Texas Instrument

The bmp280 board used is from Adafruit: https://www.adafruit.com/product/2651

The project was developed using Segger Embedded Studio but you should be able to use other IDE or build system with it

## Features

- Read and calculate compensated temperature and pressure data
- Read and write settings
- Create manufacturer-defined settings or custom settings
- Available in both I2C and SPI

## Style Guide

- /\*\*/ comments indicate a section comments, meaning it apply to all things below it up until the next /**/
- // indicate a specific comment, it refers to the one thing directly below it or before it in the same line

## Naming Convention

- Error code begins with ERR_
- bmp280 function begins with bmp280_
- i2c0 function begins with i2c0_
- spi function begins with spi_
- All enum member at least has its first character in name capitalized

## Folder structure

- src/: driver .c files in here
- include/: .h file here
- external/: Dependencies here, for example git submodules are here

## Code structure

The code is divided into many layers:

- BMP280_Drv files: the front layers, their actions are BMP280 speicifc but doesn't deal directly with SPI or I2C
- BMP280_Utils: contain utilities functions as well as dealing directly with the I2C and SPI, the glue layer between BMP280_Drv and low layer functions
- TivaC_SPI related files: Contain SPI functions for SPI0 modules of TivaC, the code is hardocded to use module 0, the CS pin hardcoded to be pin 3 of port A on the TivaC board
- TivaC_I2C related files: Contain TivaC functions to work with I2C0 modules of TivaC, hard coded to use I2C0
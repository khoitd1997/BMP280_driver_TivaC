# BMP280 Driver using TivaC

Library used for interacting the bmp280 with tivaC tm4c123gh6pm processor, the dev board used is the TivaC line of Texas Instrument

The bmp280 board used is from Adafruit: https://www.adafruit.com/product/2651

Some API for compensating and collecting raw data was borrowed from Bosch: https://github.com/BoschSensortec/BMP280_driver

The project was developed using Segger Embedded Studio but you should be able to use other IDE or build system with it

## Documentation

[Doxygen  Docs]()

## Features

- Read and calculate compensated temperature and pressure data
- Read and write settings
- Read the status of the sensor
- Create manufacturer-defined settings or custom settings
- Available in both I2C and SPI

## Dependencies

The dependency is in the form of the git submodules of one of my other repo, just initialize and pull that sub module and you should be good, you can also write your own

## Folder structure

- src/: driver .c files in here
- include/: .h file here
- external/: Dependencies go here, for example git submodules are here
- docs/: doxygen generated docs

## Code structure

The code is divided into many layers:

- BMP280_Drv files: the front layers, their actions are BMP280 specifc but doesn't deal directly with SPI or I2C and thus agnostic to the protocol
- BMP280_Ware files: contain API derived from Bosch source code
- BMP280_Utils: contain utilities functions for BMP280_Drv as well as dealing directly with the I2C and SPI, this is the glue layer between BMP280_Drv and low layer communication functions
- TivaC_SPI related files: Contain SPI functions for SPI0 modules of TivaC, the code is hardocded to use module 0, the CS pin hardcoded to be pin 3 of port A on the TivaC board
- TivaC_I2C related files: Contain TivaC functions and their utilities funcs to work with I2C0 modules of TivaC, hard coded to use I2C0

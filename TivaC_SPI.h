#ifndef _TIVA_C_SPI_H
#define _TIVA_C_SPI_H
#include <stdint.h>

/* Note that this assumes we are using FreeScale SPI (so 4-wire) */

typedef enum
{

} spi_errCode;

typedef struct 
{
    uint8_t cpol;
    uint8_t cpha; 
    uint8_t isFourWire;
    uint8_t isMaster;
    // TODO: maybe add interrupts support
} spi_settings;

/* Communication setup */
void spi_open(const spi_settings* setting, spi_errCode* errCode); // setup all necessary register, but don't start communication until spi_transfer is called
void spi_close(spi_errCode* errCode); // disable the spi controller and clean up register

/* Data Transfer */
void spi_transfer(uint8_t* data, uint8_t dataLength); // used for both single and multiple bytes


#endif
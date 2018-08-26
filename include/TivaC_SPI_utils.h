#ifndef _TIVAC_SPI_UTILS_H
#define _TIVAC_SPI_UTILS_H

#include <stdint.h>
#include "include/TivaC_SPI.h"
#include "include/tm4c123gh6pm.h"

/* Error Checking */
SpiErrCode spi_check_setting(
    const SpiSettings setting);  // make sure readings are properly initialized
SpiErrCode spi_check_rx_full(void);
SpiErrCode spi_check_tx_full(void);
SpiErrCode spi_check_rx_not_empty(void);

/* Calculation */
SpiErrCode spi_calc_clock_prescalc(const SpiSettings setting, uint8_t* preScalc, uint8_t* scr);

/* Common Utility Action */
SpiErrCode spi_bus_wait(void);
void       spi_enable_spi(void);
void       spi_disable_spi(void);
void       spi_pull_cs_low(void);
void       spi_pull_cs_high(void);
void       spi_send_dummy_byte(void);  // used to stretching communication
void       spi_data_delay(void);       // delay between receiving and reading the data

/* Protocol Handling */
SpiErrCode spi_rx_one_data_unit(const SpiSettings setting, uint8_t* totalByteRxed, uint8_t* dataRx);

SpiErrCode spi_tx_one_data_unit(const uint8_t  transferSize,
                                uint8_t*       totalByteTxed,
                                const uint8_t* dataTx);
void       spi_clear_rx_buffer(void);
#endif
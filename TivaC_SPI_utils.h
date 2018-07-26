#ifndef _TIVAC_SPI_UTILS_H
#define _TIVAC_SPI_UTILS_H

#include <stdint.h>
#include "TivaC_SPI.h"
#include "tm4c123gh6pm.h"

/* Error Checking */                                        // wait until spi bus is done
spi_errCode spi_check_setting(const spi_settings setting);  // make sure readings are initialized
spi_errCode spi_check_rx_full(void);
spi_errCode spi_check_tx_full(void);
spi_errCode spi_check_rx_not_empty(void);

/* Calculation */
spi_errCode spi_calc_clock_prescalc(const spi_settings setting, uint8_t* preScalc, uint8_t* scr);

/* Common Utility Action */
void spi_wait_busy(void);
void spi_enable_spi(void);
void spi_disable_spi(void);

/* Protocol Handling */
spi_errCode spi_Rx_one_data_unit(const spi_settings setting, uint8_t* totalBitRx, uint16_t* dataRx);

spi_errCode spi_tx_one_data_unit(const spi_settings setting,
                                 uint8_t*           totalBitTx,
                                 const uint16_t*    dataTx);
#endif
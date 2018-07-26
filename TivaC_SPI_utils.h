#ifndef _TIVAC_SPI_UTILS_H
#define _TIVAC_SPI_UTILS_H

#include <stdint.h>
#include "TivaC_SPI.h"
#include "tm4c123gh6pm.h"

/* Utility Function */
spi_errCode spi_wait_busy(void);                            // wait until spi bus is done
spi_errCode spi_check_setting(const spi_settings setting);  // make sure readings are initialized
spi_errCode spi_check_rx_full(void);
spi_errCode spi_check_tx_full(void);
spi_errCode spi_check_rx_not_empty(void);
spi_errCode spi_calc_clock_prescalc(const spi_settings setting, uint8_t* preScalc, uint8_t* scr);

spi_errCode spi_recv_one_data_unit(const spi_settings setting,
                                   uint8_t*           totalBitRecv,
                                   uint16_t*          dataRecv);

spi_errCode spi_send_one_data_unit(const spi_settings setting,
                                   uint8_t*           totalBitSend,
                                   const uint16_t*    dataSend);
#endif
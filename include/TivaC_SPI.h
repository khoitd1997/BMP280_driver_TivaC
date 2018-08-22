#ifndef _TIVA_C_SPI_H
#define _TIVA_C_SPI_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief the smallest transfer size of the spi, note that this is different from the transfer size
 * in the spi structure, a complete transfer is made up of small 8 bit transfers
 *
 */
#define SPI_TRF_SIZE 8

typedef enum {
  ERR_NO_ERR,
  ERR_SPI_CPA_UNDEFINED,
  ERR_RX_FULL,
  ERR_TX_FULL,
  ERR_RX_EMPTY,
  ERR_INVAL_SYS_CLK_RATE,
  ERR_NO_VAL_PRESCALC,
  ERR_INVAL_BIT_RATE,
  ERR_INVAL_DATA_SIZE,
  ERR_INVAL_CPOL,
  ERR_INVAL_CPHA,
  ERR_INVAL_PROTOCOL,
  ERR_INVAL_ROLE,
  ERR_INVAL_OPERMODE,
  ERR_INVAL_CLOCKSOURCE
} SpiErrCode;

typedef enum { Freescale, Tissf, Microwire } SpiProtocolMode;
typedef enum { Slave, Master } SpiRole;
typedef enum { Systemclock, Piosc } ClockSource;

typedef enum { Tx, Rx, Both } SpiTransferMode;
typedef struct {
  bool            enableDMA;
  float           spiBitRateMbits;
  float           cpuClockMHz;
  uint8_t         cpol;
  uint8_t         cpha;
  SpiProtocolMode operMode;
  bool            isLoopBack;
  uint8_t         transferSizeBit;  // how much to tx/rx per SPI transfer
  SpiRole         role;
  ClockSource     clockSource;
  // TODO: maybe add interrupts support
} SpiSettings;

/* Communication setup */
SpiErrCode spi_open(const SpiSettings setting);  // setup all necessary register, but don't
                                                 // start communication until spi_transfer
                                                 // is called
// clean reg and disable SSI
// calling this will disable clock for spi and change all
// spi pins to general digital pins
SpiErrCode spi_close(void);

/* Data Transfer */
// used for both single and multiple bytes
// used for both tx receive
SpiErrCode spi_transfer(const SpiSettings setting,
                        uint8_t*          dataTx,
                        const uint8_t     dataTxLenByte,
                        uint8_t*          dataRx,
                        const uint8_t     dataRxLenByte);

#endif
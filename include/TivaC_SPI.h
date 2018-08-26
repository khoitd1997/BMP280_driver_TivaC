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

#define SPI_TRY_FUNC(funcToExecute)                    \
  do {                                                 \
    SpiErrCode errCode = funcToExecute;                \
    if (errCode != SPI_ERR_NO_ERR) { return errCode; } \
  } while (0)

/**
 * SPI error code
 *
 */
typedef enum {
  SPI_ERR_NO_ERR,
  SPI_ERR_DISABLED,
  SPI_ERR_TIMEOUT,
  SPI_ERR_SPI_CPA_UNDEFINED,
  SPI_ERR_RX_FULL,
  SPI_ERR_TX_FULL,
  SPI_ERR_RX_EMPTY,
  SPI_ERR_INVAL_SYS_CLK_RATE,
  SPI_ERR_NO_VAL_PRESCALC,
  SPI_ERR_INVAL_BIT_RATE,
  SPI_ERR_INVAL_DATA_SIZE,
  SPI_ERR_INVAL_CPOL,
  SPI_ERR_INVAL_CPHA,
  SPI_ERR_INVAL_PROTOCOL,
  SPI_ERR_INVAL_ROLE,
  SPI_ERR_INVAL_OPERMODE,
  SPI_ERR_INVAL_CLOCKSOURCE
} SpiErrCode;

typedef enum { Freescale, Tissf, Microwire } SpiProtocolMode;
typedef enum { Slave, Master } SpiRole;
typedef enum { Systemclock, Piosc } ClockSource;

/**
 * represent a spi module
 *
 */
typedef struct {
  float           spiBitRateMbits;
  float           cpuClockMHz;
  uint8_t         cpol;
  uint8_t         cpha;
  SpiProtocolMode operMode;
  bool            isLoopBack;
  uint8_t         transferSizeBit;  // how much to tx/rx per SPI transfer
  SpiRole         role;
  ClockSource     clockSource;
} SpiSettings;

/* Communication setup */
// setup all necessary register, but don't
// start communication until spi_transfer
// is called
SpiErrCode spi_open(const SpiSettings setting);

// clean reg and disable SSI
// calling this will disable clock for spi
SpiErrCode spi_close(void);

SpiErrCode spi_check_spi_enabled(void);

/* Data Transfer */
// used for both single and multiple bytes
// used for both tx and rx
SpiErrCode spi_transfer(const SpiSettings setting,
                        uint8_t*          dataTx,
                        const uint8_t     dataTxLenByte,
                        uint8_t*          dataRx,
                        const uint8_t     dataRxLenByte);

#endif
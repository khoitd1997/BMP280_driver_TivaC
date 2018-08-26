#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "external/TivaC_Utils/include/TivaC_Other_Utils.h"
#include "external/TivaC_Utils/include/bit_manipulation.h"
#include "external/TivaC_Utils/include/tm4c123gh6pm.h"
#include "include/TivaC_SPI.h"
#include "include/TivaC_SPI_utils.h"

#define MAX_TIVAC_CLOCK 80
#define MAX_SCR 255      // max SSI serial clock rate
#define MAX_CPSDVSR 254  // max clock pre scaler
#define SPI_TIMEOUT_COUNTER 500

SpiErrCode spi_bus_wait(void) {
  uint8_t timeoutCounter = 0;
  while (SSI0_SR_R & SSI_SR_BSY) {
    ++timeoutCounter;
    if ((timeoutCounter) > SPI_TIMEOUT_COUNTER) { return SPI_ERR_TIMEOUT; }
  }
  return SPI_ERR_NO_ERR;
}

SpiErrCode spi_check_rx_full(void) {
  return ((SSI0_SR_R & SSI_SR_RFF) ? SPI_ERR_RX_FULL : SPI_ERR_NO_ERR);
}

SpiErrCode spi_check_tx_full(void) {
  return (SSI0_SR_R & SSI_SR_TNF) ? SPI_ERR_NO_ERR : SPI_ERR_TX_FULL;
}

SpiErrCode spi_calc_clock_prescalc(const SpiSettings setting, uint8_t* preScalc, uint8_t* scr) {
  uint8_t errCode;
  if ((errCode = spi_check_setting(setting)) != SPI_ERR_NO_ERR) { return errCode; }
  assert(preScalc);
  assert(scr);

  uint8_t tempProduct = (setting.cpuClockMHz) / (setting.spiBitRateMbits);
  uint8_t CPSDVSR;

  for (uint16_t SCR = 1; SCR <= MAX_SCR; ++SCR) {
    CPSDVSR = tempProduct / (1 + SCR);
    if (CPSDVSR <= MAX_CPSDVSR && (CPSDVSR % 2 == 0)) {
      *preScalc = CPSDVSR;
      *scr      = SCR;
      return SPI_ERR_NO_ERR;
    }
  }
  return SPI_ERR_NO_VAL_PRESCALC;
}

SpiErrCode spi_check_setting(const SpiSettings setting) {
  if (setting.cpuClockMHz <= 0 || setting.cpuClockMHz > MAX_TIVAC_CLOCK) {
    return SPI_ERR_INVAL_SYS_CLK_RATE;
  }
  if (setting.spiBitRateMbits <= 0 || setting.spiBitRateMbits > 2) {
    return SPI_ERR_INVAL_BIT_RATE;
  }
  if (setting.transferSizeBit > 16 || setting.transferSizeBit < 4) {
    return SPI_ERR_INVAL_DATA_SIZE;
  }
  if (setting.cpol > 1 || setting.cpol < 0) { return SPI_ERR_INVAL_CPOL; }
  if (setting.cpha > 1 || setting.cpha < 0) { return SPI_ERR_INVAL_CPHA; }
  if (setting.role != Master && setting.role != Slave) { return SPI_ERR_INVAL_ROLE; }
  if (setting.operMode != Freescale && setting.operMode != Tissf && setting.operMode != Microwire) {
    return SPI_ERR_INVAL_OPERMODE;
  }
  if (setting.clockSource != Piosc && setting.clockSource != Systemclock) {
    return SPI_ERR_INVAL_CLOCKSOURCE;
  }
  return SPI_ERR_NO_ERR;
}

SpiErrCode spi_check_rx_not_empty(void) {
  return ((SSI0_SR_R & SSI_SR_RNE) ? SPI_ERR_NO_ERR : SPI_ERR_RX_EMPTY);
}

SpiErrCode spi_rx_one_data_unit(const SpiSettings setting,
                                uint8_t*          totalByteRxed,
                                uint8_t*          dataRx) {
  spi_bus_wait();
  if (SPI_ERR_NO_ERR == spi_check_rx_not_empty()) {
    dataRx[*totalByteRxed] = ((SSI0_DR_R)&SSI_DR_DATA_M) >> (SSI_DR_DATA_S);
    *totalByteRxed         = *totalByteRxed + 1;
  }
  return SPI_ERR_NO_ERR;
}

// TODO: refactor this
SpiErrCode spi_tx_one_data_unit(const uint8_t  transferSize,
                                uint8_t*       totalByteTxed,
                                const uint8_t* dataTx) {
  if (SPI_ERR_NO_ERR == spi_check_tx_full()) {
    spi_bus_wait();
    SSI0_DR_R      = (uint8_t)(dataTx[*totalByteTxed]) << (transferSize - SPI_TRF_SIZE);
    *totalByteTxed = *totalByteTxed + 1;
    return SPI_ERR_NO_ERR;
  } else {
    return SPI_ERR_TX_FULL;
  }
  spi_bus_wait();
}

void __attribute__((optimize("O0"))) spi_clear_rx_buffer(void) {
  // read until all data is gone

  while (SPI_ERR_NO_ERR == spi_check_rx_not_empty()) {
    // clear the reg by reading it
    SSI0_DR_R + 2;
  }
}

void spi_enable_spi(void) {
  bit_set(SSI0_CR1_R, SSI_CR1_SSE);
  while (0 == bit_get(SYSCTL_PRSSI_R, SYSCTL_PRSSI_R0)) {
    // wait until the SSI is ready to be accessed
  }
}

void spi_send_dummy_byte(void) {
  spi_bus_wait();
  SSI0_DR_R = 0;  // used to stretch the transfer until all the data is received
  spi_bus_wait();
}

void spi_disable_spi(void) { bit_clear(SSI0_CR1_R, SSI_CR1_SSE); }
void spi_pull_cs_low(void) { bit_clear(GPIO_PORTA_DATA_R, 0x8); }
void spi_pull_cs_high(void) { bit_set(GPIO_PORTA_DATA_R, 0x8); }

// from the time when the SPi module receives data till it puts the data available for read, there
// is really short delay
void __attribute__((optimize("O0"))) spi_data_delay(void) {
  for (int waitIndex = 0; waitIndex < 50; ++waitIndex) {
    // wait loop
  }
}
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

void spi_wait_busy(void) {
  while (SSI0_SR_R & SSI_SR_BSY) {
    // wait until the busy bit is unset
  }
}

spi_errCode spi_check_rx_full(void) { return (SSI0_SR_R & SSI_SR_RFF) ? ERR_RX_FULL : ERR_NO_ERR; }

spi_errCode spi_check_tx_full(void) { return (SSI0_SR_R & SSI_SR_TNF) ? ERR_NO_ERR : ERR_TX_FULL; }

spi_errCode spi_calc_clock_prescalc(const spi_settings setting, uint8_t* preScalc, uint8_t* scr) {
  uint8_t errCode;
  if ((errCode = spi_check_setting(setting)) != ERR_NO_ERR) { return errCode; }
  assert(preScalc);
  assert(scr);

  uint8_t tempProduct = (setting.cpuClockMHz) / (setting.spiBitRateMbits);
  uint8_t CPSDVSR;

  for (uint16_t SCR = 1; SCR <= MAX_SCR; ++SCR) {
    CPSDVSR = tempProduct / (1 + SCR);
    if (CPSDVSR <= MAX_CPSDVSR && (CPSDVSR % 2 == 0)) {
      *preScalc = CPSDVSR;
      *scr      = SCR;
      return ERR_NO_ERR;
    }
  }
  return ERR_NO_VAL_PRESCALC;
}

spi_errCode spi_check_setting(const spi_settings setting) {
  if (setting.cpuClockMHz <= 0 || setting.cpuClockMHz > MAX_TIVAC_CLOCK) {
    return ERR_INVAL_SYS_CLK_RATE;
  }
  if (setting.spiBitRateMbits <= 0 || setting.spiBitRateMbits > 2) { return ERR_INVAL_BIT_RATE; }
  if (setting.transferSizeBit > 16 || setting.transferSizeBit < 4) { return ERR_INVAL_DATA_SIZE; }
  if (setting.cpol > 1 || setting.cpol < 0) { return ERR_INVAL_CPOL; }
  if (setting.cpha > 1 || setting.cpha < 0) { return ERR_INVAL_CPHA; }
  if (setting.role != Master && setting.role != Slave) { return ERR_INVAL_ROLE; }
  if (setting.operMode != Freescale && setting.operMode != Tissf && setting.operMode != Microwire) {
    return ERR_INVAL_OPERMODE;
  }
  if (setting.clockSource != Piosc && setting.clockSource != Systemclock) {
    return ERR_INVAL_CLOCKSOURCE;
  }
  return ERR_NO_ERR;
}

spi_errCode spi_check_rx_not_empty(void) {
  return (SSI0_SR_R & SSI_SR_RNE) ? ERR_NO_ERR : ERR_RX_EMPTY;
}

spi_errCode spi_rx_one_data_unit(const spi_settings setting,
                                 uint8_t*           totalByteRxed,
                                 uint8_t*           dataRx) {
  GPIO_PORTA_DATA_R &= ~0x8;  // pull CS low to signal ready to read
  spi_enable_spi();           // begin SPI operation
  delayms(5);

  if (spi_check_rx_not_empty() == ERR_NO_ERR) {
    spi_wait_busy();
    uint8_t tempRxData     = SSI0_DR_R & 0x000000FF;
    dataRx[*totalByteRxed] = tempRxData;
    *totalByteRxed         = *totalByteRxed + 1;
  }
  return ERR_NO_ERR;
}

// TODO: refactor this
spi_errCode spi_tx_one_data_unit(const spi_settings setting,
                                 uint8_t*           totalByteTxed,
                                 const uint8_t*     dataTx) {
  uint8_t tempTxData = 0;

  // prep the data to be sent
  if (spi_check_tx_full() == ERR_NO_ERR) {
    tempTxData     = dataTx[*totalByteTxed];
    *totalByteTxed = *totalByteTxed + 1;
  }
  spi_wait_busy();
  SSI0_DR_R = tempTxData;  // shift so the data is in MSB to be sent first
  delayms(5);
  spi_pull_cs_low();
  spi_enable_spi();
  spi_wait_busy();
  return ERR_NO_ERR;
}

void spi_enable_spi(void) { SSI0_CR1_R |= SSI_CR1_SSE; }

void spi_disable_spi(void) { SSI0_CR1_R &= ~SSI_CR1_SSE; }

void spi_pull_cs_low(void) { bit_clear(GPIO_PORTA_DATA_R, 0x8); }
void spi_pull_cs_high(void) { bit_set(GPIO_PORTA_DATA_R, 0x8); }
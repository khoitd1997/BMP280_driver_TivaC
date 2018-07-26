#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "TivaC_SPI.h"
#include "TivaC_SPI_utils.h"
#include "tm4c123gh6pm.h"

#define SPI_MAX_CONTROLLER 3

spi_errCode spi_wait_busy(void) {
  while (SSI0_SR_R & SSI_SR_BSY) {
    // wait until the busy bit is unset
  }
  return ERR_NO_ERR;
}

spi_errCode spi_check_rx_full(void) {
  return (SSI0_SR_R & SSI_SR_RFF) ? ERR_RX_FULL : ERR_NO_ERR;
}

spi_errCode spi_check_tx_full(void) {
  return (SSI0_SR_R & SSI_SR_TNF) ? ERR_NO_ERR : ERR_TX_FULL;
}

spi_errCode spi_calc_clock_prescalc(const spi_settings setting, uint8_t* preScalc, uint8_t* scr) {
  if (spi_check_setting(setting) != ERR_NO_ERR) { return spi_check_setting(setting); }
  assert(preScalc);
  assert(scr);

  uint8_t tempProduct = (setting.cpuClockMHz) / (setting.spiBitRateMbits);
  uint8_t CPSDVSR;

  for (uint16_t SCR = 1; SCR < 256; ++SCR) {
    CPSDVSR = tempProduct / (1 + SCR);
    if (CPSDVSR < 255 && (CPSDVSR % 2 == 0)) {
      *preScalc = CPSDVSR;
      *scr      = SCR;
      return ERR_NO_ERR;
    }
  }
  return ERR_NO_VAL_PRESCALC;
}

spi_errCode spi_check_setting(const spi_settings setting) {
  if (setting.cpuClockMHz <= 0 || setting.cpuClockMHz > 80) { return ERR_INVAL_SYS_CLK_RATE; }
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

spi_errCode spi_recv_one_data_unit(const spi_settings setting,
                                   uint8_t*           totalBitRecv,
                                   uint16_t*          dataRecv) {
  GPIO_PORTA_DATA_R &= ~0x8;  // pull CS low to signal ready to read
  SSI0_CR1_R |= SSI_CR1_SSE;  // begin SPI operation
  for (int i = 0; i < 30; ++i) {}
  if (spi_check_rx_not_empty() == ERR_NO_ERR) {
    uint8_t  curBitRecv   = 0;
    uint16_t tempRecvData = SSI0_DR_R & 0x0000FFFF;
    spi_wait_busy();
    while (curBitRecv < 8) {
      dataRecv[*totalBitRecv / 8] += ((tempRecvData & (1 << curBitRecv)) >> curBitRecv)
                                     << (*totalBitRecv % 8);
      ++curBitRecv;
      *totalBitRecv = *totalBitRecv + 1;
    }
  }
  return ERR_NO_ERR;
}

spi_errCode spi_send_one_data_unit(const spi_settings setting,
                                   uint8_t*           totalBitSend,
                                   const uint16_t*    dataSend) {
  uint8_t tempSendData   = 0;
  uint8_t curBitTransfer = 0;

  // prep the data to be sent
  while (curBitTransfer < 8) {
    if (spi_check_tx_full() == ERR_NO_ERR) {
      tempSendData +=
          (((dataSend[*totalBitSend / 8] & (0b1 << (*totalBitSend % 8))) >> *totalBitSend)
           << curBitTransfer);
      ++curBitTransfer;
      *totalBitSend = *totalBitSend + 1;
    }
  }
  spi_wait_busy();
  SSI0_DR_R += (tempSendData) << 8;
  for (int i = 0; i < 5000; ++i) {}
  GPIO_PORTA_DATA_R &= ~0x8;  // pre-load then pull CS low
  SSI0_CR1_R |= SSI_CR1_SSE;  // begin SPI operation
  spi_wait_busy();
  return ERR_NO_ERR;
}
#include <assert.h>

#include "TivaC_SPI.h"
#include "TivaC_SPI_utils.h"
#include "tm4c123gh6pm.h"

spi_errCode spi_open(const spi_settings setting) {
  if (spi_check_setting(setting) != ERR_NO_ERR) { return spi_check_setting(setting); }

  uint8_t preScalc    = 0;
  uint8_t scr         = 0;
  uint8_t tempErrCode = 0;

  // TODO: add support for SPI controller other than spi0

  /* Prepping GPIO pin for SPI functionalities */
  SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R0;    // turn on SPI module
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;  // enable clock for SPI pins

  GPIO_PORTA_LOCK_R = 0x4C4F434B;  // unlock register
  GPIO_PORTA_CR_R |= 0x3C;         // allow bits in the register to be written

  // need afsel for 19(PA2)(clock), 20(frame signal), 21(RX), 22(TX)
  GPIO_PORTA_AFSEL_R &= ~0x3C;
  GPIO_PORTA_AFSEL_R |= 0x00000034;  // enable every pin except the CS pin for special function

  // PMCn for the pins above to 2
  GPIO_PORTA_PCTL_R &= 0xFF0000FF;
  GPIO_PORTA_PCTL_R |= 0x00222200;

  // GPIODEN for all those pins
  GPIO_PORTA_DEN_R |= 0x0000003C;
  GPIO_PORTA_DIR_R |= 0x8;
  GPIO_PORTA_DATA_R |= 0x8;  // pull CS high

  GPIO_PORTA_ODR_R |= 0x10;  // open drain necessary for MISO
  GPIO_PORTA_DR8R_R |= 0x30;

  //  pull up/ pull down pull-up for clock pin if high steady state in SPO
  if (setting.cpol == 0) {
    GPIO_PORTA_PDR_R |= 0x4;
  } else if (setting.cpol == 1) {
    GPIO_PORTA_PUR_R |= 0x4;
  } else {
    return ERR_SPI_CPA_UNDEFINED;
  }

  GPIO_PORTA_CR_R &= ~0x3C;
  GPIO_PORTA_LOCK_R = 0;  // relock the register

  /* Setting the SPI register based on user settings */
  SSI0_CR1_R &= ~SSI_CR1_SSE;  // disable SSI port before changing setting

  if (setting.role == Master) {
    SSI0_CR1_R &= ~SSI_CR1_MS;
  } else {
    SSI0_CR1_R |= SSI_CR1_MS;
  }

  if (setting.clockSource == Piosc) {
    SSI0_CC_R |= SSI_CC_CS_PIOSC;
  } else {
    SSI0_CC_R &= SSI_CC_CS_SYSPLL;
  }

  if (spi_calc_clock_prescalc(setting, &preScalc, &scr) == ERR_NO_ERR) {
    SSI0_CR0_R &= ~SSI_CR0_SCR_M;
    SSI0_CR0_R += scr << SSI_CR0_SCR_S;

    SSI0_CPSR_R &= ~SSI_CPSR_CPSDVSR_M;
    SSI0_CPSR_R += preScalc << SSI_CPSR_CPSDVSR_S;
  } else {
    // TODO: check for more efficient error return
    return spi_calc_clock_prescalc(setting, &preScalc, &scr);
  }

  if (setting.cpha == 1) {
    SSI0_CR0_R |= SSI_CR0_SPH;
  } else {
    SSI0_CR0_R &= ~SSI_CR0_SPH;
  }

  if (setting.cpol == 1)
    SSI0_CR0_R |= SSI_CR0_SPO;
  else
    SSI0_CR0_R &= ~SSI_CR0_SPO;

  SSI0_CR0_R &= ~SSI_CR0_DSS_M;
  SSI0_CR0_R += setting.transferSizeBit - 1;

  // setting protocol type
  SSI0_CR0_R &= ~SSI_CR0_FRF_M;
  switch (setting.operMode) {
    case Freescale:
      SSI0_CR0_R |= SSI_CR0_FRF_MOTO;
      break;
    case Tissf:
      SSI0_CR0_R |= SSI_CR0_FRF_TI;
      break;
    case Microwire:
      SSI0_CR0_R |= SSI_CR0_FRF_NMW;
      break;
    default:
      return ERR_INVAL_PROTOCOL;
  }

  if (setting.enableDMA) {
    // TODO: prep the DMA for support
    SSI0_DMACTL_R |= SSI_DMACTL_TXDMAE;
    SSI0_DMACTL_R |= SSI_DMACTL_RXDMAE;
  } else {
    SSI0_DMACTL_R &= ~SSI_DMACTL_TXDMAE;
    SSI0_DMACTL_R &= ~SSI_DMACTL_RXDMAE;
  }

  if (setting.isLoopBack) {
    SSI0_CR1_R |= SSI_CR1_LBM;
  } else {
    SSI0_CR1_R &= ~SSI_CR1_LBM;
  }
  return ERR_NO_ERR;
}

spi_errCode spi_transfer(const spi_settings setting,
                         uint16_t*          dataSend,
                         uint8_t            dataSendLenByte,
                         uint16_t*          dataRecv,
                         uint8_t            dataRecvLenByte,
                         spi_transfer_mode  transferMode) {
  /* Pre-Transfer Error Checking and Computation Start Here */
  if (spi_check_setting(setting) != ERR_NO_ERR) { return spi_check_setting(setting); }
  uint8_t totalBitToSend = 0;
  uint8_t totalBitToRecv = 0;

  if (transferMode == Tx || transferMode == Both) {
    assert(dataSend);
    assert(dataSendLenByte > 0);
    // convert from byte to bit for amount of data need to be sent/recv
    totalBitToSend = dataSendLenByte * 8;
  }

  if (transferMode == Rx || transferMode == Both) {
    assert(dataRecv);
    assert(dataRecvLenByte > 0);
    for (int i = 0; i < dataRecvLenByte; ++i) { dataRecv[i] = 0; }
    // convert from byte to bit for amount of data need to be sent/recv
    totalBitToRecv = dataRecvLenByte * 8;
  }

  /* Begin Transfer */
  uint8_t totalBitSend = 0;
  uint8_t totalBitRecv = 0;

  while (totalBitSend < totalBitToSend || totalBitRecv < totalBitToRecv) {
    // TODO: implement some kind of timeout if possible
    if ((transferMode == Tx || transferMode == Both) && (totalBitSend < totalBitToSend)) {
      spi_send_one_data_unit(setting, &totalBitSend, dataSend);
    }

    if ((transferMode == Rx || transferMode == Both) && totalBitRecv < totalBitToRecv) {
      spi_recv_one_data_unit(setting, &totalBitRecv, dataRecv);
    }
  }
  for (int i = 0; i < 50; ++i) {}  // delay
  spi_wait_busy();
  GPIO_PORTA_DATA_R |= 0x8;
  SSI0_CR1_R &= ~SSI_CR1_SSE;  // cease SPI operation
  return ERR_NO_ERR;
}

void main(void) {
  const spi_settings spiSetting = {.enableDMA       = false,
                                   .spiBitRateMbits = 0.3,
                                   .cpuClockMHz     = 16,
                                   .cpol            = 0,
                                   .cpha            = 0,
                                   .operMode        = Freescale,
                                   .isLoopBack      = false,
                                   .transferSizeBit = 16,
                                   .role            = Master,
                                   .clockSource     = Systemclock};

  if (spi_open(spiSetting) != ERR_NO_ERR) {
    for (;;) {
      // debug loop
    }
  }

  uint16_t sendData[1] = {0xD0};
  uint16_t recvData[1];

  for (;;) {
    for (int i = 0; i < 50000; ++i) {}  // add delay
    spi_transfer(spiSetting, sendData, 1, recvData, 1, Both);
  }
}
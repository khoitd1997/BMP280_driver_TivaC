#include "include/TivaC_SPI.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "external/TivaC_Utils/include/TivaC_LED.h"
#include "external/TivaC_Utils/include/TivaC_Other_Utils.h"
#include "external/TivaC_Utils/include/bit_manipulation.h"
#include "external/TivaC_Utils/include/tm4c123gh6pm.h"
#include "include/TivaC_SPI_utils.h"

SpiErrCode spi_open(const SpiSettings setting) {
  uint8_t errCode;
  if ((errCode = spi_check_setting(setting)) != SPI_ERR_NO_ERR) { return errCode; }

  uint8_t preScalc    = 0;
  uint8_t scr         = 0;
  uint8_t tempErrCode = 0;

  /* Prepping GPIO pin for SPI functionalities */
  SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R0;    // turn on SPI module
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;  // enable clock for SPI pins

  // unlock register and allow bits in the register to be written
  GPIO_PORTA_LOCK_R = 0x4C4F434B;
  GPIO_PORTA_CR_R |= 0x3C;

  // need afsel for 19(PA2)(clock), 21(RX), 22(TX)
  // enable every pin except the CS pin for special function
  // not making the CS a special function allows us to control it manually
  GPIO_PORTA_AFSEL_R &= ~0x3C;
  GPIO_PORTA_AFSEL_R |= 0x00000034;

  // PMCn for the pins above to 2
  GPIO_PORTA_PCTL_R &= 0xFF0000FF;
  GPIO_PORTA_PCTL_R |= 0x00222200;

  // GPIODEN for all those pins
  GPIO_PORTA_DEN_R |= 0x0000003C;
  GPIO_PORTA_DIR_R |= 0x8;   // allow CS pin to be an output
  GPIO_PORTA_DATA_R |= 0x8;  // pull CS high

  GPIO_PORTA_ODR_R |= 0x10;   // open drain necessary for MISO
  GPIO_PORTA_DR8R_R |= 0x30;  // Increase Drive Strength to 8mA for MOSI pin

  //  pull up/ pull down pull-up for clock pin depending on steady state of clock pin
  if (setting.cpol == 0) {
    GPIO_PORTA_PDR_R |= 0x4;
  } else if (setting.cpol == 1) {
    GPIO_PORTA_PUR_R |= 0x4;
  } else {
    return SPI_ERR_SPI_CPA_UNDEFINED;
  }

  // relock the register
  GPIO_PORTA_CR_R &= ~0x3C;
  GPIO_PORTA_LOCK_R = 0;

  /* Setting the SPI register based on user settings */
  spi_disable_spi();  // disable SSI port before changing setting

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

  if ((errCode = spi_calc_clock_prescalc(setting, &preScalc, &scr)) == SPI_ERR_NO_ERR) {
    SSI0_CR0_R &= ~SSI_CR0_SCR_M;
    SSI0_CR0_R += scr << SSI_CR0_SCR_S;

    SSI0_CPSR_R &= ~SSI_CPSR_CPSDVSR_M;
    SSI0_CPSR_R += preScalc << SSI_CPSR_CPSDVSR_S;
  } else {
    return errCode;
  }

  if (setting.cpha == 1) {
    bit_set(SSI0_CR0_R, SSI_CR0_SPH);
  } else {
    bit_clear(SSI0_CR0_R, SSI_CR0_SPH);
  }

  if (setting.cpol == 1) {
    bit_set(SSI0_CR0_R, SSI_CR0_SPO);
  } else {
    bit_clear(SSI0_CR0_R, SSI_CR0_SPO);
  }
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
      return SPI_ERR_INVAL_PROTOCOL;
  }

  if (setting.enableDMA) {
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
  return SPI_ERR_NO_ERR;
}

SpiErrCode spi_close(void) { bit_clear(SYSCTL_RCGCSSI_R, SYSCTL_RCGCSSI_R0); }

SpiErrCode spi_transfer(const SpiSettings setting,
                        uint8_t*          dataTx,
                        const uint8_t     dataTxLenByte,
                        uint8_t*          dataRx,
                        const uint8_t     dataRxLenByte) {
  uint8_t errCode;
  /* Pre-Transfer Error Checking and Computation Start Here */

  if ((dataTxLenByte) > 0) { assert(dataTx); }

  if ((dataRxLenByte) > 0) { assert(dataRx); }

  /* Begin Transfer */
  uint8_t totalByteTxed = 0;
  uint8_t totalByteRxed = 0;
  bool    firstRun      = true;
  bool    secondRun     = true;

  spi_enable_spi();
  spi_clear_rx_buffer();
  while (totalByteTxed < dataTxLenByte || totalByteRxed < dataRxLenByte) {
    spi_pull_cs_low();

    if ((totalByteTxed < dataTxLenByte)) {
      spi_tx_one_data_unit(setting.transferSizeBit, &totalByteTxed, dataTx);
    }

    spi_data_delay();
    if (true == firstRun) {
      spi_clear_rx_buffer();
      firstRun = false;
    }

    if ((totalByteRxed < dataRxLenByte)) {
      if (dataTxLenByte == totalByteTxed) { spi_send_dummy_byte(); }
      spi_rx_one_data_unit(setting, &totalByteRxed, dataRx);
    }
    spi_bus_wait();
    if ((setting.cpol == 0 && setting.cpha == 0) || (setting.cpol == 1 && setting.cpha == 0)) {
      spi_pull_cs_high();  // the spi module requires that cs is pulled high on these settings
                           // btween transfer
    }
  }

  spi_bus_wait();
  spi_pull_cs_high();
  spi_disable_spi();
  return SPI_ERR_NO_ERR;
}

// TODO: put in check for SPI
SpiErrCode spi_check_spi_enabled(void) {}

// void main(void) {
// const SpiSettings spiSetting = {.enableDMA       = false,
//                                 .spiBitRateMbits = 0.3,
//                                 .cpuClockMHz     = 16,
//                                 .cpol            = 1,
//                                 .cpha            = 1,
//                                 .operMode        = Freescale,
//                                 .isLoopBack      = false,
//                                 .transferSizeBit = 8,
//                                 .role            = Master,
//                                 .clockSource     = Systemclock};

//   if (spi_open(spiSetting) != SPI_ERR_NO_ERR) { return; }

//   uint8_t txData[1] = {0xD0};
//   uint8_t rxData[1];
//   greenled_init();
//   redled_init();
//   blueled_init();
//   greenled_off();

//   for (;;) {
//     spi_transfer(spiSetting, txData, 1, rxData, 1);
//     printf("\nThe ID is %d", rxData[1]);
//     if (0x58 == rxData[0]) {
//       greenled_on();
//     } else {
//       greenled_off();
//       for (;;) {}
//     }
//     blueled_off();
//     delayms(100);
//   }
// }
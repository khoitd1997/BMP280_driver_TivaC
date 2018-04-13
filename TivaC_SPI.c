#include <assert.h>

#include "TivaC_SPI.h"
#include "TivaC_SPI_utils.h"
#include "tm4c123gh6pm.h"

spi_errCode spi_open(const spi_settings* setting)
{
  spi_check_setting(setting);

  uint8_t preScalc    = 0;
  uint8_t scr         = 0;
  uint8_t tempErrCode = 0;

  // TODO: add support for SPI controller other than spi0
  spi_wait_busy(setting->spiControlerNum);
  if (setting->spiControlerNum == 0)
    {
      /* Prepping GPIO pin for SPI functionalities */
      SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R0;    // turn on SPI module
      SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;  // enable clock SPI pins

      // need afsel for 19(PA2)(clock), 20(frame signal), 21(RX), 22(TX)
      GPIO_PORTA_AFSEL_R &= ~0x0000003C;

      // PMCn for the pins above to 2
      GPIO_PORTA_PCTL_R &= ~0xFFDDDDFF;
      GPIO_PORTA_PCTL_R |= 0x00222200;

      // GPIODEN for all those pins
      GPIO_PORTA_DEN_R |= 0x0000003C;

      GPIO_PORTA_DR4R_R |= 0x0000003C;  // 4 mA drive Strength
      GPIO_PORTA_ODR_R |= 0x0000003C;   // open drain enabled

      //  pull up/ pull down pull-up for clock pin if high steady state in SPO
      if (setting->cpol == 0)
        {
          GPIO_PORTA_PDR_R |= 0x00000004;
        }
      else if (setting->cpol == 1)
        {
          GPIO_PORTA_PUR_R |= 0x00000004;
        }
      else
        {
          return ERR_SPI_CPA_UNDEFINED;
        }

      /* Setting the SPI register based on user settings */
      SSI0_CR1_R &= ~SSI_CR1_SSE;  // disable SSI port before changing setting

      if (setting->role == Master)
        {
          SSI0_CR1_R &= ~SSI_CR1_MS;
        }
      else
        {
          SSI0_CR1_R |= SSI_CR1_MS;
        }

      if (setting->clockSource == Piosc)
        {
          SSI0_CC_R |= SSI_CC_CS_PIOSC;
        }
      else
        {
          SSI0_CC_R &= SSI_CC_CS_SYSPLL;
        }

      if (spi_calc_clock_prescalc(setting, &preScalc, &scr) == ERR_NO_ERR)
        {
          SSI0_CR0_R &= ~SSI_CR0_SCR_M;
          SSI0_CR0_R += scr << SSI_CR0_SCR_S;

          SSI0_CPSR_R &= ~SSI_CPSR_CPSDVSR_M;
          SSI0_CPSR_R += preScalc << SSI_CPSR_CPSDVSR_S;
        }
      else
        {
          // TODO: check for more efficient error return
          return spi_calc_clock_prescalc(setting, &preScalc, &scr);
        }

      if (setting->cpha == 1)
        SSI0_CR0_R |= SSI_CR0_SPH;
      else
        SSI0_CR0_R &= ~SSI_CR0_SPH;

      if (setting->cpol == 0)
        SSI0_CR0_R |= SSI_CR0_SPO;
      else
        SSI0_CR0_R &= ~SSI_CR0_SPO;

      SSI0_CR0_R &= ~SSI_CR0_DSS_M;
      SSI0_CR0_R += setting->dataSize;

      if (setting->enableDMA)
        {
          // TODO: prep the DMA for support
          SSI0_DMACTL_R |= SSI_DMACTL_TXDMAE;
          SSI0_DMACTL_R |= SSI_DMACTL_RXDMAE;
        }
      else
        {
          SSI0_DMACTL_R &= ~SSI_DMACTL_TXDMAE;
          SSI0_DMACTL_R &= ~SSI_DMACTL_RXDMAE;
        }
    }
  return ERR_NO_ERR;
}

spi_errCode spi_transfer(const spi_settings* setting,
                         uint8_t*            dataSend,
                         uint8_t             dataSendLength,
                         uint8_t*            dataRecv,
                         uint8_t             dataRecvLength,
                         spi_transfer_mode   transferMode)
{
  if (spi_check_setting(setting) != ERR_NO_ERR)
    {
      return spi_check_setting(setting);
    }
  if (transferMode == Tx || transferMode == Both)
    {
      if (spi_check_tx_full(setting->spiControlerNum))
        {
          return ERR_TX_FULL;
        }
      assert(dataSend);
      assert(dataSendLength > 0);
    }
  if (transferMode == Rx || transferMode == Both)
    {
      if (spi_check_rx_full(setting->spiControlerNum))
        {
          return ERR_RX_FULL;
        }
      assert(dataRecv);
      assert(dataRecvLength > 0);
      for (int i = 0; i < dataRecvLength; ++i)
        {
          dataRecv[i] = 0;
        }
    }

  uint8_t  totalBitTransfer = 0;
  uint8_t  totalBitRecv     = 0;
  uint8_t  curBitTransfer   = 0;  // how many bits transferred in this transaction
  uint8_t  curBitRecv       = 0;
  uint16_t tempSendData;
  uint16_t tempRecvData;

  // convert from byte to bit for amount of data need to be sent/recv
  dataSendLength = dataSendLength * 8 - (dataSendLength * 8) % (setting->dataSize);
  dataRecvLength = dataRecvLength * 8 - (dataRecvLength * 8) % (setting->dataSize);

  if (setting->spiControlerNum == 0)
    {
      SSI0_CR1_R |= SSI_CR1_SSE;
      while (totalBitTransfer < dataSendLength || totalBitRecv < dataRecvLength)
        {
          spi_wait_busy(setting->spiControlerNum);
          if (totalBitTransfer < dataSendLength)
            {
              if (transferMode == Tx || transferMode == Both)
                {
                  tempSendData   = 0;
                  curBitTransfer = 0;
                  while (curBitTransfer < setting->dataSize)
                    {
                      tempSendData += (dataSend[totalBitTransfer / 8] >> (totalBitTransfer % 8))
                                      << curBitTransfer;
                      ++curBitTransfer;
                      ++totalBitTransfer;
                    }
                  SSI0_DR_R &= ~SSI_DR_DATA_M;
                  SSI0_DR_R += tempSendData << SSI_DR_DATA_S;
                }
            }

          spi_wait_busy(setting->spiControlerNum);

          if (totalBitRecv < dataRecvLength)
            {
              if (transferMode == Rx || transferMode == Both)
                {
                  uint8_t curBitRecv = 0;
                  tempRecvData       = SSI0_DR_R & 0x0000FFFF;
                }
              while (curBitRecv < setting->dataSize)
                {
                  dataRecv[totalBitRecv / 8] += (tempRecvData & (1 << curBitRecv))
                                                << (totalBitTransfer % 8);
                  ++curBitRecv;
                  ++totalBitRecv;
                }
            }
        }
      SSI0_CR1_R &= ~SSI_CR1_SSE;
    }
  return ERR_NO_ERR;
}

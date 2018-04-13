#include <stdint.h>
#include <stdbool.h>

#include "TivaC_SPI.h"
#include "TivaC_SPI_utils.h"
#include "tm4c123gh6pm.h"

#define SPI_MAX_CONTROLLER 3

spi_errCode spi_wait_busy(uint8_t spiControllerNum)
{
  assert(spiControllerNum < SPI_MAX_CONTROLLER);
  if (spiControllerNum == 0)
    {
      while (SSI0_SR_R & SSI_SR_BSY)
        {
          // wait until the busy bit is unset
        }
      return ERR_NO_ERR;
    }
}

spi_errCode spi_check_rx_full(uint8_t spiControllerNum)
{
  assert(spiControllerNum < SPI_MAX_CONTROLLER);
  if (spiControllerNum == 0)
    {
      return (SSI0_SR_R & SSI_SR_RFF) ? ERR_RX_FULL : ERR_NO_ERR;
    }
}

spi_errCode spi_check_tx_full(uint8_t spiControllerNum)
{
  assert(spiControllerNum < SPI_MAX_CONTROLLER);
  if (spiControllerNum == 0)
    {
      (SSI0_SR_R & SSI_SR_TNF) ? ERR_TX_FULL : ERR_NO_ERR;
    }
}

spi_errCode spi_calc_clock_prescalc(spi_settings* setting,
                                    uint8_t*      preScalc,
                                    uint8_t*      scr)
{
  spi_check_setting(setting);
  assert(preScalc);
  assert(scr);
  uint8_t tempProduct = (setting->currCpuClockMHz) / (setting->spiBitRate);
  uint8_t CPSDVSR;

  for (uint16_t SCR = 0; SCR < 256; ++SCR)
    {
      CPSDVSR = tempProduct / (1 + SCR);
      if (CPSDVSR < 255 && (CPSDVSR % 2 == 0))
        {
          *preScalc = CPSDVSR;
          *scr      = SCR;
          return ERR_NO_ERR;
        }
    }
  return ERR_NO_VAL_PRESCALC;
}

spi_errCode spi_check_setting(spi_settings* setting)
{
  assert(setting);
  if (setting->currCpuClockMHz <= 0 || setting->currCpuClockMHz > 80)
    {
      return ERR_INVAL_SYS_CLK_RATE;
    }
  if (setting->spiBitRate <= 0 || setting->spiBitRate > 2)
    {
      return ERR_INVAL_BIT_RATE;
    }
  if (setting->spiControlerNum > SPI_MAX_CONTROLLER - 1)
    {
      return ERR_INVAL_SPI_CTRL;
    }
  if (setting->dataSize > 16) return ERR_INVAL_DATA_SIZE;
  if (setting->cpol > 1) return ERR_INVAL_CPOL;
  if (setting->cpha > 1) return ERR_INVAL_CPHA;

  return ERR_NO_ERR;
}
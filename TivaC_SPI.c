#include <assert.h>

#include "TivaC_SPI.h"
#include "tm4c123gh6pm.h"

void spi_open(const spi_settings* setting, spi_errCode* errCode)
{
  assert(setting);
  assert(errCode);

  // TODO: add support for SPI controller other spi0
  if (setting->spiControlerNum == 0)
    {
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
          // TODO: change this into error handling func
          *errCode = ERROR_SPI_CPA_UNDEFINED;
          return;
        }
    }
}
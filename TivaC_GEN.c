#include "TivaC_GEN.h"
/**
 * Available values are 80, 66.6, 50, 44.4, 40, ...
 *
 */

uint8_t pll_enable(uint8_t target_frequency)
{
  // hard code at 80MHz for now
  SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;
  SYSCTL_RCC_R |= SYSCTL_RCC_BYPASS;
  SYSCTL_RCC_R &= ~(SYSCTL_RCC_USESYSDIV);
  SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;

  // Select external crystal and oscillator source
  SYSCTL_RCC_R &= ~SYSCTL_RCC_XTAL_M;
  SYSCTL_RCC_R += SYSCTL_RCC_XTAL_16MHZ;
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_OSCSRC2_M;
  SYSCTL_RCC2_R += SYSCTL_RCC2_OSCSRC2_MO;
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;
  // select system divider and set usesys

  SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_SYSDIV2LSB;
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_SYSDIV2_M;
  SYSCTL_RCC2_R += (0x02) << 23;
  while (!(SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS))
    {
      // wait for the PLL to reach ready time
    }

  // unlock bypass bit
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;

  return 0;
}

uint8_t delayms(uint32_t milliseconds)
{
  // setup systick assuming 80MHz system clock frequency

  NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
  NVIC_ST_RELOAD_R &= ~NVIC_ST_RELOAD_M;
  NVIC_ST_RELOAD_R += (80000 - 1) << NVIC_ST_RELOAD_S;
  NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC;
  NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;
  while (milliseconds-- > 0)
    {
      while (!(NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT))
        {
          // wait for the count bit to hit 0
        }
    }
  return 0;
}

// TODO: Fix Green LED
// Green LED on PF3
void greenled_init(void)
{
  SYSCTL_RCGCGPIO_R |= 0x20;
  GPIO_PORTF_LOCK_R = 0x4C4F434B;
  GPIO_PORTF_CR_R |= 0x08;
  GPIO_PORTF_AFSEL_R &= ~(0x08);
  GPIO_PORTF_PCTL_R &= ~(0x08);
  GPIO_PORTF_DEN_R |= 0x08;
  GPIO_PORTF_DIR_R |= 0x08;
  GPIO_PORTF_LOCK_R = 0;
}

void greenled_on(void)
{
  GPIO_PORTF_DATA_R |= 0x08;
}
void greenled_off(void)
{
  GPIO_PORTF_DATA_R &= ~(0x08);
}

// Red LED on PF1
void redled_init(void)
{
  SYSCTL_RCGCGPIO_R |= 0x20;
  GPIO_PORTF_LOCK_R = 0x4C4F434B;
  GPIO_PORTF_CR_R |= 0x02;
  GPIO_PORTF_AFSEL_R &= ~(0x02);
  GPIO_PORTF_PCTL_R &= ~(0x02);
  GPIO_PORTF_DEN_R |= 0x02;
  GPIO_PORTF_DIR_R |= 0x02;
  GPIO_PORTF_LOCK_R = 0;
}
void redled_on(void)
{
  GPIO_PORTF_DATA_R |= 0x02;
}
void redled_off(void)
{
  GPIO_PORTF_DATA_R &= ~(0x02);
}

// Blue LED on PF1
void blueled_init(void)
{
  SYSCTL_RCGCGPIO_R |= 0x20;
  GPIO_PORTF_LOCK_R = 0x4C4F434B;
  GPIO_PORTF_CR_R |= 0x04;
  GPIO_PORTF_AFSEL_R &= ~(0x04);
  GPIO_PORTF_PCTL_R &= ~(0x04);
  GPIO_PORTF_DEN_R |= 0x04;
  GPIO_PORTF_DIR_R |= 0x04;
  GPIO_PORTF_LOCK_R = 0;
}
void blueled_on(void)
{
  GPIO_PORTF_DATA_R |= 0x04;
}
void blueled_off(void)
{
  GPIO_PORTF_DATA_R &= ~(0x04);
}

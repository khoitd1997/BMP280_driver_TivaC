#ifndef _TIVAC_GEN_H
#define _TIVAC_GEN_H
#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#define GPIO_PORTA_DATA_R (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DIR_R (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_DEN_R (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_AMSEL_R (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R (*((volatile unsigned long *)0x4000452C))
#define PA2 (*((volatile unsigned long *)0x40004010))
#define SYSCTL_RCGC2_R (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF 0x00000020  // port F Clock Gating Control
/** Written for TIva C general function like:
 * GPIO, uDMA, Watchdog, ADC, memory, sleep
 *
 */
// TODO: add support for PLL
// add support for watchdog timer
uint8_t pll_enable(uint8_t target_frequency);
uint8_t delayms(uint32_t milliseconds);

// Make PA2 an output, enable digital I/O, ensure alt. functions off
void greenled_init(void);
void greenled_on(void);
void greenled_off(void);
void blueled_init(void);
void blueled_on(void);
void blueled_off(void);
void redled_init(void);
void redled_on(void);
void redled_off(void);

#endif

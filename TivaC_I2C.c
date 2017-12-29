#include <TivaC_I2C.h>

/**
 * @brief
 * Use I2C0 controller and support single or multiple bytes transfer up to high
 * speed modes
 *
 */
// TODO: OPTIONAL MASTER OR SLAVE CONFIG, high speed, arbitation
// add checking if the port is open or not
uint8_t i2c0_open(void)
{
  // Enable RCGCI2C for i2c0
  SYSCTL_RCGCI2C_R |= 0x01;
  // Enable PortB RCGCGPIO
  SYSCTL_RCGCGPIO_R |= 0x02;
  // set GPIOAFSEL default i2c0 enabled
  GPIO_PORTB_LOCK_R = 0x4C4F434B;
  GPIO_PORTB_CR_R |= 0x0C;
  GPIO_PORTB_DEN_R &= 0xFFF3;
  GPIO_PORTB_DIR_R &= 0xFFF3;
  GPIO_PORTB_PUR_R &= 0xFFF3;
  GPIO_PORTB_AFSEL_R &= 0xFFF3;
  GPIO_PORTB_PCTL_R |= 0x03;

  // Open drain on I2CSDA PB3 and non on PB2
  GPIO_PORTB_ODR_R |= 0x08;
  GPIO_PORTB_ODR_R &= 0xFFFB;
  // No need for PMCn field in GPIOPCTL register
  // I2CMCR0 init master, glitch filter
  I2C0_MCR_R = 0x50;
  I2C0_MCS_R |= 0x01;
  // calculate the clock cycle and input into I2CMTPR
  // rn assume 80MHz, later add support for variable frequency
  I2C0_MTPR_R = 0x09;
  // input address into I2CMSA, 0x77 for the BMP280

  // enable interrupt in i2cmimr
  I2C0_MIMR_R       = 0x01;
  GPIO_PORTB_LOCK_R = 0x00;  // lock the PORTB
}

uint8_t i2c0_close(void)
{
// should really disable interrupt here
#ifdef FORCED_I2C_CLOSED  // force close even if there is error
  if ((I2C0_MCS_R & 0x080) || (I2C0_MCS_R & 0x02))
    {
      // Error occured
      return 1;
    }
#endif
  if ((I2C0_MCS_R & 0x01) || (I2C0_MCS_R & 0x40) || !(I2C0_MCS_R & 0x20))
    {
      // Something is still happening
      return 2;
    }

  GPIO_PORTB_LOCK_R = 0x4C4F434B;
  GPIO_PORTB_CR_R |= 0x0C;
  GPIO_PORTB_AFSEL_R &= 0xFFF3;
  I2C0_MCS_R &= 0xFFF8;
  I2C0_MCR_R &= 0xFF8F;
  GPIO_PORTB_LOCK_R = 0x00;  // lock the PORTB
  // reenable interrupt here
  // just don't disable the clock
  return 0;
}

uint8_t i2c0_data_write(uint8_t data_byte, uint32_t slave_address)
{
  // write data byte into I2CMDR

  while ((I2C0_MCS_R & 0x01) || (I2C0_MCS_R & 0x40) || !(I2C0_MCS_R & 0x20))
    {
      // wait for the bus to stop being busy
    }
  I2C0_MSA_R = (slave_address << 1) & 0xFFFE;  // need to indicate 1
  I2C0_MDR_R = data_byte;
  // init transmit-> writing I2CMCS
  I2C0_MCS_R |= 0xF;  // 0-111  // generate start signal

  // check error in I2CMCS and return the error
  if ((I2C0_MCS_R & 0x080) || (I2C0_MCS_R & 0x02))
    {
      // Error occured
      return 1;
    }
}
// I2C0_MMIS_R leave in the main loop
uint32_t i2c0_data_read(uint32_t slave_address)
{
  while ((I2C0_MCS_R & 0x01) || (I2C0_MCS_R & 0x40) || !(I2C0_MCS_R & 0x20))
    {
      // wait for the bus to stop being busy
    }
  I2C0_MSA_R = (slave_address << 1) | 0x01;  // need to indicate 1
  // init transmit-> writing I2CMCS
  I2C0_MCS_R |= 0x07;  // 0-111  // generate start signal

  // check error in I2CMCS and return the error
  if ((I2C0_MCS_R & 0x080) || (I2C0_MCS_R & 0x02))
    {
      // Error occured
      return 0x01FF;  // maxium legal return value of I2C is 0xFF
    }
  else
    {
      return (I2C0_MDR_R & 0x00FF);
    }
  // if there is an interrupt in i2cmcs error and arblst bit, busy bit
  // clear i2cmicr bit to exit interrupt
  // write to i2cmcs
  // seem to need to check busy and error bit in i2cmcs
  // Read data from I2CMDR
}
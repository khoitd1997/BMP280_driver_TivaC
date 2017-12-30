#include "TivaC_I2C.h"
#include "TM4C123.h"

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
  GPIO_PORTB_ODR_R &= ~(0x04);
  // No need for PMCn field in GPIOPCTL register
  // I2CMCR0 init master, glitch filter
  I2C0_MCR_R |= I2C_MCR_GFE | I2C_MCR_MFE;
  // calculate the clock cycle and input into I2CMTPR
  // rn assume 80MHz, later add support for variable frequency
  I2C0_MTPR_R &= ~(I2C_MTPR_TPR_M);
  I2C0_MTPR_R += 0x09 << I2C_MTPR_TPR_S;
  // input address into I2CMSA, 0x77 for the BMP280

  // enable interrupt in i2cmimr
  I2C0_MIMR_R       = 0x01;
  GPIO_PORTB_LOCK_R = 0x00;  // lock the PORTB
  return 0;
}

uint8_t i2c0_close(void)
{
// should really disable interrupt here
#ifdef FORCED_I2C0_CLOSED  // force close even if there is error
  if ((I2C0_MCS_R & 0x080) || (I2C0_MCS_R & 0x02))
    {
      // Error occured
      return 1;
    }
#endif
  while ((I2C0_MCS_R & 0x01) || (I2C0_MCS_R & 0x40) || !(I2C0_MCS_R & 0x20))
    {
      // Wait till the bus is free
    }

  GPIO_PORTB_LOCK_R = 0x4C4F434B;
  GPIO_PORTB_CR_R |= 0x0C;
  GPIO_PORTB_AFSEL_R &= 0xFFF3;
  I2C0_MCS_R &= 0xFFF8;
  I2C0_MCR_R &= 0xFF8F;
  GPIO_PORTB_LOCK_R = 0x00;  // lock the PORTB
  // reenable interrupt here
  // just don't disable the system clock
  return 0;
}

uint8_t i2c0_data_read(uint16_t slave_address)
{
  uint16_t data_counter;

  while ((I2C0_MCS_R & I2C_MCS_BUSY))
    {
      // wait for the bus to stop being busy
    }

  I2C0_MSA_R &= ~I2C_MSA_SA_M;
  I2C0_MSA_R += slave_address << I2C_MSA_SA_S;
  I2C0_MSA_R |= I2C_MSA_RS;
  I2C0_MCS_R |=
      I2C_MCS_ACK | I2C_MCS_STOP | I2C_MCS_START | I2C_MCS_RUN & (~I2C_MCS_HS);

  while ((I2C0_MCS_R & I2C_MCS_BUSY))
    {
      // wait for the bus to stop being busy
    }

  if (I2C0_MCS_R & I2C_MCS_ERROR)
    {
      return 1;
    }
  else
    {
      return I2C0_MDR_R;
    }
}

uint8_t i2c0_data_write(uint32_t slave_address, uint8_t data_byte)
{
  while ((I2C0_MCS_R & I2C_MCS_BUSY))
    {
      // wait for the bus to stop being busy
    }
  I2C0_MDR_R = data_byte << I2C_MDR_DATA_S;

  while ((I2C0_MCS_R & I2C_MCS_BUSY))
    {
      // wait for the bus to stop being busy
    }

  I2C0_MSA_R &= ~(I2C_MSA_RS);
  I2C0_MCS_R |=
      I2C_MCS_ACK | I2C_MCS_STOP | I2C_MCS_START | I2C_MCS_RUN & (~I2C_MCS_HS);

  while ((I2C0_MCS_R & I2C_MCS_BUSY))
    {
      // wait for the bus to stop being busy
    }

  if (I2C0_MCS_R & I2C_MCS_ERROR)
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

/*
uint8_t i2c0_data_byte_write(uint8_t data_byte, uint32_t slave_address)
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
  while ((I2C0_MCS_R & 0x01))
    {
      // check if the I2C controller is still busy
      // maybe add a delay
    }
  // check error in I2CMCS and return the error
  if ((I2C0_MCS_R & 0x080) || (I2C0_MCS_R & 0x02))
    {
      // Error occured
      return 1;
    }
  return 0;
}

// I2C0_MMIS_R leave in the main loop
uint32_t i2c0_data_byte_read(uint32_t slave_address)
{
  while ((I2C0_MCS_R & 0x01) || (I2C0_MCS_R & 0x40) || !(I2C0_MCS_R & 0x20))
    {
      // wait for the bus to stop being busy
    }
  I2C0_MSA_R = (slave_address << 1) | 0x01;
  // init transmit-> writing I2CMCS
  I2C0_MCS_R |= 0x07;  // 0-111  // generate start signal
  while ((I2C0_MCS_R & 0x01))
    {
      // check if the I2C controller is still busy
      // maybe add a delay
    }
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
*/
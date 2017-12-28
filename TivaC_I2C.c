#include <TivaC_I2C.h>

/**
 * @brief
 * Use I2C0 controller and support single or multiple bytes transfer up to high
 * speed modes
 *
 */
// TODO: GO BACK AND WRITE GPIOAFSEL and GPIOPCTL
uint8_t i2c0_open(uint32_t slave_address)
{
  // Enable RCGCI2C for i2c0
  SYSCTL_RCGCI2C_R |= 0x01;
  // Enable PortB RCGCGPIO
  SYSCTL_RCGCGPIO_R |= 0x02;
  // no need to set GPIOAFSEL default i2c0 enabled
  // Open drain on I2CSDA PB3 and non on PB2
  GPIO_PORTB_ODR_R |= 0x08;
  GPIO_PORTB_ODR_R &= 0xFFFD;
  // No need for PMCn field in GPIOPCTL register
  // I2CMCR0 init master, glitch filter
  I2C0_MCR_R = 0x50;
  // calculate the clock cycle and input into I2CMTPR
  // rn assume 80MHz, later add support for variable frequency
  I2C0_MTPR_R = 0x09;
  // input address into I2CMSA
  I2C0_MSA_R
  // enable interrupt in i2cmimr
  I2C0_MIMR_R
}

uint8_t i2c0_close(void)
{
  // Generate a Stop condition
  // clear the i2c function off gpio register
  // just don't disable the clock
}

uint8_t i2c0_data_write(uint8_t data_byte)
{
  // write data byte into I2CMDR
  // init transmit-> writing I2CMCS
  // check error in I2CMCS and return the error
}

uint32_t i2c0_data_read(void)
{
  // if there is an interrupt in i2cmcs error and arblst bit
  // clear i2cmicr bit to exit interrupt
  // write to i2cmcs
  // seem to need to check busy and error bit in i2cmcs
  // Read data from I2CMDR
}
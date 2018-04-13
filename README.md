Library used for interacting bmp280 with tivaC tm4c123gh6pm processor, the dev board used is the TivaC line of Texas Instrument 

---

## Style Guide:

- /\*\*/ comments indicate a section comments, meaning it apply to all things below it up until the next /**/
- // indicate a specific comment, it refers to the one thing directly below it or before it in the same line

## Naming Convention:

- Error code begins with ERR_
- bmp280 function begins with bmp280_
- i2c0 function begins with i2c0_
- spi function begins with spi_
- All enum member at least has its first character in name capitalized

---

As of 4/13/2018, I'm in the process of restructuring error handling and refactor both i2c and spi code 
#include "BMP280_Utils.h"
#include "BMP280_Drv.h"

int checkUnitialized(bmp280* sensor, bmp280_errCode* errCode)
{
  if (sensor->mode == Uninitialized || sensor->tempSamp == Uninitialized ||
      sensor->presSamp == Uninitialized || sensor->samplSet == Uninitialized ||
      sensor->iirFilter == Uninitialized || sensor->standbyTime == -1)
    {
      *errCode = ERR_SETTING_UNITIALIZED;
      return -1;
    }
  else
    {
      *errCode = ERR_NO_ERR;
      return 0;
    }
}

// TODO: use register to check for open port instead 
int checkPortOpened(bmp280* sensor, bmp280_errCode* errCode)
{
  if (!(sensor->portOpened))
    {
      *errCode = ERR_PORT_NOT_OPEN;
      return -1;
    }
  else
    {
      *errCode = ERR_NO_ERR;
      return 0;
    }
}

uint8_t createCtrlByte(bmp280_Coeff    tempSampling,
                       bmp280_Coeff    pressSampling,
                       bmp280_operMode powerMode,
                       bmp280_errCode* errCode)
{
  // start with all 1 and zero out the needed bits
  uint8_t tempByte = 0xFF;
  switch (tempSampling)
    {
      case x0:
        tempByte &= 0x1F;
        break;

      case x1:
        tempByte &= 0x3F;
        break;

      case x2:
        tempByte &= 0x5F;
        break;

      case x4:
        tempByte &= 0x7F;
        break;

      case x8:
        tempByte &= 0x9F;
        break;

      case x16:
        tempByte &= 0xFF;
        break;

      default:
        *errCode = ERR_SETTING_UNRECOGNIZED;
        return 1;
        break;
    }

  switch (pressSampling)
    {
      case x0:
        tempByte &= 0xE3;
        break;

      case x1:
        tempByte &= 0xE7;
        break;

      case x2:
        tempByte &= 0xEB;
        break;

      case x4:
        tempByte &= 0xEF;
        break;

      case x8:
        tempByte &= 0xF3;
        break;

      case x16:
        tempByte &= 0xFF;
        break;

      default:
        *errCode = ERR_SETTING_UNRECOGNIZED;
        return 1;
        break;
    }

  switch (powerMode)
    {
      case Sleep:
        tempByte &= 0xFC;
        break;

      case Forced:
        tempByte &= 0xFD;
        break;

      case Normal:
        tempByte &= 0xFF;
        break;

      default:
        *errCode = ERR_SETTING_UNRECOGNIZED;
        return 1;
        break;
    }

  *errCode = ERR_NO_ERR;
  return tempByte;
}

uint8_t createConfigByte(float              standbyTime,
                         bmp280_Coeff       filterCoeff,
                         bmp280_comProtocol protocol,
                         bmp280_errCode*    errCode)
{
  // start with all 1 and zero out the needed bits
  uint8_t tempByte = 0xFF;

  // handle standby time
  if (standbyTime == 0.5)
    {
      tempByte &= 0x1F;
    }
  else if (standbyTime == 62.5)
    {
      tempByte &= 0x3F;
    }
  else if (standbyTime == 125)
    {
      tempByte &= 0x5F;
    }
  else if (standbyTime == 250)
    {
      tempByte &= 0x7F;
    }
  else if (standbyTime == 500)
    {
      tempByte &= 0x9F;
    }
  else if (standbyTime == 1000)
    {
      tempByte &= 0xBF;
    }
  else if (standbyTime == 2000)
    {
      tempByte &= 0xDF;
    }
  else if (standbyTime == 4000)
    {
      tempByte &= 0xFF;
    }
  else
    {
      *errCode = ERR_SETTING_UNRECOGNIZED;
      return 1;
    }

  // handle iir filter sampling
  switch (filterCoeff)
    {
      case x0:
        tempByte &= 0xE3;
        break;

      case x2:
        tempByte &= 0xEB;
        break;

      case x4:
        tempByte &= 0xEF;
        break;

      case x8:
        tempByte &= 0xF3;
        break;

      case x16:
        tempByte &= 0xFF;
        break;

      default:
        *errCode = ERR_SETTING_UNRECOGNIZED;
        return 1;
        break;
    }

  // set protocol to be I2C or SPI
  if (protocol == I2C)
    {
      tempByte &= 0xFE;
    }

  *errCode = ERR_NO_ERR;
  return tempByte;
}

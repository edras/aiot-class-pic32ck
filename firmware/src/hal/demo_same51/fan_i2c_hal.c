#include "fan_i2c_hal.h"

/*
 * INSTRUCTIONS
 * ============
 *
 * Implement all functions where they are marked as IMPLEMENT.
 * Follow the function specification in the comments.
 */

bool fan_i2c_read(uint8_t devid, uint8_t regaddr, uint8_t *regdata, uint16_t len)
{    
    FAN_I2C_WriteRead(devid, &regaddr, 1, regdata, len);
    
    while(FAN_I2C_IsBusy());
    
    return (FAN_I2C_ErrorGet() == FAN_I2C_ERROR_NONE);
}

bool fan_i2c_write(uint8_t devid, uint8_t reg, uint8_t *data_in, uint16_t len)
{
	uint8_t tx_buf[len + 1]; 
    uint8_t cnt = 0;

	tx_buf[cnt] = reg;
	for (cnt = 1; cnt <= len; cnt++)
    {
		tx_buf[cnt] = *data_in;
        data_in++;
	}
    
    FAN_I2C_Write(devid, tx_buf, len + 1);
    
    while (FAN_I2C_IsBusy());
    
    return (FAN_I2C_ErrorGet() == FAN_I2C_ERROR_NONE);
}


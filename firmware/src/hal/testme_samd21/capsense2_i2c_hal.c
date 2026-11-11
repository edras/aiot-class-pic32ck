#include "capsense2_i2c_hal.h"

/*
 * INSTRUCTIONS
 * ============
 *
 * Implement all functions where they are marked as IMPLEMENT.
 * Follow the function specification in the comments.
 */

#ifndef MIKROE_CAPSENSE2
int8_t CAPSENSE2_init(void){ return CAPSENSE2_E_DEV_NOT_FOUND; }
int8_t capsense2_check_alert_pin(){ return 0; }
void CAPSENSE2_execute(void){}
bool CAPSENSE2_action_required(void){ return false; }
CAPSENSE2_ACTIONS CAPSENSE2_get_s1_action(void){ return CAPSENSE2_NONE; }
CAPSENSE2_ACTIONS CAPSENSE2_get_s2_action(void){ return CAPSENSE2_NONE; }
CAPSENSE2_ACTIONS CAPSENSE2_get_slider_action(void){ return CAPSENSE2_NONE; }
uint8_t CAPSENSE2_get_slider_value(void){ return CAPSENSE2_NONE; }
bool CAPSENSE2_available(void){ return false; }
#endif

int8_t capsense2_i2c_read(uint8_t devid, uint8_t regaddr, uint8_t *regdata, uint16_t len)
{    
    CAPSENSE2_I2C_WriteRead(devid, &regaddr, 1, regdata, len);
    
    while(CAPSENSE2_I2C_IsBusy());
    
    if(CAPSENSE2_I2C_ERROR_GET() == CAPSENSE2_I2C_ERROR_NONE)
    {
        return CAPSENSE2_OK; //ToDo should not use defines from module header file
    }
    else 
    {
        return CAPSENSE2_E_COMM_FAIL; //ToDo should not use defines from module header file
    }
}

int8_t capsense2_i2c_write(uint8_t devid, uint8_t regaddr, uint8_t *regdata, uint16_t len)
{
#ifdef MIKROE_CAPSENSE2
    
	uint8_t temp_data[30], i;
	temp_data[0] = regaddr;
	for (i = 0; i < len; i++) {
		temp_data[i + 1] = regdata[i];
	}
    
    CAPSENSE2_I2C_Write(devid, temp_data, len+1);
    
    while(CAPSENSE2_I2C_IsBusy());
    
    if(CAPSENSE2_I2C_ERROR_GET() == CAPSENSE2_I2C_ERROR_NONE)
    {
        return CAPSENSE2_OK; //ToDo should not use defines from module header file
    }
    else 
    {
        return CAPSENSE2_E_COMM_FAIL; //ToDo should not use defines from module header file
    }
#else
    return CAPSENSE2_OK; //ToDo should not use defines from module header file
#endif
}
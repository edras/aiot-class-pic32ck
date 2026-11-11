#include "bme280_hal.h"

#ifdef MIKROE_BME280
#include "BME280/bme280_defs.h"
#else
uint8_t BME280_init(void){ return false; }
uint8_t BME280_get_all_sensor_data(void){ return BME280_OK; }
int16_t  BME280_get_temperature(void){ return 0; }
uint16_t BME280_get_humidity(void){ return 0; }
uint16_t BME280_get_pressure(void){ return 0; }
bool BME280_available(void){ return false; }
#endif



uint8_t bme280_i2c_hal_read(uint8_t devid, uint8_t regaddr, uint8_t *regdata, uint16_t len)
{
#ifdef MIKROE_BME280
//    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
    
    BME280_I2C_WriteRead(devid, &regaddr, 1, regdata, len);
    
//    while(transferStatus == APP_TRANSFER_STATUS_IN_PROGRESS);
    while(BME280_I2C_IsBusy());
    
    if(BME280_I2C_ERROR_GET() == BME280_I2C_ERROR_NONE)
    {
        return BME280_OK;
    }
    else 
    {
        return BME280_E_COMM_FAIL;
    }
#else
    return BME280_OK;
#endif
}

uint8_t bme280_i2c_hal_write(uint8_t devid, uint8_t regaddr, uint8_t *regdata, uint16_t len)
{
#ifdef MIKROE_BME280
	uint8_t temp_data[30], i;
	temp_data[0] = regaddr;
	for (i = 0; i < len; i++) {
		temp_data[i + 1] = regdata[i];
	}
    
//    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
    
    BME280_I2C_Write(devid, temp_data, len+1);

    
//    while(transferStatus == APP_TRANSFER_STATUS_IN_PROGRESS);
    while(BME280_I2C_IsBusy());
    
    if(BME280_I2C_ERROR_GET() == BME280_I2C_ERROR_NONE)
    {
    return BME280_OK;
}
    else 
    {
        return BME280_E_COMM_FAIL;
    }
#else
    return BME280_OK;
#endif
}

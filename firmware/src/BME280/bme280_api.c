/**
 * \file
 *
 * \brief Application implement
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip
 * Support</a>
 */

#include "bme280_api.h"
#include "bme280_defs.h"
#include "bme280_hal.h"

#ifdef MIKROE_BME280

struct bme280_data data;
struct bme280_dev device;

typedef enum
{
    APP_TRANSFER_STATUS_IN_PROGRESS,
    APP_TRANSFER_STATUS_SUCCESS,
    APP_TRANSFER_STATUS_ERROR,
    APP_TRANSFER_STATUS_IDLE,

} APP_TRANSFER_STATUS;

#endif

static bool bme280_connected = false;
uint8_t BME280_init(void)
{
#ifdef MIKROE_BME280
	uint8_t rslt = BME280_OK;

	device.dev_id   = BME280_I2C_ADDR_PRIM;
	device.intf     = BME280_I2C_INTF;
	device.read     = (bme280_com_fptr_t)bme280_i2c_hal_read;
	device.write    = (bme280_com_fptr_t)bme280_i2c_hal_write;
	device.delay_ms = (bme280_delay_fptr_t)BME280_DelayMs;
    
//    BME280_I2C_CB_REGISTER( bme280_if_I2CCallback, (uintptr_t)&transferStatus );
    
	/*Initialize BME280 */
	rslt = bme280_init(&device);

	/* Recommended mode of operation: Indoor navigation */
	rslt = bme280_get_sensor_settings(&device);
	device.settings.osr_h = BME280_OVERSAMPLING_1X;
	device.settings.osr_p = BME280_OVERSAMPLING_1X;
	device.settings.osr_t = BME280_OVERSAMPLING_1X;
	rslt = bme280_set_sensor_settings(
        (BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL), &device);
	rslt = bme280_get_sensor_settings(&device);
    
    bme280_connected = (rslt == BME280_OK);
	return rslt;
#else
    return 0;    
#endif
}

bool BME280_available(void)
{
    return bme280_connected;
}

uint8_t BME280_get_all_sensor_data(void)
{
#ifdef MIKROE_BME280
	bme280_set_sensor_mode(BME280_FORCED_MODE, &device);
	TIMER_DelayMs(20);
	return bme280_get_sensor_data(BME280_ALL, &data, &device);
#else
    return 0;
#endif
}

uint8_t BME280_sleep_mode(void)
{
#ifdef MIKROE_BME280
	bme280_set_sensor_mode(BME280_SLEEP_MODE, &device);
#endif
	return 0;
}

int16_t BME280_get_temperature(void)
{
#ifdef MIKROE_BME280
    return (int16_t)(data.temperature/10);
#else
    return 0;
#endif    
}

uint16_t BME280_get_humidity(void)
{
#ifdef MIKROE_BME280
    return (uint16_t)(data.humidity/100);
#else
    return 0;
#endif
}

uint16_t BME280_get_pressure(void)
{
#ifdef MIKROE_BME280
    return (uint16_t)(data.pressure/10);
#else
    return 0;
#endif
}



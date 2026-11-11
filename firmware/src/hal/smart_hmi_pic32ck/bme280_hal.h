/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#ifndef BME280_HAL_H_
#define BME280_HAL_H_

#include "hal.h"
#include "bus_hal.h"                // SYS function prototypes
#include "BME280/bme280_api.h"

#ifdef MIKROE_BME280

#define BME280_I2C_ERROR_GET    HAL_I2C_ErrorGet
#define BME280_I2C_ERROR_NONE   SERCOM_I2C_ERROR_NONE

//#define BME280_I2C_CB_REGISTER  HAL_I2C_CallbackRegister
#define BME280_I2C_IsBusy 		HAL_I2C_IsBusy
#define BME280_I2C_WriteRead    HAL_I2C_WriteRead
#define BME280_I2C_Write        HAL_I2C_Write

#define BME280_DelayMs          SYSTICK_DelayMs

#else

#endif

uint8_t bme280_i2c_hal_read(uint8_t devid, uint8_t regaddr, uint8_t *regdata, uint16_t len);
uint8_t bme280_i2c_hal_write(uint8_t devid, uint8_t regaddr, uint8_t *regdata, uint16_t len);


#endif /* BME280_HAL_H_ */

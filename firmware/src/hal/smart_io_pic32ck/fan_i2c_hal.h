/*******************************************************************************
 * Fan Click MIKROE-2004
 *
 * Company:
 *    Microchip Technology Inc.
 *
 *  File Name:
 *    fanclick_i2c_hal.h
 *
 *  Summary:
 *    Hardware layer definitions for Fan Click MIKROE-2004. 
 *
 *  Description:
 *    Fan Click is based on the EMS2301, a single/multiple RPM-based PWM 
 *    fan controller from Microchip. 
 * 
 ******************************************************************************/

/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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

#ifndef FAN_HAL_H_
#define FAN_HAL_H_

/* Microchip SAM Definitions by Harmony*/
#include "hal.h"
#include "bus_hal.h"
#include "FAN/fan.h"

#define FAN_I2C_ErrorGet              HAL_I2C_ErrorGet
#define FAN_I2C_ERROR_NONE            SERCOM_I2C_ERROR_NONE

// I2C Definitions
#define FAN_I2C_Write                 HAL_I2C_Write
#define FAN_I2C_WriteRead             HAL_I2C_WriteRead
#define FAN_I2C_IsBusy                HAL_I2C_IsBusy
#define FAN_I2C_Read                  HAL_I2C_Read

// Timer Definitions
#define FAN_TimerStart   			  SYSTICK_TimerStart
#define FAN_DelayMs                   SYSTICK_DelayMs

#define FAN_TERMINAL_Write            SERCOM5_USART_Write
#define FAN_TERMINAL_Done             SERCOM5_USART_TransmitComplete       

int8_t fan_i2c_read(uint8_t devid, uint8_t regaddr, uint8_t *regdata, uint16_t len);
int8_t fan_i2c_write(uint8_t devid, uint8_t regaddr, uint8_t *regdata, uint16_t len);

#endif /* FAN_HAL_H_ */

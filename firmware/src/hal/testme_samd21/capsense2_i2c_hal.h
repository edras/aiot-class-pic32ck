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

#ifndef CAPSENSE2_HL_H_
#define CAPSENSE2_HL_H_

#include "hal.h"
#include "CAPSENSE2/capsense2.h"

#ifdef MIKROE_CAPSENSE2
#define CAPSENSE2_AVAILABLE      true

#define CAPSENSE2_reset_low      RST3_Clear
#define CAPSENSE2_reset_high     RST3_Set
#define CAPSENSE2_get_alert      INT3_Get

#define CAPSENSE2_I2C_ERROR_GET    SERCOM2_I2C_ErrorGet
#define CAPSENSE2_I2C_ERROR_NONE   SERCOM_I2C_ERROR_NONE

//#define CAPSENSE2_I2C_CB_REGISTER  SERCOM2_I2C_CallbackRegister
#define CAPSENSE2_I2C_IsBusy 	   SERCOM2_I2C_IsBusy
#define CAPSENSE2_I2C_WriteRead    SERCOM2_I2C_WriteRead
#define CAPSENSE2_I2C_Write        SERCOM2_I2C_Write

#define CAPSENSE2_delay_ms          SYSTICK_DelayMs

#else

#define CAPSENSE2_AVAILABLE             false
#define CAPSENSE2_reset_low(...)        do{}while(0)
#define CAPSENSE2_reset_high(...)       do{}while(0)  
#define CAPSENSE2_get_alert(...)        0

#define CAPSENSE2_I2C_ERROR_GET(...)    0
#define CAPSENSE2_I2C_ERROR_NONE        0

//#define CAPSENSE2_I2C_CB_REGISTER  SERCOM2_I2C_CallbackRegister
#define CAPSENSE2_I2C_IsBusy(...)       0
#define CAPSENSE2_I2C_WriteRead(...)    do{}while(0)
#define CAPSENSE2_I2C_Write(...)        do{}while(0)

#define CAPSENSE2_delay_ms          SYSTICK_DelayMs

#endif

int8_t capsense2_i2c_read(uint8_t devid, uint8_t regaddr, uint8_t *regdata, uint16_t len);
int8_t capsense2_i2c_write(uint8_t devid, uint8_t regaddr, uint8_t *regdata, uint16_t len);

#endif /* CAPSENSE2_HL_H_ */

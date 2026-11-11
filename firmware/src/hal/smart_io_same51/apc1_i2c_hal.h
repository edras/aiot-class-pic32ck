/*******************************************************************************
* Copyright (C) 2023-2024 Microchip Technology Inc. and its subsidiaries.
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
#ifndef _APC1_I2C_HAL_H    /* Guard against multiple inclusion */
#define _APC1_I2C_HAL_H

#include "hal.h"

#ifdef MIKROE_APC1    /* Check if the module should be used in this project */

/* define the required macros (pins, functions) */
#define APC1_cs_low                  CS3_Clear
#define APC1_cs_high                 CS3_Set
#define APC1_rst_low                 RST3_Clear
#define APC1_rst_high                RST3_Set

#define APC1_delay_ms                TIMER_DelayMs

// I2C Definitions
#define APC1_I2C_Write               SERCOM2_I2C_Write
#define APC1_I2C_WriteRead           SERCOM2_I2C_WriteRead
#define APC1_I2C_IsBusy              SERCOM2_I2C_IsBusy
#define APC1_I2C_Read                SERCOM2_I2C_Read

#define APC1_I2C_ErrorGet            SERCOM2_I2C_ErrorGet
#define APC1_I2C_ERROR_NONE          SERCOM_I2C_ERROR_NONE

#else   /* MIKROE_APC1 not defined */

/* define the required macros (pins, functions) */
#define APC1_cs_low(...)             do{}while(0)
#define APC1_cs_high(...)            do{}while(0)
#define APC1_rst_low(...)            do{}while(0)
#define APC1_rst_high(...)           do{}while(0)

#define APC1_delay_ms(...)           do{}while(0)

// I2C Definitions
#define APC1_I2C_WRITE(...)          do{}while(0)
#define APC1_I2C_WriteRead(...)      do{}while(0)
#define APC1_I2C_IsBusy(...)         0
#define APC1_I2C_Read(...)           do{}while(0)

#define APC1_I2C_ErrorGet(...)       0
#define APC1_I2C_ERROR_NONE          0

#endif

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif
/*!
 *  @brief APC1 I2C read from sensor
 *
 *  @param[in]  devaddr : Device address
 *  @param[out] regdata: Buffer to store data
 *  @param[in]  len : Buffer length
 * 
 *  @return True if data was read correctly, false otherwise
 */
bool apc1_i2c_read(uint8_t devaddr, uint8_t *regdata, uint16_t len);

/*!
 *  @brief APC1 I2C read from specific register address
 *
 *  @param[in]  devaddr : Device address
 *  @param[in]  regaddr  : Register address
 *  @param[out] regdata: Buffer to store data
 *  @param[in]  len : Buffer length
 * 
 *  @return True if data was read correctly, false otherwise
 */
bool apc1_i2c_read_from_addr(uint8_t devaddr, uint8_t regaddr, uint8_t *regdata, uint16_t len);

/*!
 *  @brief APC1 I2C write function
 *
 *  @param[in]  devaddr : Device address
 *  @param[in]  regaddr  : Register address
 *  @param[out] regdata: Buffer of data that will be written
 *  @param[in]  len : Buffer length
 * 
 *  @return True if data was written correctly, false otherwise.
 */
bool apc1_i2c_write(uint8_t devaddr, uint8_t regaddr, uint8_t *regdata, uint16_t len);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _APC1_I2C_HAL_H */

/* *****************************************************************************
 End of File
 */
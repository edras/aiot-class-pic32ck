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
#if 0 /*GUARD AGAINST MPLAB X IDE INCLUSION */

#ifndef _MODULEXYZ_SPI_HAL_H    /* Guard against multiple inclusion */
#define _MODULEXYZ_SPI_HAL_H

#include "hal.h"

#ifdef MIKROE_MODULEXYZ    /* Check if the module should be used in this project */

/* define the required macros (pins, functions) */
#define MODULEXYZ_cs_low                  PIN_NAME_Clear
#define MODULEXYZ_cs_high                 PIN_NAME_Set
#define MODULEXYZ_delay_ms                TIMER_DelayMs

#else   /* MIKROE_MODULEXYZ not defined */

/* define stubs */
#define MODULEXYZ_cs_low(...)             do{}while(false)
#define MODULEXYZ_cs_high(...)            do{}while(false)
#define MODULEXYZ_delay_ms(...)           do{}while(false)

#endif

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief HAL API description
 *
 *  @param[in]  tx_data : Data to be sent
 *  @param[in]  tx_len  : Length of the data to be sent
 *  @param[out] rx_data : Buffer to store data
 *  @param[in]  rx_len  : Length of the data to be read
 * 
 *  @return Description e.g. "True if data was read correctly, false otherwise."
 */
bool MODULEXYZ_spi_write_read(uint8_t *tx_data, size_t tx_len, uint8_t *rx_data, size_t rx_len);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _MULTIMETER_SPI_HAL_H */

/* *****************************************************************************
 End of File
 */

#endif
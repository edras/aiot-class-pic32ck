/******************************************************************************/
/** T1S HAL HEADER                                                             */
/*******************************************************************************
 T1S

  Company:
    Microchip Technology Inc.

  File Name:
    t1s_lwip_hal.h

  Summary:
    T1S module "click" board support - header file interface
 definition

  Description:
    Generic support for the T1S Lightweight IP (LWIP) module
 ******************************************************************************/

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

#include "hal.h"

#ifndef _T1s_LWIP_HAL_H   
#define _T1s_LWIP_HAL_H

#ifdef MIKROE_T1S
#include "T1S/t1s_lwip.h"

#define T1S_BOARD_INSTANCE   BOARD_TYPE
#define T1S_AVAILABLE        true

#define T1S_RST_Clear        RST1_Clear
#define T1S_RST_Set          RST1_Set
#define T1S_CS_Clear         CS1_Clear
#define T1S_CS_Set           CS1_Set

#define T1S_INT_Get          INT1_Get

#define T1S_TERMINAL_Write   SERCOM5_USART_Write
#define T1S_TERMINAL_Done    SERCOM5_USART_TransmitComplete

#define T1S_DelayMs          SYSTICK_DelayMs
#define T1S_TIMEOUT          SYSTICK_TIMEOUT
#define T1S_StartTimeOut     SYSTICK_StartTimeOut
#define T1S_IsTimeoutReached SYSTICK_IsTimeoutReached
#define T1S_GetTickCount     SYSTICK_GetTickCounter  

// SPI Definitions
#define T1S_SPI_WriteRead    SERCOM1_SPI_WriteRead
#define T1S_SPI_IsBusy       SERCOM1_SPI_IsTransmitterBusy

// I2C Definitions
#define T1S_I2C_WriteRead    SERCOM2_I2C_WriteRead
#define T1S_I2C_IsBusy       SERCOM2_I2C_IsBusy

#define T1S_reset            __NVIC_SystemReset

void T1S_EIC_CB_Register(void (* callback)(uintptr_t context), uintptr_t context);

#else

#define T1S_AVAILABLE        false
#define T1S_available(...)   false
#define T1S_init(...)        false

#define T1S_I2C_WriteRead(...)   0
#define T1S_I2C_IsBusy(...)  0

#endif

#endif /* _T1s_LWIP_HAL_H */

/* *****************************************************************************
 End of File
 */

/* ************************************************************************** 
 *  Hardware Layer definition
 *
 *  @Company
 *    Microchip Technology Inc.
 *
 *  @File Name
 *    hw_layer.h
 *
 *  @Summary
 *    Prototypes and defines for hardware layer.
 *
 *  @Description
 *    Definitions and function prototypes of hardware related functions and 
 *    definitions.
 *
 ****************************************************************************/

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

#ifndef _HW_LAYER_H    /* Guard against multiple inclusion */
#define _HW_LAYER_H

#include "definitions.h"
#include "board_types.h"

#define BOARD_TYPE              SMART_IO_BOARD
#define BOARD_LABEL             "Smart I/O"
#define BOARD_NODE_NAME         "smart_io"   // mqtt node name, small case always

/*
 * Board Modules Available
 */

#undef MIKROE_APC1
#undef MIKROE_BME280
#undef MIKROE_CAPSENSE2
#define MIKROE_T1S
#define MIKROE_FAN_CLICK
#define MIKROE_MULTIMETER
#undef MIKROE_OLED_C
#define MIKROE_RELAY_2_CLICK
#undef MIKROE_RNBD451
#undef MIKROE_SENSIRION

#include "bme280_hal.h"
#include "sensirion_i2c_hal.h"
#include "t1s_lwip_hal.h"

/*
 * Board Timer and Terminal functions
 */

#define TIMER_SetCallback(X)    SYSTICK_TimerCallbackSet(X, 0)
#define TIMER_DelayMs           SYSTICK_DelayMs
#define TIMER_DelayUs           SYSTICK_DelayUs

#undef  TERMINAL_CALLBACK
#define TERMINAL_Write          SERCOM5_USART_Write
#define TERMINAL_TXComplete     SERCOM5_USART_TransmitComplete
#define TERMINAL_NewData        SERCOM5_USART_ReceiverIsReady
#define TERMINAL_ReadByte       SERCOM5_USART_ReadByte


//#define CONTEXT_PTR             uintptr_t context
//#define CONTEXT_PTR(context)    uintptr_t context
#define ONLINE_LED_Toggle       LED_Toggle

#define ADC1_ENABLE(...)        do{}while(false)

void HardwareLayer_init(void);
void static inline reset_device(void)
{
     NVIC_SystemReset();
}

void print_oled_data_hal(void);
uint32_t get_mcu_uuid(void);

#endif /* _HW_LAYER_H */

/* *****************************************************************************
 End of File
 */

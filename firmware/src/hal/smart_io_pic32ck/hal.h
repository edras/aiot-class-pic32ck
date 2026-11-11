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

/*
 * LED0 polarity differs between the two boards, so the shared application's
 * LED_Set()/LED_Clear() calls have to be swapped here.
 *
 *   SAM E51 Curiosity Nano (EV76S68A, DS70005432B 4.2.1): LED0 on PA14 is
 *   ACTIVE LOW - "The LED can be activated by driving the connected I/O line
 *   to GND."
 *   PIC32CK SG01 Curiosity Nano (EV33V87A, DS50004178A 7.2): LED0 on PD05 is
 *   ACTIVE HIGH - "Driving the connected I/O line to [...] VDDIO will activate
 *   the LED."
 *
 * app.c's led_sub_cb() lights the LED with LED_Clear() and extinguishes it with
 * LED_Set(), i.e. it is written against the active-low board. Mapping those two
 * onto the opposite drive level keeps the MQTT "curiosity/led" topic meaning the
 * same thing on both targets. ONLINE_LED_Toggle is polarity-agnostic and needs
 * no adjustment: both boards boot with LED0 dark (PA14 latched high, PD05
 * latched low), so the 50 ms pulse in blink_online_led() looks identical.
 */
#undef  LED_Set
#undef  LED_Clear
#define LED_Set()               PORT_PinClear(LED_PIN)
#define LED_Clear()             PORT_PinSet(LED_PIN)

#define ONLINE_LED_Toggle       LED_Toggle

#define ADC1_ENABLE(...)        do{}while(false)

void HardwareLayer_init(void);
void static inline reset_device(void)
{
     NVIC_SystemReset();
}

void print_oled_data_hal(void);
uint32_t get_mcu_uuid(void);

/*
 * SW0 is polled rather than interrupt-driven on PIC32CK: PD02 carries the
 * functions NMI/TSUCOMP/DEN2/SOF_1KHZ and no EXTINT at all, so the SAME51
 * EIC_PIN_15 registration has no equivalent. main.c calls this from the
 * superloop when HAL_HAS_POLL_BUTTON is defined.
 */
void HAL_PollButton(void);
#define HAL_HAS_POLL_BUTTON

#endif /* _HW_LAYER_H */

/* *****************************************************************************
 End of File
 */

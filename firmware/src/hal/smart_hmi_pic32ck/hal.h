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

#define BOARD_TYPE              SMART_HMI_BOARD
#define BOARD_LABEL             "Smart HMI"
#define BOARD_NODE_NAME         "smart_hmi"   // mqtt node name, small case always

/*
 * Board Modules Available
 */

#undef MIKROE_APC1
#undef MIKROE_BME280
#define MIKROE_CAPSENSE2
#define MIKROE_T1S
#undef MIKROE_FAN_CLICK
#undef MIKROE_MULTIMETER
#define MIKROE_OLED_C
#undef MIKROE_RELAY_2_CLICK
#define MIKROE_RNBD451
#undef MIKROE_RE46C42X
#undef MIKROE_SENSIRION

#include "bme280_hal.h"
#include "sensirion_i2c_hal.h"
#include "t1s_lwip_hal.h"

/*
 * Board Timer and Terminal functions
 */

#define TIMER_SetCallback(X)    SYSTICK_TimerCallbackSet(X, 0)
#define TIMER_DelayMs           SYSTICK_DelayMs

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

#define ADC1_ENABLE(...)        do{}while(false)
/*
 * LED0 polarity differs between the two boards, so the shared application's
 * LED_Set()/LED_Clear() calls have to be swapped here. LED0 is ACTIVE LOW on the
 * SAM E51 Curiosity Nano (PA14, DS70005432B 4.2.1) and ACTIVE HIGH on the
 * PIC32CK SG01 Curiosity Nano (PD05, DS50004178A 7.2). app.c's led_sub_cb()
 * lights the LED with LED_Clear() and extinguishes it with LED_Set(), i.e. it is
 * written against the active-low board, so mapping those two onto the opposite
 * drive level keeps the MQTT "curiosity/led" topic meaning the same thing on both
 * targets. ONLINE_LED_Toggle is polarity-agnostic and needs no adjustment.
 */
#undef  LED_Set
#undef  LED_Clear
#define LED_Set()               PORT_PinClear(LED_PIN)
#define LED_Clear()             PORT_PinSet(LED_PIN)

#define ONLINE_LED_Toggle       LED_Toggle

void HardwareLayer_init(void);
void static inline reset_device(void)
{
     NVIC_SystemReset();
}

void print_oled_data_hal(void);
uint32_t get_mcu_uuid(void);

/*
 * SW0 is polled rather than interrupt-driven on PIC32CK: PD02 carries the
 * functions NMI/TSUCOMP/DEN2/SOF_1KHZ and no EXTINT at all. main.c calls this
 * from the superloop when HAL_HAS_POLL_BUTTON is defined.
 */
void HAL_PollButton(void);
#define HAL_HAS_POLL_BUTTON

/*
 * ACCEPTED DIFFERENCE FROM smart_hmi_same51: CapSense touch input is INACTIVE.
 *
 * capsense2_i2c_hal.h maps CAPSENSE2_get_alert to INT3_Get, and app.c's
 * capsense2_check_state() returns early unless that reads true - so the alert
 * line gates ALL touch handling: buttons, slider and the capsense MQTT
 * publishes. INT3 (PA00) is configured as a GPIO output on PIC32CK, which leaves
 * PINCFG.INEN clear, so INT3_Get() always reads 0. The generated
 * INT3_InputEnable() macro cannot fix this at run time - it only clears DIR, it
 * does not set PINCFG.INEN.
 *
 * SAME51 configures INT3/PB07 as a digital input, which is why touch works
 * there. To enable it here, change PA00 to a digital input in the MCC pin
 * configuration; nothing else needs to change, because PIC32CK emits the full
 * GPIO macro set regardless of direction. Note PA00 is also smart_sensor's
 * RE46C42X chip select, so the two uses conflict in the shared pin
 * configuration - though that chip select does not drive on SAME51 either, since
 * INT3 is an input there and nothing calls INT3_OutputEnable().
 */

#endif /* _HW_LAYER_H */

/* *****************************************************************************
 End of File
 */

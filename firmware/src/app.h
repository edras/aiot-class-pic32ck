/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology Inc.

  @File Name
    app.h

  @Summary
    Application function prototypes declaration.

  @Description
    High level application functions.
 */
/* ************************************************************************** */

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

#ifndef _APP_H    /* Guard against multiple inclusion */
#define _APP_H

#include "APC1/apc1.h"
#include "BME280/bme280_api.h"
#include "CAPSENSE2/capsense2.h"
#include "FAN/fan.h"
#include "MULTIMETER/multimeter.h"
#include "OLED/oled.h"
#include "RE46C42X/re46c42x.h"
#include "RELAY2/relay2.h"
#include "RNBD451/rnbd451.h"
#include "sensirion/sensirion_api.h"
#include "PROXIMITY3/proximity3.h"
#include "T1S/t1s_lwip.h"
#include "MQTT/mqtt_topics.h"

// states for main state machine
typedef enum app_state
{
    STATE_APP_RXTX,
    STATE_APP_TERMINAL,
}
APP_MODE;

#define BUTTON_PRESSED  0xFF
#define BUTTON_RELEASED 0x00
#define SMOKE_DETECTED  0xFF
#define SMOKE_FREE      0x00
#define BUTTON_LONGPRESS_RESET  5000  // milliseconds

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

void init_modules(void);
void init_mqtt_topics(void);
void read_sensor_data(void);
void fast_sensor_read(void);
void publish_sensor_data(void);
void publish_csv_data(void);
void check_button(void);
void handle_button(uintptr_t context);
void handle_USART_cmd(void);
void print_header(bool init_header);
void toggle_bridge_mode(void);
void toggle_debug_mode(unsigned char nr);
void print_message(char*);
void print_weather(void);
void print_oled_data();
void capsense2_check_state(void);
void check_rnbd_data(void);
void check_button_longPress_reset(void);
void smoke_react(void);
void blink_online_led(void);

APP_MODE get_app_state(void);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _APP_H */

/* *****************************************************************************
 End of File
 */

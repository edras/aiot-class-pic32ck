/*******************************************************************************
  App Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the "application" level functions for the project.

  Description:
    This file contains the application functions for a project at their higher
    level.
 *******************************************************************************/
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
#include "app.h"
#include "terminal.h"
#include "MQTT/mqtt_topics.h"
#include "mqtt_topics_hal.h"

#include <string.h>
#include <stdio.h>

/** MACRO used to configure the application used buffer sizes.
 *  This is used by the application for communication buffers.
 */
#define MAX_BUFFER_SIZE                 (80)
/**< Status Buffer instance passed to RNBD drive used for Asynchronous Message Handling (see *asyncBuffer in rnbd.c) */
static char statusBuffer[MAX_BUFFER_SIZE];

uint16_t button_timer = 0;
bool button_set = false;
bool button_state = true;
bool bridge_active = false;
bool csv_active = false;
bool debug_active = false;
bool mqtt_connected = false;
bool smoke_detected = false;
bool stand_alone = false;

uint16_t banner_color = GREEN;
char msg[100];


/* local function prototypes */
void led_sub_cb(char* data);
void switch2str(char* data);
void fwrev2str(char* data);
void mcuid2str(char* data);

/* module functions implementation */

void init_modules(void)
{
    TERMINAL_init();
    BME280_init();
    CAPSENSE2_init();
    fan_init();
    APC1_init();
    if (!APC1_available())
    {
        sensirion_init();
    }
    RE46C42X_init();
    MULTIMETER_init();
    vcnl4200_init();
    OLEDC_initialize();    // Ensure this call is done before RNBD_Init()
    if (RNBD_init() == RNBD_OK)
    {
        RNBD_set_async_message_handler(statusBuffer, (uint8_t)sizeof(statusBuffer));
        RNBD_enter_cmd_mode();
        TIMER_DelayMs(100);
        RNBD_set_name(BOARD_LABEL, sizeof (BOARD_LABEL));
        TIMER_DelayMs(100);
        RNBD_enter_data_mode();
        TIMER_DelayMs(100);
    }
    T1S_init();
}

void init_mqtt_topics()
{
    // publish mcu uid and firmware version only at start: autoPublish = false
    TopicItem mcu_uid = {"curiosity", "mcu_uid", mcuid2str, NULL, true};
    MQTT_insert_topic(mcu_uid);
    TopicItem revision = {"curiosity", "firmware", fwrev2str, NULL, true};
    MQTT_insert_topic(revision);

    TopicItem item;
    item.nodename = "curiosity";        // the name of the module, i.e.: mikroe-xyz
    item.topic_name = "sw0";            // the parameter name of sensor property, i.e.: temperature
    item.get_str_value = switch2str;    // a function to return a string representation of the value
    item.sub_callback = NULL;           // a callback to handle mqtt messages when subscribed to a mqtt broker
    item.autoPublish = false;           // should this topic be published periodically?
    MQTT_insert_topic(item);

    // to check if the topic registration was correct, MQTT_insert_topic
    // returns true if it was successful, false otherwise. Here we insert the
    // led0 subscription topic into the queue.
    TopicItem item2 = {"curiosity", "led0", NULL, led_sub_cb, false};
    if(!MQTT_insert_topic(item2))
    {
        printf("Error inserting topic on the mqtt topics table.");
        return;
    }

    // Initialize additional topics related to this hardware/board
    MQTT_init_topics_hal();
}

void fwrev2str(char* data)
{
    static const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char monStr[4];
    int day, year, mon;

    sscanf(__DATE__, "%3s %d %d", monStr, &day, &year);
    for (mon = 0; mon < 12; mon++)
    {
        if (strncmp(monStr, months[mon], 3) == 0)
        {
            break;
        }
    }
    sprintf(data, "%04d-%02d-%02d", year, mon + 1, day);
}

void mcuid2str(char *data) {

    uint32_t mcu_id = get_mcu_uuid();
    sprintf(data, "%08X", (unsigned int)mcu_id);
}

void switch2str(char* data)
{
    strcpy(data, button_state?"false":"true");
}

void led_sub_cb(char* data)
{
    if(strncmp(data, "true", 4) == 0 || strncmp(data, "ff", 2) == 0)
    {
        LED_Clear();
    }
    else
    {
        LED_Set();
    }
}

void handle_button(uintptr_t context)
{
    button_set = true;

    if (BUTTON_Get() == 0)
    {
        button_state = false;
    }
    else
    {
        button_state = true;
    }
}

void check_button_longPress_reset(void)
{
    if(button_state)
    {
        button_timer = 0;
    }
    else
    {
        button_timer++;
        if(button_timer > BUTTON_LONGPRESS_RESET) reset_device();
    }
}

bool RNBD_send_msg(char *msg)
{
    bool isConnected,isOTAComplete;
    print_message("\r\n Trying to send BT message \r\n" );

    //check connection status
    if (!RNBD_is_connected())
        RNBD_check_connection();

    isConnected = RNBD_is_connected();
    isOTAComplete = RNBD_is_OTA_complete();
    if (true == isConnected && false == isOTAComplete)
    {
        RNBD_write(msg);
    }

    return isConnected;
}

void read_sensor_data(void)
{
    if(BME280_available()) BME280_get_all_sensor_data();
    if(APC1_available())   APC1_get_measurement();
    if(sensirion_available()) sensirion_read_data();
}

void print_oled_data(void)
{
    if(oledc_is_any_message()) return;

    if(!stand_alone)
    {
        if(mqtt_connected)  oledc_draw_string_on_bg(2, 2, 1, 2, "    ONLINE     ", banner_color, BLACK);
        else                oledc_draw_string_on_bg(2, 2, 1, 2, " CONNECTING... ", banner_color, BLACK);
    }

    print_oled_data_hal();
}

void publish_sensor_data(void)
{
    /* Ensure that this function is called timely and only once */
    mqtt_connected = MQTT_available();
    if(!mqtt_connected) return;
    MQTT_publish_topics();
}

void check_button(void)
{
    if(button_set == true)
    {
        button_set = false;
        if(!mqtt_connected) return;
        TopicItem item = {"curiosity", "sw0"};
        MQTT_publish_topic(item);
    }
}

void capsense2_check_state()
{
    uint8_t status;

    if (!CAPSENSE2_available() || capsense2_check_alert_pin() == false)
        return;

    CAPSENSE2_execute();
    if(!CAPSENSE2_action_required())
        return;

     if(!mqtt_connected) return;

    status = CAPSENSE2_get_s1_action();
    if(status != CAPSENSE2_NONE)
    {
        MQTT_publish_capsens(1, status);
    }
    status = CAPSENSE2_get_s2_action();
    if(status != CAPSENSE2_NONE)
    {
        MQTT_publish_capsens(2, status);
    }
    status = CAPSENSE2_get_slider_action();
    if(status != CAPSENSE2_NONE)
    {
        status = CAPSENSE2_get_slider_value();
        MQTT_publish_capsens(3, status);
    }
}

APP_MODE get_app_state(void)
{
    return bridge_active ? STATE_APP_TERMINAL : STATE_APP_RXTX;
}

void print_message(char *message)
{
    TERMINAL_Write(message, strlen(message));
    while(!TERMINAL_TXComplete());
}

void print_header(bool init_header)
{
    if(csv_active) return;

    // send welcome message to terminal
    print_message("\e[1;1H\e[2J");
    if(init_header)
    {
        sprintf(msg, "Sensor");
    }
    else if(T1S_available())
    {
        sprintf(msg, "T1S LWIP");
    }
    else
    {
        sprintf(msg, "Stand-alone Sensor");
        stand_alone = true;
    }
    print_message(msg);
    sprintf(msg, " Demo with %s\r\nVersion %s - %s\r\n", BOARD_LABEL, __DATE__, __TIME__);
    print_message(msg);
    print_message("Curiosity Nano Board > 1[COMM Module] 2[Weather Click/OLED] 3[HVAC Click]\r\n");

    if(init_header)
    {
        print_message("\r\nInitializing COMM module... \r\n");
    }
    else
    {
        print_message("\r\nProgram Running: \r\n");
        print_message("- Each 5s: Publish weather data to topics 2,5, and 6 (T/H/CO2)\r\n");
        print_message("- Each 1s: Read topic 4 and replicate its value on LED\r\n");
        print_message("- User interaction: Topic 3 will be updated by key press/release\r\n");
        sprintf(msg, "\r\nTotal registered topics: %d\r\n", MQTT_get_number_of_topics());
        print_message(msg);
        print_message("\r\nType following commands to interact:\r\n\r\n");
        print_message(" b - Bridge mode (send commands to COMM module, press 'b' again to exit)\r\n");
        print_message(" d - Debug mode (output module control)\r\n");
        print_message(" w - Print weather info \r\n");
        print_message(" v - Output CSV format \r\n");
        print_message(" c - Clear screen\r\n");
    }
}

void toggle_csv_mode(void)
{
    csv_active ^= 1;
    if(!csv_active) print_header(false);
}

void toggle_debug_mode(unsigned char nr)
{
    debug_active ^= 1;
    switch(nr)
    {
        case '1': T1S_set_debug(debug_active);    break;
        case '2': RNBD_set_debug(debug_active);   break;
        case '3': RE46C42X_set_debug(debug_active); break;
        case '4': vcnl4200_set_debug(debug_active); break;
        case '5': APC1_set_debug(debug_active);   break;
        default:
            debug_active ^= 1;
            sprintf(msg, "\r\nd1 - T1S\r\nd2 - RNDB\r\nd3 - RE46C42X\r\nd4 - VCNL4200\r\nd5 - APC1\r\n");
            print_message(msg);
    }

    sprintf(msg, "\r\nDebug mode turned %s\r\n", debug_active ? "ON":"OFF");
    print_message(msg);
    if(!T1S_available())
    {
        sprintf(msg, "\r\nCOMM Module is not available\r\n");
        print_message(msg);
    }
}

void toggle_bridge_mode(void)
{
    bridge_active ^= 1;
    T1S_set_bridge(bridge_active);

    sprintf(msg, "\r\nBridge mode turned %s\r\n", bridge_active ? "ON":"OFF");
    print_message(msg);

    if(!bridge_active) return;

    if(T1S_available())
    {
        T1S_print_menu();
    }
    else
    {
        sprintf(msg, "\r\nCOMM Module is not available\r\n");
        print_message(msg);
    }
}

void publish_csv_data(void)
{
    /**
     * CSV format defined as:
     * Protocol; Name; Parameter; Value; Unit; CRLF
     * Protocol: Out
     * Name: The sensor name
     * Parameter: the parameter name, i.e.: Temperature
     */
    if(!csv_active) return;
    TopicItem* item = NULL;
    char tmp[30];
    while((item = MQTT_get_next_topic(item)) != NULL)
    {
        if(item->get_str_value)
        {
            item->get_str_value(tmp);
            if(tmp[0] == '\0') continue;
            sprintf(msg, "Out;%s;%s;%s;\r\n", item->nodename, item->topic_name, tmp);
            print_message(msg);
        }
    }
}

void print_weather(void)
{
    print_message("\r\n");
    if (APC1_available())
    {
        APC1_print_data();
    }
    else
    {
        sensirion_print_data();
    }
    if(BME280_available())
    {
        sprintf(msg, "**** BME280 - Bosch sensor ****\r\n");
        print_message(msg);
        sprintf(msg, "BME280 Temperature: %.1f ?C\r\n", (float)BME280_get_temperature()/10);
        print_message(msg);
        sprintf(msg, "BME280 Humidity: %.1f %%RH\r\n", (float)BME280_get_humidity()/10);
        print_message(msg);
        sprintf(msg, "BME280 Air Pressure: %.1f hPA\r\n", (float)BME280_get_pressure()/10);
        print_message(msg);
    }
}

void handle_USART_cmd(void)
{
    if(TERMINAL_has_new_input())
    {
        char* buffer = TERMINAL_get_buffer();

        if(buffer[0] == 'b')
        {
            toggle_bridge_mode();
        }
        else if(bridge_active)
        {
            if(T1S_available())
            {
                T1S_send_cmd(buffer);
            }
        }
        else
        {
            switch(buffer[0])
            {
                case 'd': toggle_debug_mode(buffer[1]);  break;
                case 'v': toggle_csv_mode();    break;
                case 'c': print_header(false);  break;
                case 'w': print_weather();      break;
                case 'a': sprintf(msg, "Test\n");print_message(msg);      break;
                default: break;
            }
        }
    }
}

void check_rnbd_data()
{
    char text_ble[20];
    uint8_t cnt=0;

    if(RNBD_is_connected() == false)
    {
        return;
    }

    if (RNBD_data_ready())
    {
        char message;
        do
        {
            message = RNBD_read();
            text_ble[cnt++] = message;
            if(cnt == 20) cnt--;
        }
        while((message!='%'));
        text_ble[cnt-1] = '\0';

        sprintf(msg, "> %s          ", text_ble);
        oledc_draw_string_on_bg(2, 80, 1, 2, msg, WHITE, BLACK);
        if(mqtt_connected)
        {
            MQTT_publish_ble_text(text_ble);
        }
        if(debug_active)
        {
            sprintf(msg, "BLE: %s\r\n", text_ble);
            print_message(msg);
        }
    }
}

void fast_sensor_read(void)
{
    smoke_detected = RE46C42X_is_smoke_detected();
    vcnl4200_read_data();
}

void smoke_react()
{
    if (smoke_detected)
    {
        RE46C42X_rled_on();
//        RE46C42X_horn_on(); //uncomment if you would like to have the horn on
        sprintf(msg, "WARNING --- SMOKE DETECTED\r\n");
        print_message(msg);
        oledc_show_warning("WARNING!","SMOKE","DETECTED");
//        RE46C42X_horn_off();
        RE46C42X_rled_off();
    }
}

void blink_online_led(void)
{
    if(!mqtt_connected) return;
    ONLINE_LED_Toggle();
    TIMER_DelayMs(50);
    ONLINE_LED_Toggle();
}

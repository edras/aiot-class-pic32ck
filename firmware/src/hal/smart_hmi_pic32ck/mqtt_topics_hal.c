/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology Inc.

  @File Name
    mqtt_topics_hal.c

  @Summary
    Implementation of topics layer functions.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "MQTT/mqtt_topics.h"
#include "app.h"

/* private function prototypes */
void printMqttOledMessage(bool alarm, bool message);
void handle_target_ptr(char *data, void (*func_true)(), void (*func_false)());
void handle_alarm(char *data);
void handle_mcu2bt(char *data);

void capsens_s1_str(char *data);
void capsens_s2_str(char *data);
void capsens_level_str(char *data);
void bt2mcu_str(char *data);

void MQTT_init_topics_hal(void)
{
    TopicItem items[] = {
        {"mikroe4982", "s1",      capsens_s1_str,     NULL, true},
        {"mikroe4982", "s2",      capsens_s2_str,     NULL, true},
        {"mikroe4982", "level",   capsens_level_str,  NULL, true},
        {"mikroe5545", "oled",    NULL,               handle_alarm, true},
        {"rnbd451",    "mcu2bt",  NULL,               handle_mcu2bt, true},
        {"rnbd451",    "bt2mcu",  bt2mcu_str,         NULL, true}
    };
    for (uint8_t i = 0; i < 6; i++) MQTT_insert_topic(items[i]);
}

/* helper function prototypes */
void ascii2hex(char *output, char *input);
void hex2ascii(char *output, char *input);
bool checkHexValidString(char* input, char* output);

uint8_t button_s1 = false;
uint8_t button_s2 = false;
int8_t button_level = 0;
char ble_text[20];

void MQTT_publish_capsens(uint8_t button, uint8_t status)
{
    TopicItem item = {"mikroe4982", "s1"};
    if(button == 1) // switch S1
    {
        button_s1 = (status == CAPSENSE2_PRESS) + 1;
    }
    if(button == 2) // switch S2
    {
        button_s2 = (status == CAPSENSE2_PRESS) + 1;
        item.topic_name = "s2";
    }
    if(button == 3) // slider
    {
        button_level = status;
        item.topic_name = "level";
    }
    MQTT_publish_topic(item);
}

void capsens_s1_str(char *data)
{
    data[0] = '\0';
    if(button_s1 == 0) return;
    button_s1--;
    strcpy(data, button_s1?"true":"false");
    button_s1 = 0;
}
void capsens_s2_str(char *data)
{
    data[0] = '\0';
    if(button_s2 == 0) return;
    button_s2--;
    strcpy(data, button_s2?"true":"false");
    button_s2 = 0;
}
void capsens_level_str(char *data)
{
    data[0] = '\0';
    if(button_level < 0) return;
    sprintf(data, "%d", button_level);
    button_level = -1;
}

void handle_target_ptr(char *data, void (*func_true)(), void (*func_false)())
{
    if(data == NULL) return;
    if(strncmp(data, "ff", 2) == 0 || strncmp(data, "true", 4) == 0)
    {
        func_true();
    }
    else
    {
        func_false();
    }
}

void handle_alarm(char *data)
{
    if(data == NULL) return;
    if(strncmp(data, "ff", 2) == 0 || strncmp(data, "true", 4) == 0)
    {
        oledc_show_warning("Attention!", "", "");
    }
    else
    {
        oledc_clear_warning();
    }
}

void handle_mcu2bt(char *data)
{
    if(data == NULL) return;
    strcpy(ble_text, data);
    if(strncmp(ble_text, "00", 2) == 0)
    {
        ble_text[0] = '\0';
        return;
    }
    RNBD_write(ble_text);
}

void MQTT_publish_ble_text(char *text)
{
    strcpy(ble_text, text);
    TopicItem item = {"rnbd451", "bt2mcu"};
    MQTT_publish_topic(item);
}

void printMqttOledMessage(bool alarm, bool message)
{
    if(alarm)
    {
        oledc_show_warning("Attention!", "", "");
    }
    else if(message)
    {
        oledc_show_message("", "", "");
    }
    else
    {
        oledc_clear_message();
        oledc_clear_warning();
    }
}

void bt2mcu_str(char *data)
{
    data[0] = '\0';
    if(strlen(ble_text) == 0) return;
    sprintf(data, "%s", ble_text);
    ble_text[0] = '\0';
}

/* HELPER FUNCTIONS */

void ascii2hex(char *output, char *input)
{
    int loop, i;
    i = 0;
    loop = 0;

    while (input[loop] != '\0') {
        sprintf((char*)(output + i), "%02X", input[loop]);
        loop += 1;
        i += 2;
    }
    //insert NULL at the end of the output string
    output[i++] = '\0';
}

void hex2ascii(char *output, char *input) {
    unsigned int value;
    while(sscanf(input, "%2x", &value) == 1)
    {
        *output++ = value;
        input += 2;
    }
    *output = '\0';
}

/* *****************************************************************************
 End of File
 */

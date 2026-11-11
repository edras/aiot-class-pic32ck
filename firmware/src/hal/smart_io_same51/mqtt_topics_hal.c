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
#include "MULTIMETER/multimeter.h"

/* private function prototypes */
void handle_target_ptr(char *data, void (*func_true)(), void (*func_false)());
void handle_CH1(char *data);
void handle_CH2(char *data);
void handle_fan_state(char *data);
void handle_fan_speed(char *data);
void stall_to_str(char *data);
void rpm_to_str(char *data);
void ohm_to_str(char *data);
void amp_to_str(char *data);
void volt_to_str(char *data);
void farad_to_str(char *data);

void MQTT_init_topics_hal(void)
{
    printf("init hal\r\n");
    TopicItem topics[] = {
        {"mikroe2004", "state", NULL,               handle_fan_state, true},
        {"mikroe2004", "speed", NULL,               handle_fan_speed, true},
        {"mikroe2004", "stall", stall_to_str,       NULL, true},
        {"mikroe2004", "rpm",   rpm_to_str,         NULL, true},
        {"mikroe1899", "ch1",   NULL,               handle_CH1, true},
        {"mikroe1899", "ch2",   NULL,               handle_CH2, true},
    };
    for(uint8_t i = 0; i < 6; i++) MQTT_insert_topic(topics[i]);
    printf("exit hal\r\n");
}

bool fan_state = false;
uint8_t fan_speed = 0;

void MQTT_publish_capsens(uint8_t button, uint8_t status)
{

}

void MQTT_publish_ble_text(char *text_ble)
{

}

void MQTT_handle_target_ptr(char *data, void (*func_true)(), void (*func_false)())
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

void handle_CH1(char *data)
{
    MQTT_handle_target_ptr(data, RELAY2_relay1_on, RELAY2_relay1_off);
}

void handle_CH2(char *data)
{
    MQTT_handle_target_ptr(data, RELAY2_relay2_on, RELAY2_relay2_off);
}

void handle_fan_state(char *data)
{
    fan_cfg_t *fan_cfg = fan_get_config();
    if(strncmp("true", data, 4) == 0 || strncmp("ff", data, 2) == 0)
    {
        fan_state = true;
        fan_set_duty_cycle_percentage(fan_cfg, fan_speed);
    }
    else
    {
        fan_state = false;
        fan_set_duty_cycle_percentage(fan_cfg, 0);
    }
}

void handle_fan_speed(char *data)
{
   if(data == NULL) return;
   fan_speed = atoi(data);

   if(!fan_state) return;
   fan_cfg_t *fan_cfg = fan_get_config();
   fan_set_duty_cycle_percentage(fan_cfg, fan_speed);
}

void stall_to_str(char *data)
{
    uint8_t stall_detect;
    fan_cfg_t *fan_cfg = fan_get_config();
    fan_get_stall_status(fan_cfg, &stall_detect);
    strcpy(data, stall_detect?"true":"false");
}
void rpm_to_str(char *data)
{
    uint16_t rpm_detect;
    fan_cfg_t *fan_cfg = fan_get_config();
    fan_get_tach(fan_cfg, &rpm_detect);
    sprintf(data, "%d", rpm_detect);
}

/* MULTIMETER CALLBACKS */
void ohm_to_str(char *data)
{
    if(!MULTIMETER_available())
    {
        strcpy(data, "N/A");
        return;
    }
    float resistance = MULTIMETER_measure_R();
    sprintf(data, "%.2f", resistance);
}

void amp_to_str(char *data)
{
    if(!MULTIMETER_available())
    {
        strcpy(data, "N/A");
        return;
    }
    float current = MULTIMETER_measure_I();
    sprintf(data, "%.2f", current);
}

void volt_to_str(char *data)
{
    if(!MULTIMETER_available())
    {
        strcpy(data, "N/A");
        return;
    }
    float voltage = MULTIMETER_measure_U();
    sprintf(data, "%.2f", voltage);
}

void farad_to_str(char *data)
{
    if(!MULTIMETER_available())
    {
        strcpy(data, "N/A");
        return;
    }
    float capacitance = MULTIMETER_measure_C();
    sprintf(data, "%.2f", capacitance);
}

/* HELPER FUNCTIONS */






/* *****************************************************************************
 End of File
 */

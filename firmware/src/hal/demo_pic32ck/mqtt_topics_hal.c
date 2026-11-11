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

#include "mqtt_topics_hal.h"
#include "app.h"

void apc1_get_temp_str(char *data);
void apc1_get_mois_str(char *data);
void apc1_get_voc_str(char *data);
void apc1_get_co2_str(char *data);
void apc1_get_aqi_str(char *data);
void apc1_get_pm1_str(char *data);
void apc1_get_pm25_str(char *data);
void apc1_get_pm10_str(char *data);
void sen5x_get_temp_str(char *data);
void sen5x_get_mois_str(char *data);
void sen5x_get_voc_str(char *data);
void sen5x_get_nox_str(char *data);
void sen5x_get_pm1_str(char *data);
void sen5x_get_pm25_str(char *data);
void sen5x_get_pm10_str(char *data);
void sen5x_get_pm4_str(char *data);
void scd4_get_co2_str(char *data);
void scd4_get_temp_str(char *data);
void scd4_get_moist_str(char *data);
void bme280_get_temp_str(char *data);
void bme280_get_moist_str(char *data);
void bme280_get_press_str(char *data);
void vcnl4200_get_dist_str(char *data);
void vcnl4200_get_light_str(char *data);

void MQTT_init_topics_hal(void)
{
    uint i = 0;
    if(APC1_available())
    {
        TopicItem apc1[] = {
            {"mikroe5908", "temperature", apc1_get_temp_str, NULL, true},
            {"mikroe5908", "moisture",    apc1_get_mois_str, NULL, true},
            {"mikroe5908", "voc",         apc1_get_voc_str,  NULL, true},
            {"mikroe5908", "co2",         apc1_get_co2_str,  NULL, true},
            {"mikroe5908", "aqi",         apc1_get_aqi_str,  NULL, true},
            {"mikroe5908", "pm1",         apc1_get_pm1_str,  NULL, true},
            {"mikroe5908", "pm25",        apc1_get_pm25_str, NULL, true},
            {"mikroe5908", "pm10",        apc1_get_pm10_str, NULL, true},
        };
        for(i = 0; i<sizeof(apc1)/sizeof(TopicItem); i++)
            MQTT_insert_topic(apc1[i]);
    }
    if(sensirion_sen5x_available())
    {
        TopicItem sen5[] = {
            {"sen5x", "temperature", sen5x_get_temp_str, NULL, true},
            {"sen5x", "moisture",    sen5x_get_mois_str, NULL, true},
            {"sen5x", "voc",         sen5x_get_voc_str,  NULL, true},
            {"sen5x", "nox",         sen5x_get_nox_str,  NULL, true},
            {"sen5x", "pm1",         sen5x_get_pm1_str,  NULL, true},
            {"sen5x", "pm25",        sen5x_get_pm25_str, NULL, true},
            {"sen5x", "pm10",        sen5x_get_pm10_str, NULL, true},
            {"sen5x", "pm4",         sen5x_get_pm4_str,  NULL, true},
        };
        for(i = 0; i < sizeof(sen5)/sizeof(TopicItem); i++)
            MQTT_insert_topic(sen5[i]);
    }
    if(sensirion_scd4x_available())
    {
        TopicItem scd4[] = {
            {"mikroe4290", "CO2",         scd4_get_co2_str,   NULL, true},
            {"mikroe4290", "temperature", scd4_get_temp_str,  NULL, true},
            {"mikroe4290", "moisture",    scd4_get_moist_str, NULL, true},
        };
        for(i = 0; i < sizeof(scd4)/sizeof(TopicItem); i++)
            MQTT_insert_topic(scd4[i]);
    }

    if(BME280_available())
    {
        char* bme280 = "mikroe5761";
        TopicItem item = {bme280, "temperature", bme280_get_temp_str, NULL, true};
        MQTT_insert_topic(item);

        item.topic_name = "moisture";
        item.get_str_value = bme280_get_moist_str;
        MQTT_insert_topic(item);

        item.topic_name = "pressure";
        item.get_str_value = bme280_get_press_str;
        MQTT_insert_topic(item);
    }
    if(vcnl4200_available())
    {
        TopicItem scd4[] = {
            {"mikroe2801", "distance", vcnl4200_get_dist_str,   NULL, true},
            {"mikroe2801", "light",    vcnl4200_get_light_str,  NULL, true},

        };
        for(i = 0; i < sizeof(scd4)/sizeof(TopicItem); i++)
            MQTT_insert_topic(scd4[i]);
    }
}
void MQTT_publish_capsens(uint8_t button, uint8_t status)
{

}
void MQTT_publish_ble_text(char *text_ble)
{

}
/* private function prototypes */
void MQTT_handle_target_ptr(char *data, void (*func_true)(), void (*func_false)());

/*
 * Define here the topic names, subscription and subscription callbacks to the topics
 * defined at Topics enumeration at mqtt_topics_hal.h.
 */

void MQTT_handle_target_ptr(char *data, void (*func_true)(), void (*func_false)())
{
    if(data == NULL) return;
    if((data[0] == 0xFF) || (strcmp(data, "true")) == 0)
    {
        func_true();
    }
    else
    {
        func_false();
    }
}
/* HELPER FUNCTIONS */

void apc1_get_temp_str(char *data)
{
    apc1_measurement *apc1 = APC1_get_data();
    sprintf(data, "%.1f", apc1->t_comp);
}
void apc1_get_mois_str(char *data)
{
    apc1_measurement *apc1 = APC1_get_data();
    sprintf(data, "%.0f", apc1->rh_comp);
}
void apc1_get_voc_str(char *data)
{
    apc1_measurement *sensor = APC1_get_data();
    sprintf(data, "%d", sensor->tvoc);
}
void apc1_get_co2_str(char *data)
{
    apc1_measurement *sensor = APC1_get_data();
    sprintf(data, "%d", sensor->eco2);
}
void apc1_get_aqi_str(char *data)
{
    apc1_measurement *sensor = APC1_get_data();
    sprintf(data, "%d", sensor->aqi);
}
/*
 * These topics are named pm1/pm25/pm10 and are consumed as ug/m3, so they must
 * publish the mass-concentration fields, not the particle counts. APC1 datasheet
 * SC-001581-DS-4 Table 13: PM1.0/PM2.5/PM10 mass concentration live at frame
 * offsets 0x04/0x06/0x08 in ug/m3 (ranges 0-500, 0-1000, 0-1500), whereas
 * "# particles >0.3/0.5/1.0um" at 0x10/0x12/0x14 are cumulative counts per 0.1 L
 * of air spanning 0-65535. Publishing the counts produced readings of several
 * thousand that fell as the size threshold rose - PM1.0 above PM10, which is
 * impossible for cumulative mass concentrations, and far outside PM10's range.
 */
void apc1_get_pm1_str(char *data)
{
    apc1_measurement *sensor = APC1_get_data();
    sprintf(data, "%d", sensor->pm1_0);
}
void apc1_get_pm25_str(char *data)
{
    apc1_measurement *sensor = APC1_get_data();
    sprintf(data, "%d", sensor->pm2_5);
}
void apc1_get_pm10_str(char *data)
{
    apc1_measurement *sensor = APC1_get_data();
    sprintf(data, "%d", sensor->pm10);
}
void sen5x_get_temp_str(char *data)
{
    sensirion_data *sensor = sensirion_get_data();
    sprintf(data, "%.1f", (float) sensor->sen5x.temperature / 100);
}
void sen5x_get_mois_str(char *data)
{
    sensirion_data *sensor = sensirion_get_data();
    sprintf(data, "%.1f", (float) sensor->sen5x.humidity / 100);
}
void sen5x_get_voc_str(char *data)
{
    sensirion_data *sensor = sensirion_get_data();
    sprintf(data, "%.1f", (float) sensor->sen5x.voc_index / 10);
}
void sen5x_get_nox_str(char *data)
{
    sensirion_data *sensor = sensirion_get_data();
    sprintf(data, "%.1f", (float) sensor->sen5x.nox_index / 10);
}
void sen5x_get_pm1_str(char *data)
{
    sensirion_data *sensor = sensirion_get_data();
    sprintf(data, "%.1f", (float) sensor->sen5x.mass_concentration_pm1p0 / 10);
}
void sen5x_get_pm25_str(char *data)
{
    sensirion_data *sensor = sensirion_get_data();
    sprintf(data, "%.1f", (float) sensor->sen5x.mass_concentration_pm2p5 / 10);
}
void sen5x_get_pm10_str(char *data)
{
    sensirion_data *sensor = sensirion_get_data();
    sprintf(data, "%.1f", (float) sensor->sen5x.mass_concentration_pm10p0 / 10);
}
void sen5x_get_pm4_str(char *data)
{
    sensirion_data *sensor = sensirion_get_data();
    sprintf(data, "%.1f", (float) sensor->sen5x.mass_concentration_pm4p0 / 10);
}
void scd4_get_co2_str(char *data)
{
    sensirion_data *sensor = sensirion_get_data();
    sprintf(data, "%d", sensor->scd4x.co2);
}
void scd4_get_temp_str(char *data)
{
    sensirion_data *sensor = sensirion_get_data();
    sprintf(data, "%d", (int16_t)sensor->scd4x.temperature);
}
void scd4_get_moist_str(char *data)
{
    sensirion_data *sensor = sensirion_get_data();
    sprintf(data, "%d", (int16_t)sensor->scd4x.temperature);
}
/*
 * BME280_get_temperature() and BME280_get_humidity() both return TENTHS of a
 * unit: the Bosch integer compensation path yields 0.01 degC and 0.001 %RH
 * (see the temperature_min/max = -4000/8500 and humidity_max = 100000 clamps in
 * bme280.c), and bme280_api.c divides those by 10 and 100 respectively. The
 * /10 here is therefore required to print degC and %RH - without it a reading
 * of 24.2 degC printed as "242.0". Every other consumer of these accessors
 * already does this (app.c's weather print, smart_sensor_same51/hal.c).
 * Temperature is signed, so it must not be read back through an unsigned type
 * or sub-zero readings wrap into large positives.
 */
void bme280_get_temp_str(char *data)
{
    int16_t sensor = BME280_get_temperature();
    sprintf(data,  "%.1f", (float)sensor / 10.0f);
}
void bme280_get_moist_str(char *data)
{
    uint16_t sensor = BME280_get_humidity();
    sprintf(data,  "%.1f", (float)sensor / 10.0f);
}
void bme280_get_press_str(char *data)
{
    uint16_t sensor = BME280_get_pressure();
    sprintf(data, "%d", sensor / 10);
}
void vcnl4200_get_dist_str(char *data)
{
    uint16_t dist, light;
    vcnl4200_get_values(&dist, &light);
    sprintf(data, "%d", dist);
}
void vcnl4200_get_light_str(char *data)
{
    uint16_t dist, light;
    vcnl4200_get_values(&dist, &light);
    sprintf(data, "%d", light);
}



/* *****************************************************************************
 End of File
 */

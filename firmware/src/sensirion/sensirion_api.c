/*******************************************************************************
  HVAC source file

  Company:
    Microchip Technology Inc.

  File Name:
    hvac.c

  Summary:

  Description:
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*
    (c) 2021 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/
//DOM-IGNORE-END

/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "sensirion_api.h"

#include "sensirion_i2c_hal.h"
#include "scd4x_i2c.h"
#include "sen5x_i2c.h"
#include "sensirion_common.h"

#include <string.h>                     // string lib functions
#include <stdio.h>                      // sprintf function

sensirion_data sensor_data;
bool initialized = false;
bool scd4_init = false;
bool sen5_init = false;
bool sensirion_debug = false;
char buffer[200];
char scd4_serial[13];

void sensirion_print_message(void)
{
    if(sensirion_debug)
    {
        SENSIRION_TERMINAL_Write((uint8_t *)buffer, strlen(buffer));
        while(!SENSIRION_TERMINAL_Done());
    }
}

bool sensirion_handle_error(int16_t error, const char* error_msg)
{
    if(error == NO_ERROR) return false;
    if(sensirion_debug)
    {
        sprintf(buffer, "%s: %i\n", error_msg, error);
        sensirion_print_message();
    }
    return true;
}

void sensirion_init(void)
{
    if(!initialized)
    {
        sensirion_scd4x_init();
        sensirion_sen5x_init();
    
        sensirion_scd4x_start_measurement();
        sensirion_sen5x_start_measurement();
        
        initialized = sensirion_scd4x_available() || sensirion_sen5x_available();
        SENSIRION_DelayMs(1000);      
        return;
    }
}

bool sensirion_available(void)
{
    return initialized;
}

bool sensirion_scd4x_available(void)
{
    return scd4_init;
}

bool sensirion_sen5x_available(void)
{
    return sen5_init;
}

sensirion_data* sensirion_read_data(void)
{
    sensirion_scd4x_read_data();
    sensirion_sen5x_read_data();
    return &sensor_data;
}

sensirion_data* sensirion_get_data(void)
{
    return &sensor_data;
}

void sensirion_set_debug(bool state)
{
    sensirion_debug = state;
}

void sensirion_print_data(void)
{
    // save state of debug flag. To print we need to set debug = true;
    bool debug_bckp = sensirion_debug;
    sensirion_debug = true;
    
    if(!sen5_init)
    {
        sprintf(buffer, "SEN5X not initialized!\n");   
        sensirion_print_message(); 
    }
    else
    {
        sprintf(buffer, "**** SEN5X sensor ****\n");   
        sensirion_print_message(); 
        sprintf(buffer, "SEN5X Mass concentration pm1p0: %.2f ug/m3 \n", 
                (float)sensor_data.sen5x.mass_concentration_pm1p0 / 10);   
        sensirion_print_message();    
        sprintf(buffer, "SEN5X Mass concentration pm2p5: %.2f ug/m3 \n", 
                (float)sensor_data.sen5x.mass_concentration_pm2p5 / 10);   
        sensirion_print_message();    
        sprintf(buffer, "SEN5X Mass concentration pm4p0: %.2f ug/m3 \n", 
                (float)sensor_data.sen5x.mass_concentration_pm4p0 / 10); 
        sensirion_print_message();    
        sprintf(buffer, "SEN5X Mass concentration pm10p0: %.2f ug/m3 \n", 
                (float)sensor_data.sen5x.mass_concentration_pm10p0 / 10);   
        sensirion_print_message();    

        if (sensor_data.sen5x.humidity == 0x7fff) 
            sprintf(buffer, "SEN5X Ambient humidity: n/a\n");
        else  
            sprintf(buffer, "SEN5X Ambient humidity: %.1f %%RH\n", 
                    (float)sensor_data.sen5x.humidity / 100);               
        sensirion_print_message();

        if (sensor_data.sen5x.temperature == 0x7fff) 
            sprintf(buffer, "SEN5X Ambient temperature: n/a\n");
        else 
            sprintf(buffer, "SEN5X Ambient temperature: %.1f C\n", 
                    (float)sensor_data.sen5x.temperature / 100);
        sensirion_print_message(); 

        if (sensor_data.sen5x.voc_index == 0x7fff) 
            sprintf(buffer, "SEN5X Voc index: n/a\n");
        else 
            sprintf(buffer, "SEN5X Voc index: %.1f \n", 
                    (float)sensor_data.sen5x.voc_index / 10);    
        sensirion_print_message();

        if (sensor_data.sen5x.nox_index == 0x7fff) 
            sprintf(buffer, "SEN5X Nox index: n/a\n");
        else 
            sprintf(buffer, "SEN5X Nox index: %.1f \n", 
                    (float)sensor_data.sen5x.nox_index / 10);
        sensirion_print_message();
    }
    
    if(!scd4_init)
    {
        sprintf(buffer, "SCD4X not initialized!\n");   
        sensirion_print_message(); 
    }
    if(sensor_data.scd4x.co2 == 0) 
    {
        sprintf(buffer, "Invalid sample detected, skipping...\n");
        sensirion_print_message();
    } 
    else 
    {
        sprintf(buffer, "**** SCD4X sensor ****\n"); 
        sensirion_print_message(); 
        sprintf(buffer, "SCD4X CO2: %d                     \n", 
                sensor_data.scd4x.co2);
        sensirion_print_message();
        sprintf(buffer, "SCD4X Temperature: %d mC          \n", 
                (int)sensor_data.scd4x.temperature);
        sensirion_print_message();
        sprintf(buffer, "SCD4X Humidity: %d mRH            \n", 
                (int)sensor_data.scd4x.humidity);
        sensirion_print_message();
    }    
    // get back debug flag state
    sensirion_debug = debug_bckp;
}

void sensirion_sen5x_init() {
    int16_t error = 0;
    int16_t sum_error = 0;

    sensirion_i2c_hal_init();

    error = sen5x_device_reset();
    sensirion_handle_error(error, "Error executing sen5x_device_reset");

    unsigned char serial_number[32];
    uint8_t serial_number_size = 32;
    error = sen5x_get_serial_number(serial_number, serial_number_size);
    if (sensirion_handle_error(error, "Error executing sen5x_get_serial_number")) 
    {
        sum_error += error;
    } 
    else 
    {
        sprintf(buffer, "SEN5X Serial number: %s\n", serial_number);
        sensirion_print_message();
    }
    
    unsigned char product_name[32];
    uint8_t product_name_size = 32;
    error = sen5x_get_product_name(product_name, product_name_size);
    if (sensirion_handle_error(error, "Error executing sen5x_get_product_name")) 
    {
        sum_error += error;
    } 
    else 
    {
        sprintf(buffer, "SEN5X Product name: %s\n", product_name);
        sensirion_print_message();
    }

    uint8_t firmware_major;
    uint8_t firmware_minor;
    bool firmware_debug;
    uint8_t hardware_major;
    uint8_t hardware_minor;
    uint8_t protocol_major;
    uint8_t protocol_minor;
    error = sen5x_get_version(&firmware_major, &firmware_minor, &firmware_debug,
                              &hardware_major, &hardware_minor, &protocol_major,
                              &protocol_minor);

    if (sensirion_handle_error(error, "Error executing sen5x_get_version"))
    {
        sum_error += error;
    } 
    else 
    {
        sprintf(buffer, "SEN5X Firmware: %u.%u, Hardware: %u.%u\n", firmware_major,
               firmware_minor, hardware_major, hardware_minor);
        sensirion_print_message();
    }
    
    if(sum_error == NO_ERROR) sen5_init = true;
}

void sensirion_sen5x_start_measurement(){
    // set a temperature offset - supported by SEN54 and SEN55 sensors
    //
    // By default, the temperature and humidity outputs from the sensor
    // are compensated for the modules self-heating. If the module is
    // designed into a device, the temperature compensation might need
    // to be adapted to incorporate the change in thermal coupling and
    // self-heating of other device components.
    //
    // A guide to achieve optimal performance, including references
    // to mechanical design-in examples can be found in the app note
    // ?SEN5x ? Temperature Compensation Instruction? at www.sensirion.com.
    // Please refer to those application notes for further information
    // on the advanced compensation settings used in
    // `sen5x_set_temperature_offset_parameters`,
    // `sen5x_set_warm_start_parameter` and
    // `sen5x_set_rht_acceleration_mode`.
    //
    // Adjust temp_offset in degrees celsius to account for additional
    // temperature offsets exceeding the SEN module's self heating.
    int16_t error = 0;
    float temp_offset = 0.0f;
    int16_t default_slope = 0;
    uint16_t default_time_constant = 0;
    
    error = sen5x_set_temperature_offset_parameters(
        (int16_t)(200 * temp_offset), default_slope, default_time_constant);
    if (!sensirion_handle_error(error, "Error executing sen5x_set_temperature_offset_parameters")) 
    {
        sprintf(buffer, "Temperature Offset set to %.2f C (SEN54/SEN55 only)\n", temp_offset);
        sensirion_print_message();
    }

    // Start Measurement
    error = sen5x_start_measurement();
    sensirion_handle_error(error, "Error executing sen5x_start_measurement"); 
    // wait min 1 sec
}

void sensirion_sen5x_stop_measurement(void) {
    int16_t error = 0;
    error = sen5x_stop_measurement();
    sensirion_handle_error(error, "Error executing sen5x_stop_measurement"); 
}

sensirion_sen5x_data* sensirion_sen5x_read_data(void){
    int16_t error = 0;
    // if sensor is not initialized, do not read the the sensor
    if(!sen5_init) return &sensor_data.sen5x;
    
    // Read Measurement
    error = sen5x_read_measured_values(
            &sensor_data.sen5x.mass_concentration_pm1p0, 
            &sensor_data.sen5x.mass_concentration_pm2p5,
            &sensor_data.sen5x.mass_concentration_pm4p0, 
            &sensor_data.sen5x.mass_concentration_pm10p0,
            &sensor_data.sen5x.humidity, 
            &sensor_data.sen5x.temperature, 
            &sensor_data.sen5x.voc_index, 
            &sensor_data.sen5x.nox_index);
    
    sensor_data.sen5x.temperature = sensor_data.sen5x.temperature / 2;
    sensirion_handle_error(error, "Error executing sen5x_read_measured_values");
    return &sensor_data.sen5x;
}

sensirion_sen5x_data* sensirion_sen5x_get_data(void){
    return &sensor_data.sen5x;
}

void sensirion_scd4x_init(void){
    int16_t error = 0;

    sensirion_i2c_hal_init();

    // Clean up potential SCD40 states
    scd4x_wake_up();
    scd4x_stop_periodic_measurement();
    scd4x_reinit();

    uint16_t serial_0;
    uint16_t serial_1;
    uint16_t serial_2;
    error = scd4x_get_serial_number(&serial_0, &serial_1, &serial_2);
    if (!sensirion_handle_error(error, "Error executing scd4x_get_serial_number")) 
    {
        sprintf(scd4_serial, "%04x%04x%04x", serial_0, serial_1, serial_2);
        sprintf(buffer, "SCD4X Serial: 0x%s\n", scd4_serial);
        sensirion_print_message();
        scd4_init = true;
    }
}

void sensirion_scd4x_start_measurement(void){
    int16_t error = 0;
    // Start Measurement
    error = scd4x_start_periodic_measurement();
    sensirion_handle_error(
            error, 
            "Error executing scd4x_start_periodic_measurement");
    // wait min 5 sec
}

sensirion_scd4x_data* sensirion_scd4x_read_data(void){
    int16_t error = 0;
    
    // if sensor is not initialized, return
    if(!scd4_init) return &sensor_data.scd4x;
    
    // Read Measurement
    bool data_ready_flag = false;    
    error = scd4x_get_data_ready_flag(&data_ready_flag);
    if (sensirion_handle_error(
            error, 
            "Error executing scd4x_get_data_ready_flag"
        )) 
    {
        return &sensor_data.scd4x;
    }
    if (!data_ready_flag) {
        return &sensor_data.scd4x;
    }
    error = scd4x_read_measurement(
            &sensor_data.scd4x.co2, 
            &sensor_data.scd4x.temperature, 
            &sensor_data.scd4x.humidity);
    sensirion_handle_error(error, "Error executing scd4x_read_measurement");
    return &sensor_data.scd4x;
}

sensirion_scd4x_data* sensirion_scd4x_get_data(void){
    return &sensor_data.scd4x;
}


/* *****************************************************************************
 End of File
 */


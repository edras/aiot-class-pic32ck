/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology Inc.

  @File Name
    hw_layer.c

  @Summary
    Implementation of hardware layer functions.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "hal.h"
#include "app.h"
#include "terminal.h"

/* This section lists the other files that are included in this file.
 */

void HardwareLayer_init(void)
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
       
    // Interrupt function for SW0 button
    EIC_CallbackRegister(EIC_PIN_15, handle_button, 0);
    
    SYSTICK_TimerStart();    

}

/*
 * functions that should be available across boards but
 * with different implementations according to the board.
 */
void print_oled_data_hal(void)
{
    char msg[50];
    if(BME280_available())
    {
        sprintf(msg, "Temp: %.1f C    ", (float)BME280_get_temperature()/10);
        oledc_draw_string_on_bg(2, 20, 1, 2, msg, GREEN, BLACK);
        sprintf(msg, "Mois: %.1f %%RH ", (float)BME280_get_humidity()/10);
        oledc_draw_string_on_bg(2, 40, 1, 2, msg, GREEN, BLACK);
        sprintf(msg, "Press:%.0f hPA  ", (float)BME280_get_pressure()/10);
        oledc_draw_string_on_bg(2, 60, 1, 2, msg, GREEN, BLACK);
    }
    else if(APC1_available())
    {
        apc1_measurement *apc1_data = APC1_get_data();
        sprintf(msg, "Temp: %.1f C    ", apc1_data->t_comp);
        oledc_draw_string_on_bg(2, 20, 1, 2, msg, GREEN, BLACK);
        sprintf(msg, "Mois: %.1f %%RH ", apc1_data->rh_comp);
        oledc_draw_string_on_bg(2, 40, 1, 2, msg, GREEN, BLACK);
        sprintf(msg, "CO2:  %d ppm     ", apc1_data->eco2);
        oledc_draw_string_on_bg(2, 60, 1, 2, msg, GREEN, BLACK);
    }
    else if(sensirion_available())
    {
        sensirion_data *sensor = sensirion_get_data();
        sprintf(msg, "Temp: %.1f C    ", (float)sensor->sen5x.temperature/100);
        oledc_draw_string_on_bg(2, 20, 1, 2, msg, GREEN, BLACK);
        sprintf(msg, "Mois: %.1f %%RH ", (float)sensor->sen5x.humidity/100);
        oledc_draw_string_on_bg(2, 40, 1, 2, msg, GREEN, BLACK);
        sprintf(msg, "CO2:  %d ppm     ", sensor->scd4x.co2);
        oledc_draw_string_on_bg(2, 60, 1, 2, msg, GREEN, BLACK);
    }
}

uint32_t get_mcu_uuid(void)
{
//    #define DSU_BASE_ADDRESS 0x41002000
    #define DSU_DID_OFFSET   0x18

    // Access the DSU_DID register directly using the base address and offset
    return *((volatile uint32_t *)(DSU_BASE_ADDRESS + DSU_DID_OFFSET));    
}


/* *****************************************************************************
 End of File
 */

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

void print_oled_data_hal(void)
{
    char msg[20];
    TopicItem *item = NULL;
    uint8_t count = 0;
    uint8_t size = 0;
    while((item = MQTT_get_next_topic(item)) != NULL)
    {
        // do not print switch and led topics, only the modules available on mikrobus
        if(strcmp(item->nodename, "curiosity") == 0) continue;
        if(item->get_str_value && count < 2)
        {
            item->get_str_value(msg);
            size = strlen(msg);
            for(uint8_t i = size; i < 20; i++) msg[i] = ' ';
            msg[19] = '\0';

            oledc_draw_string_on_bg(2, count*40 + 20, 1, 2, item->topic_name, GREEN, BLACK);
            oledc_draw_string_on_bg(2, count*40 + 40, 1, 2, msg, GREEN, BLACK);
            count++;
        }
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

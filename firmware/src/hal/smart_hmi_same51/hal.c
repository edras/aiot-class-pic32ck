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
    oledc_draw_string_on_bg(2, 40, 1, 2, "AIoT Thermostat", BLUE, BLACK);
    oledc_draw_string_on_bg(2, 60, 1, 2, " BLE UART BM70 ", BLUE, BLACK);
    oledc_draw_string_on_bg(2, 80, 1, 2, "               ", BLACK, BLACK);  
}

uint32_t get_mcu_uuid(void)
{
    //#define DSU_BASE_ADDRESS 0x41002000
    #define DSU_DID_OFFSET   0x18

    // Access the DSU_DID register directly using the base address and offset
    return *((volatile uint32_t *)(DSU_BASE_ADDRESS + DSU_DID_OFFSET));    
}


/* *****************************************************************************
 End of File
 */

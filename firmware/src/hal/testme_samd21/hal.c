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
    EIC_CallbackRegister(EIC_PIN_0, handle_button, 0);
    EIC_CallbackRegister(EIC_PIN_1, handle_button, 0);
    
    SYSTICK_TimerStart();    

}

/*
 * functions that should be available across boards but
 * with different implementations according to the board.
 */
void print_oled_data_hal(void)
{
  
}

uint32_t get_mcu_uuid(void)
{
    return 0;
}

/* *****************************************************************************
 End of File
 */

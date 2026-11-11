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
#include "rnbd_uart_hal.h"

/* This section lists the other files that are included in this file.
 */

void HardwareLayer_init(void)
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    SYSTICK_TimerStart();

    /* Arm reception on the click UART before the first RNBD command is sent, so
       the module's response cannot be missed. smart_sensor_same51 gets this for
       free from its ring buffer USART plib; SERCOM1 here is the plain plib,
       which only receives while a request is outstanding. */
    RNBD_uart_hal_init();
}

/* smart_sensor_same51 drives handle_button() from EIC_PIN_15, configured
   SENSE_BOTH (plib_eic.c: EIC_CONFIG[1] SENSE7_BOTH). PD02 has no EXTINT
   function on this device - its function A is NMI - so the button is polled from
   the superloop instead. The poll must therefore report BOTH edges:
   handle_button() re-reads the pin to derive button_state, and app.c's
   check_button_longPress_reset() resets the board once button_state has stayed
   false for BUTTON_LONGPRESS_RESET ticks. Reporting only the press would leave
   button_state false forever and reset the device a few seconds after any
   press. */
static bool button_last_state = true; /* released */

void HAL_PollButton(void)
{
    bool button_now = BUTTON_Get();
    if (button_now != button_last_state)
    {
        handle_button(0);
        button_last_state = button_now;
    }
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
    return DSU_REGS->DSU_DID;
}


/* *****************************************************************************
 End of File
 */

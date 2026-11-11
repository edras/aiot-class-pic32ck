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
       the module's response cannot be missed. demo_same51 gets this for free
       from its ring buffer USART plib; SERCOM1 here is the plain plib, which
       only receives while a request is outstanding. */
    RNBD_uart_hal_init();
}

/* demo_same51 drives handle_button() from EIC_PIN_15, configured SENSE_BOTH
   (plib_eic.c: EIC_CONFIG[1] SENSE7_BOTH). PD02 has no EXTINT function on this
   device - its function A is NMI - so the button is polled from the superloop
   instead. The poll must therefore report BOTH edges: handle_button() re-reads
   the pin to derive button_state, and app.c's check_button_longPress_reset()
   resets the board once button_state has stayed false for BUTTON_LONGPRESS_RESET
   ticks. Reporting only the press would leave button_state false forever and
   reset the device a few seconds after any press. */
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
    return DSU_REGS->DSU_DID;
}


/* *****************************************************************************
 End of File
 */

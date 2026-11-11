/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

/*******************************************************************************
 * Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
 * ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "tasks.h"                      // Application tasks header
#include "app.h"

/*
 * Terminal UART configuration: 115200/8/N/1
 */

void Millisecond_Callback(uintptr_t context) {
    /* This function should be called each 1 millisecond */
    Task_synch();
    /* Reset if button is pressed more than BUTTON_LONGPRESS_RESET ms*/
    check_button_longPress_reset();
}

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main(void)
{
    HardwareLayer_init();

    /*
     * This function should be called inside hal (Hardware Abstraction Layer) module.
     * It is placed here for clarity of Task architecture
     */
    TIMER_SetCallback(Millisecond_Callback);

    print_header(true);
    init_modules();
    init_mqtt_topics();
    print_header(false);

    Task_register(0, 2000, read_sensor_data);
    Task_register(0, 1000, publish_sensor_data);
    Task_register(0, 1000, print_oled_data);
    Task_register(0, 1000, publish_csv_data);
    Task_register(0, 1000, blink_online_led);
    Task_register(0, 300,  fast_sensor_read);

    while (true)
    {
        switch(get_app_state())
        {
            case STATE_APP_RXTX:
                Task_execute();
                check_button();
#ifdef HAL_HAS_POLL_BUTTON
                HAL_PollButton();
#endif
                capsense2_check_state();
                check_rnbd_data();
                break;

            case STATE_APP_TERMINAL:
            default: break;
        }
        T1S_execute();
        handle_USART_cmd();
    }

    /* Execution should not come here during normal operation */
    return ( EXIT_FAILURE);
}

/*******************************************************************************
 End of File
 */


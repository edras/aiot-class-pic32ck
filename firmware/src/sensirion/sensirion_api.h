/*******************************************************************************
  Weather click header file

  Company:
    Microchip Technology Inc.

  File Name:
    hvac.h

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

#ifndef _SENSIRION_EXAMPLE_H    /* Guard against multiple inclusion */
#define _SENSIRION_EXAMPLE_H

/**
  Section: Included Files
 */

#include "hal.h"
#include <stdbool.h>

/**
  Section: Sensirion Click Driver APIs
 */

typedef struct {
    uint16_t mass_concentration_pm1p0;
    uint16_t mass_concentration_pm2p5;
    uint16_t mass_concentration_pm4p0;
    uint16_t mass_concentration_pm10p0;
    int16_t  humidity;
    int16_t  temperature;
    int16_t  voc_index;
    int16_t  nox_index;
} sensirion_sen5x_data;

typedef struct {
    uint16_t co2;
    int32_t  temperature;
    int32_t  humidity;    
} sensirion_scd4x_data;

typedef struct {
    sensirion_scd4x_data scd4x;
    sensirion_sen5x_data sen5x;
} sensirion_data;

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

    void sensirion_init(void);
    bool sensirion_available(void);
    void sensirion_set_debug(bool state);
    sensirion_data* sensirion_read_data(void);
    sensirion_data* sensirion_get_data(void);
    void sensirion_print_data(void);  
    
    void sensirion_scd4x_init(void);
    bool sensirion_scd4x_available(void);
    void sensirion_scd4x_start_measurement(void);
    sensirion_scd4x_data* sensirion_scd4x_read_data(void);
    sensirion_scd4x_data* sensirion_scd4x_get_data(void);

    void sensirion_sen5x_init(void);
    bool sensirion_sen5x_available(void);
    void sensirion_sen5x_start_measurement(void);
    sensirion_sen5x_data* sensirion_sen5x_read_data(void);
    sensirion_sen5x_data* sensirion_sen5x_get_data(void);
    
    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _SENSIRION_EXAMPLE_H */

/* *****************************************************************************
 End of File
 */

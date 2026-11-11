/*******************************************************************************
* Copyright (C) 2023-2024 Microchip Technology Inc. and its subsidiaries.
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

#ifndef RE46C42X_H_
#define RE46C42X_H_

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

#include "hal.h"

#define RE46C42X_REG0_ABUF_CONTROL 0x00
#define RE46C42X_REG1_BOOST_CONTROL 0x02
#define RE46C42X_REG2_PHOTO_AMP_CONTROL 0x04
#define RE46C42X_REG3_IO_HORN_CO_CONTROL 0x06
#define RE46C42X_REG4_REG_CO_CONTROL 0x08
#define RE46C42X_REG5_BOOST_IRCAP_SPI_WD_SETUP  0x0A
#define RE46C42X_REG6_PHOTO_AMP_SETUP 0x0C
#define RE46C42X_REG7_LED_SETUP 0x0E
#define RE46C42X_REG8_IO_SETUP 0x10
#define RE46C42X_REG9_PHOTO_LED_TC_SELECTION 0x12

#define PA1 1
#define PA2 2
#define PA1_SMOKE_THRESHOLD 1000
#define PA2_SMOKE_THRESHOLD 1000
#define RE46C42X_REG6_CONF_100US_GAIN_1 0x80
#define RE46C42X_REG6_CONF_100US_GAIN_2 0x81
#define RE46C42X_REG6_CONF_100US_GAIN_4 0x82
#define RE46C42X_REG6_CONF_100US_GAIN_8 0x83
#define RE46C42X_REG6_CONF_100US_GAIN_16 0x84
#define RE46C42X_REG6_CONF_100US_GAIN_32 0x85

#define RE46C42X_REG6_CONF_200US_GAIN_1 0x90
#define RE46C42X_REG6_CONF_200US_GAIN_2 0x91
#define RE46C42X_REG6_CONF_200US_GAIN_4 0x92
#define RE46C42X_REG6_CONF_200US_GAIN_8 0x93
#define RE46C42X_REG6_CONF_200US_GAIN_16 0x94
#define RE46C42X_REG6_CONF_200US_GAIN_32 0x95

#define RE46C42X_REG6_CONF_300US_GAIN_1 0xA0
#define RE46C42X_REG6_CONF_300US_GAIN_2 0xA1
#define RE46C42X_REG6_CONF_300US_GAIN_4 0xA2
#define RE46C42X_REG6_CONF_300US_GAIN_8 0xA3
#define RE46C42X_REG6_CONF_300US_GAIN_16 0xA4
#define RE46C42X_REG6_CONF_300US_GAIN_32 0xA5

#define RE46C42X_REG6_CONF_400US_GAIN_1 0xB0
#define RE46C42X_REG6_CONF_400US_GAIN_2 0xB1
#define RE46C42X_REG6_CONF_400US_GAIN_4 0xB2
#define RE46C42X_REG6_CONF_400US_GAIN_8 0xB3
#define RE46C42X_REG6_CONF_400US_GAIN_16 0xB4
#define RE46C42X_REG6_CONF_400US_GAIN_32 0xB5
    
void RE46C42X_rled_on();
void RE46C42X_rled_off();
void RE46C42X_horn_on();
void RE46C42X_horn_off();
bool RE46C42X_available(void);

/*!
 * @brief This API will write data to a specific register.
 *
 * @param[in] reg : The register that should be read
 * 
 * @return The value from requested register
 */
uint8_t RE46C42X_read_reg(uint8_t reg);

/*!
 * @brief This API will write data to a specific register.
 *
 * @param[in] reg : The register that will be written.
 * @param[in] data : The data that will be written at given register.
 */
void RE46C42X_write_reg(uint8_t reg, uint8_t data);

/*!
 * @brief This API will initialize the module.
 * WARNING: This function should not be called while smoke is present.
 * 
 * @return True if module initialized correctly, false otherwise.
 */
bool RE46C42X_init(void);

/*!
 * @brief This API will configure PA1.
 * The register value and integration result will be saved in internal 
 * variables for further comparison.
 * WARNING: This function should not be called while smoke is present.
 *
 * @param[in] reg6_pa1_conf : The register value for PA1 integration
 */
void RE46C42X_set_pa1_conf(uint8_t reg6_pa1_conf);

/*!
 * @brief This API will configure PA2.
 * The register value and integration result will be saved in internal 
 * variables for further comparison.
 * WARNING: This function should not be called while smoke is present.
 *
 * @param[in] reg6_pa2_conf : The register value for PA2 integration
 */
void RE46C42X_set_pa2_conf(uint8_t reg6_pa2_conf);

/*!
 * @brief This API sets LED1 current value
 *
 * @param[in] current : The current value for LED1 (20, 40, 50, 60, 80, 100, 150, 200)
 */
void RE46C42X_set_led1_current(uint32_t current);

/*!
 * @brief This API sets LED2 current value
 *
 * @param[in] current : The current value for LED2 (20, 40, 50, 60, 80, 100, 150, 200)
 */
void RE46C42X_set_led2_current(uint32_t current);

/*!
 * @brief This API checks if smoke is detected.
 *
 * @return True if smoke is detected, false otherwise
 */
bool RE46C42X_is_smoke_detected();

/*!
 * @brief This API turns debug message on/off.
 *
 * @param[in] state : The state of debug
 */
void RE46C42X_set_debug(bool state);

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* RE46C42X_H_ */
/** @}*/

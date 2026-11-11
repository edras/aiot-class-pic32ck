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

#ifndef MULTIMETER_H
#define	MULTIMETER_H

#include "hal.h"

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */   

/**
  * @brief Initialize the multimeter module.
  *
  * This function initializes the MCP3204 ADC and configures the GPIO pins
  * for range selection. Should be called before any measurement.
  *
  * @return true if initialization successful, false otherwise.
  */
bool MULTIMETER_init(void);

/**
  * @brief Check if the multimeter hardware is available.
  *
  * This function checks if the MCP3204 ADC responds on the SPI bus.
  *
  * @return true if multimeter hardware is detected, false otherwise.
  */
bool MULTIMETER_available(void);

/**
  *  @brief This API is the device calibration.
  */
void MULTIMETER_calibrate(void);

/**
  * \brief Returns the value of measured current.
  *
  * \example 
  *   sprintf(msg, "\r\nmeasured current is %.2f mA\r\n", MULTIMETER_measure_I());
  *   print_message(msg);
  * \return Current value in mA
  */
float MULTIMETER_measure_I(void);

/**
  * \brief Returns the value of measured voltage.
  *
  * \example
  *   sprintf(msg, "\r\nmeasured voltage is %.2f mV\r\n", MULTIMETER_measure_U());
  *   print_message(msg);
  * \return Voltage value in mV
  */
float MULTIMETER_measure_U(void);

/**
  * \brief Returns the value of measured resistance.
  *
  * \example
  *   sprintf(msg, "\r\nmeasured resistance is %.2f ohms\r\n", MULTIMETER_measure_R());
  *   print_message(msg);
  * \return Resistance value in Ohms
  */
float MULTIMETER_measure_R(void);

/**
  * \brief Returns the value of measured capacitance.
  *
  * \example
  *   sprintf(msg, "\r\nmeasured capacitance is %.2f nF\r\n", MULTIMETER_measure_C());
  *   print_message(msg);
  * \return Capacitance value in nF
  */
float MULTIMETER_measure_C(void);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* MULTIMETER_H */
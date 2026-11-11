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

#ifndef RELAY2_H_
#define RELAY2_H_

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

#include "hal.h"
    
/* ----- PUBLIC API Functions -----*/
/**
 * \ingroup RELAY2_API
 * \brief API to configure Relay 1 state
 * 
 * \param state - boolean variable to set state to ON or OFF
 * \return Nothing
 */
void RELAY2_relay1_control(bool state);
void RELAY2_relay1_on(void);
void RELAY2_relay1_off(void);

/**
 * \ingroup RELAY2_API
 * \brief API to configure Relay 2 state
 * 
 * \param state - boolean variable to set state to ON or OFF
 * \return Nothing
 */
void RELAY2_relay2_control(bool state);
void RELAY2_relay2_on(void);
void RELAY2_relay2_off(void);

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* RELAY2_H_ */
/** @}*/

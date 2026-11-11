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
#if 0 /*GUARD AGAINST MPLAB X IDE INCLUSION */

#ifndef MODULEXYZ_H_
#define MODULEXYZ_H_

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

#include "hal.h"

/* ----- PUBLIC API Return Codes -----*/
// MODULEXYZ_OK should be the last to have value 0, errors should be negative
typedef enum  {
    MODULEXYZ_E_NOK = -5,
    MODULEXYZ_E_NULL_PTR,
    MODULEXYZ_E_DEV_NOT_FOUND,
    MODULEXYZ_E_COMM_FAIL,
    MODULEXYZ_E_INVALID,
    MODULEXYZ_OK
} MODULEXYZ_RC;


/* ----- PUBLIC API Functions -----*/
/**
 * \brief API description
 * 
 * \param variable_name - description
 * \return description
 */
 /*!
 * @brief API description
 *
 * @param[in] variable_name - description
 * @param[out] variable_name - description
 *
 * @return Result of API execution status
 */
bool MODULEXYZ_api_name(bool state, uint8_t *variable);

/* Example of APIs
 * - MODULEXYZ_init();
 * - MODULEXYZ_set_reg();
 * - MODULEXYZ_get_reg();
 */

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* MODULEXYZ_H_ */
/** @}*/

#endif
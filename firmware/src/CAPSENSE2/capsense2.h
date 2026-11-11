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

#ifndef CAPSENSE2_H_
#define CAPSENSE2_H_

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

#include "hal.h"
    
/**
 * @brief CapSense 2 Register List.
 * @details Specified Register List of CapSense 2 Click driver.
 */
#define CAPSENSE2_REG_MAIN_STATUS_CONTROL               0x00
#define CAPSENSE2_MAIN_STATUS_CONTROL_INT               0x01
#define CAPSENSE2_REG_BUTTON_STATUS_1                   0x03
#define CAPSENSE2_REG_BUTTON_STATUS_2                   0x04
#define CAPSENSE2_REG_SLIDER_POSITION_DATA              0x06
    
/**
 * @brief CapSense 2 device address setting.
 * @details Specified setting for device slave address selection of
 * CapSense 2 Click driver.
 */
#define CAPSENSE2_DEV_ADDR       0x28
#define CAPSENSE2_VENDOR_ID_ADDR 0x08
#define CAPSENSE2_VENDOR_ID      0x5D

/**
 * @brief CapSense 2 button status 1 register values.
 * @details Specified main status control register values of CapSense 2 Click driver.
 */
#define CAPSENSE2_BUTTON_STATUS_1_UP_SLIDER             0x80
#define CAPSENSE2_BUTTON_STATUS_1_DOWN_SLIDER           0x40
#define CAPSENSE2_BUTTON_STATUS_1_BUTTON_2              0x02
#define CAPSENSE2_BUTTON_STATUS_1_BUTTON_1              0x01

/**\name API success code */
#define CAPSENSE2_OK 0
/**\name API error codes */
#define CAPSENSE2_E_NULL_PTR -1
#define CAPSENSE2_E_DEV_NOT_FOUND -2
#define CAPSENSE2_E_COMM_FAIL -3
    
typedef enum {
    CAPSENSE2_NONE = 0,
    CAPSENSE2_PRESS,
    CAPSENSE2_RELEASE,
    CAPSENSE2_SLIDER_UP,
    CAPSENSE2_SLIDER_DOWN,
} CAPSENSE2_ACTIONS;

struct capsense2_dev {
	/*! Vendor Id */
	uint8_t vendor_id;
	/*! Device Id */
	uint8_t dev_id;
	/*! Read function pointer */
};

/*!
 *  @brief This API is the entry point.
 *  It reads the chip-id and calibration data from the sensor.
 *
 *  @param[in,out] dev : Structure instance of bme280_dev
 *
 *  @return Result of API execution status
 *  @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
int8_t CAPSENSE2_init(void);

/*!
 * @brief This API reads the data from the given register address of the sensor.
 *
 * @param[in] reg_addr : Register address from where the data to be read
 * @param[out] reg_data : Pointer to data buffer to store the read data.
 * @param[in] len : No of bytes of data to be read.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
int8_t capsense2_get_reg(uint8_t reg_addr, uint8_t *reg_data, uint16_t len);

/*!
 * @brief This API checks if the alert pin status.
 *
 * @return State of alert pin
 * @retval zero -> Not set / one -> Set
 */
int8_t capsense2_check_alert_pin();

/*!
 * @brief This API clears the interrupt pin of the sensor.
 * 
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
int8_t capsense2_clear_interrupt();

/*!
 * @brief This API reads the button 1 status.
 *
 * @param[out] button_status : Pointer to data buffer to store the read button status.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
int8_t capsense2_read_reg_btn_status_1(uint8_t *button_status);

void CAPSENSE2_execute(void);
bool CAPSENSE2_action_required(void);
bool CAPSENSE2_available(void);
CAPSENSE2_ACTIONS CAPSENSE2_get_s1_action(void);
CAPSENSE2_ACTIONS CAPSENSE2_get_s2_action(void);
CAPSENSE2_ACTIONS CAPSENSE2_get_slider_action(void);
uint8_t CAPSENSE2_get_slider_value(void);

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* CAPSENSE2_H_ */
/** @}*/

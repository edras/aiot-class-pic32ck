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

#ifndef RNBD451_H_
#define RNBD451_H_

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

#include "hal.h"
#include <string.h>
#include <stdbool.h>
    
/**
 * \ingroup RNBD
 * \brief This macro defines the time needed to place RNBD device in reset.
 */
#define RNBD_RESET_DELAY_TIME         (1)

/**
 * \ingroup RNBD
 * \brief This macro defines the RNBD boot time.
 */
#define RNBD_STARTUP_DELAY            (200)

/**\name API success code */
#define RNBD_OK     (0)
/**\name API error codes */
#define RNBD_E_DEV_NOT_FOUND (1)

/**
 * \ingroup RNBD_INTERFACE
 * Enum of the RNBD System Configuration Modes
 */
typedef enum
{
    TEST_MODE           = 0x00,
    APPLICATION_MODE    = 0x01 
}RNBD_SYSTEM_MODES_t;

typedef enum
{
    SF_1 = 1,   //Reset to factory default
    //SF_2 = 2,   //Reset to factory default including private services and script
}RNBD_FACTORY_RESET_MODE_t;

/* ----- PUBLIC API Functions -----*/
/**
 * \ingroup RNBD_API
 * \brief Write API transmitting to RNBD module
 * 
 * This API is used to send data bytes to the RNBD module
 *
 * HINT: This API is in place to give compile time memory allocation.
 *       Functionality exist locally within file.
 *       Use of IN LINE to prevent additional stack depth requirement. 
 *       APIs can be injected in place if suitable to save (1) stack depth level
 * 
 * \param txData - data byte to send
 * \return Nothing
 */
size_t RNBD_write(void *txData);

/**
 * \ingroup RNBD_API
 * \brief Returns if Write to RNBD module was completed
 * 
 * This API is used to receive status of communication with RNBD module
 *
 * HINT: This API is in place to give compile time memory allocation.
 *       Functionality exist locally within file.
 *       Use of IN LINE to prevent additional stack depth requirement. 
 *       APIs can be injected in place if suitable to save (1) stack depth level
 * 
 * \param N/A
 * \return bool status - RNBD is ready for another data byte
 */
bool RNBD_is_tx_done(void);

/**
 * \ingroup RNBD_API
 * \brief Returns if Read from RNBD module is ready
 * 
 * This API is used to receive status of communication with RNBD module
 *
 * HINT: This API is in place to give compile time memory allocation.
 *       Functionality exist locally within file.
 *       Use of IN LINE to prevent additional stack depth requirement. 
 *       APIs can be injected in place if suitable to save (1) stack depth level
 * 
 * \param N/A
 * \return bool status - RNBD is ready to provide another byte
 */
bool RNBD_is_rx_ready(void);

/**
 * \ingroup RNBD_API
 * \brief Initiate Hardware Reset of RNBD
 * 
 * This API is used to issue a hardware Reset to the RNBD.
 *
 * 
 * \param value true - Reset State false - Normal State
 * \return Nothing
 */
void RNBD_reset(bool value);

 /**
  * \ingroup RNBD_API
  * \brief Initializes RNBD Device
  * 
  * This API initializes RNBD Device.
  * 
  * \return Initialization Status.
  * \retval true - Success
  * \retval false - Failure.
  */
bool RNBD_init(void);

 /**
  * \ingroup RNBD_API
  * \brief Sends out command to RNBD.
  *
  * This API takes command and its length as input and sends that command 
  * to RNBD.
  * 
  * \param cmd RNBD command
  * \param cmdLen RNBD command length
  * \return Nothing
  */
void RNBD_send_cmd(const char *cmd, uint8_t cmdLen);

/**
 * \ingroup RNBD_API
 * \brief Gets config value from RNBD by sending get command
 * 
 * This API gets the config value from RNBD by sending get command.
 * For more details, refer GET COMMANDS in RNBD user guide.
 * 
 * \param getCmd Get command to send
 * \param getCmdLen Get command length
 * \param getCmdResp Buffer to store get command response
 * 
 * \return Length of get command response.
 * \retval index - tracked command response length.
 */
uint8_t RNBD_get_cmd(const char *getCmd, uint8_t getCmdLen, char *getCmdResp);

 /**
  * \ingroup RNBD_API
  * \brief Reads specific message from RNBD.
  * 
  * This API takes input from application on the expected response/status 
  * message. It starts read RNBD host interface immediately and look for 
  * expected message.
  * 
  * \param expectedMsg Expected response/status message from RNBD
  * \param msgLen Expected response/status message length.
  * \return Message Match Status
  * \retval true - Expected Message Received
  * \retval false - Otherwise
  */
bool RNBD_read_msg(const char *expectedMsg, uint8_t msgLen);

 /**
  * \ingroup RNBD_API
  * \brief Reads default response from RNBD.
  * 
  * This API reads RNBD interface for default response which is AOK or ERR.
  * 
  * \return Response Status
  * \retval true - Default Response Received
  * \retval false - Otherwise
  */
bool RNBD_read_default_response(void);

 /**
  * \ingroup RNBD_API
  * \brief Puts the RNBD in command mode.
  * 
  * This API puts the RNBD in command mode. 
  * 
  * \return Command Mode Status
  * \retval true - Success
  * \retval false - Failure
  */
bool RNBD_enter_cmd_mode(void);

 /**
  * \ingroup RNBD_API
  * \brief Puts the RNBD in data mode.
  * 
  * This API puts the RNBD in data mode.
  * 
  * \return Data Mode Status
  * \retval true - Success
  * \retval false - Failure
  */
bool RNBD_enter_data_mode(void);

 /**
  * \ingroup RNBD_API
  * \brief Changes module name.
  * 
  * This API changes the RNBD module name.
  * 
  * \return Reboot Status
  * \retval true - Success
  * \retval false - Failure
  */
bool RNBD_set_name(const char *name, uint8_t nameLen);

 /**
  * \ingroup RNBD_API
  * \brief Resets RNBD.
  * 
  * This API resets RNBD. For more details, refer R command in 
  * RNBD user guide.
  * 
  * \return Reboot Status
  * \retval true - Success
  * \retval false - Failure
  */
bool RNBD_reboot_cmd(void);

 /**
  * \ingroup RNBD_API
  * \brief Restores Factory Settings of RNBD module.
  * 
  * This API will issue a Factory Reset on the Module.
  * Restores all original module settings
  * For more details, refer R command in RNBD user guide.
  * 
  * \return Reboot Status
  * \retval true - Success
  * \retval false - Failure
  */
bool RNBD_factory_reset(RNBD_FACTORY_RESET_MODE_t resetMode);

 /**
  * \ingroup RNBD_API
  * \brief Disconnects the BLE link between RNBD and remote device.
  * 
  * This API disconnects the BLE link between RNBD and remote device. 
  * For more details, refer K command in RN_BLE user guide.
  * \return Disconnect Status
  * \retval true - Success
  * \retval false - Failure
  */
bool RNBD_disconnect(void);

 /**
  * \ingroup RNBD_API
  * \brief Sets up the Buffer and Buffer Size for Aysnc Message Handler
  * 
  * This API sets up the Buffer and the Buffer size to be used by the Async
  * Message Handler.
  * \param pBuffer Passed buffer
  * \param len Size of passed buffer 
  * \return Set Status
  * \retval true - Success
  * \retval false - Failure
  */
bool RNBD_set_async_message_handler(char* pBuffer, uint8_t len);

 /**
  * \ingroup RNBD_API
  * \brief Checks to see if there is Data Ready using Async Message Handling
  * 
  * This API checks to see if there is Data ready on the line using Async Message Handling.
  * This API follows the standard UART form.
  * \return Data Ready Status
  * \retval true - Data is ready
  * \retval false - Data is not ready
  */
bool RNBD_data_ready(void);

 /**
  * \ingroup RNBD_API
  * \brief Read incoming Data using Async Message Handling
  * 
  * This API reads incoming data using Async Message Handling.
  * This API follows the standard UART form.
  * \return Data Read
  */
uint8_t RNBD_read(void);

void RNBD_set_debug(bool debug_state);
void RNBD_check_connection();
bool RNBD_is_connected(void);
bool RNBD_is_OTA_complete(void);

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* CAPSENSE2_H_ */
/** @}*/

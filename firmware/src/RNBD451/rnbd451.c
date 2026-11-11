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
#include "rnbd451.h"
#include "rnbd_uart_hal.h"
#include <stdio.h>
#include <stdlib.h>

#define GENERAL_PRINT_SIZE_LIMIT        (100)
#define STATUS_MESSAGE_DELIMITER        ('%')

typedef enum 
{
    DISCONNECT_MSG  = 0,
    STREAM_OPEN_MSG = 1,
    GENERAL_MSG     = 2,
}RNBD_MESSAGE_TYPE;

bool rnbd_debug = false; // Print connection status on UART
static bool connected = false, OTAComplete = false; //**< RNBD connection state */
static uint8_t readbuffer[1];
static char cmdBuf[64];                                /**< Command TX Buffer */
static uint8_t dummyread;
static char *asyncBuffer;                           /**< Async Message Buffer */
static uint8_t asyncBufferSize;                     /**< Size of the Async Message Buffer */
static char *pHead;                                 /**< Pointer to the Head of the Async Message Buffer */
static uint8_t peek = 0;                            /**< Recieved Non-Status Message Data */
static bool dataReady = false;                      /**< Flag which indicates whether Non-Status Message Data is ready */

/*****************************************************
*   Driver Implementation Private API(s)
******************************************************/
static size_t uart_ble_write(uint8_t *buffer);
static uint8_t uart_ble_read(void);
static size_t uart_ble_data_ready(void);
static bool uart_ble_transmit_done(void);
static bool rnbd_filter_data(void);
static uint8_t rnbd_internal_read(void);
// Function used mainly for debug purposes, we should decide on a better approach for the future
static void uart_cdc_write(char *buffer);
static void uart_cdc_write_char(char buffer);

/*****************************************************
*   Driver Public API
******************************************************/

bool RNBD_is_connected(void)
{
    return connected;
}

bool RNBD_is_OTA_complete(void)
{
    return OTAComplete;
}

size_t RNBD_write(void *txData)
{
    return uart_ble_write((uint8_t*)txData);
}


bool RNBD_is_tx_done(void)
{
    return (uart_ble_transmit_done());
}

bool RNBD_is_rx_ready(void)
{
    return uart_ble_data_ready();
}

bool RNBD_init(void)
{
    uint8_t try_count = 5;
    bool rslt;
    
    if(!RNBD_available) return RNBD_E_DEV_NOT_FOUND;

    // ---WARNING--- Same mikroBUS as OLEDC, the module will be rebooted due to oledc_initialize()
    //Enter reset
//    RNBD_Reset(true);
    //Wait for Reset
//    RNBD_delay_ms(RNBD_RESET_DELAY_TIME);
    //Exit reset
//    RNBD_Reset(false);

    //Wait while RN Device is booting up
//    RNBD_delay_ms(RNBD_STARTUP_DELAY);

    while (try_count) {
        if (RNBD_is_rx_ready())
        {
            //Remove unread data sent by RNBD, if any    
            while (RNBD_is_rx_ready())
            {
                dummyread = rnbd_internal_read();
                rslt = RNBD_OK;
                connected = true;
            }
            break;
        }
        /* Wait for 1 ms */
        RNBD_delay_ms(1);
        --try_count;
    }
    if (!try_count)
        rslt = RNBD_E_DEV_NOT_FOUND;

    return rslt;
}

void RNBD_send_cmd(const char *cmd, uint8_t cmdLen)
{
    //Remove unread data sent by RNBD, if any    
    while (RNBD_is_rx_ready())
    {
        dummyread=rnbd_internal_read();
    }
    RNBD451_write((uint8_t *)cmd, cmdLen);
    RNBD_delay_ms(300); //Ensure the module had enough time to process the cmd
}

uint8_t RNBD_get_cmd(const char *getCmd, uint8_t getCmdLen, char *getCmdResp)
{
    uint8_t index = 0;

    RNBD_send_cmd(getCmd, getCmdLen);

    do
    {
        getCmdResp[index++] = (char)rnbd_internal_read();
    }
    while (getCmdResp[index - 1U] != '\n');

    return index;
}

bool RNBD_read_msg(const char *expectedMsg, uint8_t msgLen)
{
    uint8_t index;
    char resp;

    for (index = 0; index < msgLen; index++)
    {
        resp = (char)rnbd_internal_read();
            uart_cdc_write_char(resp);
        if (resp != (uint8_t)expectedMsg[index])
        {
            return false;
        }
    }

    return true;
}

bool RNBD_read_default_response(void)
{
    char resp[5];
    bool status = false;

    resp[0] = (char)rnbd_internal_read();
    resp[1] = (char)rnbd_internal_read();
    resp[2] = (char)rnbd_internal_read();    
    uart_cdc_write(&resp[0]);
    uart_cdc_write(&resp[1]);
    uart_cdc_write(&resp[2]);
    
    switch (resp[0])
    {
        case 'A':
        {
            if ((resp[1] == 'O') && (resp[2] == 'K'))
            {
                status = true;
            }
            break;
        }
        case 'E':
        {
            if ((resp[1] == 'r') && (resp[2] == 'r'))
            {
                status = false;
            }
            break;
        }
        default:
        {
            return status;
        }
    }

    /* Read carriage return and line feed comes with response */
    dummyread=rnbd_internal_read();
    dummyread=rnbd_internal_read();

    //Read CMD>
    dummyread=rnbd_internal_read();
    dummyread=rnbd_internal_read();
    dummyread=rnbd_internal_read();
    dummyread=rnbd_internal_read();
    dummyread=rnbd_internal_read();
  
    return status;
}

bool RNBD_enter_cmd_mode(void)
{
    const char cmdPrompt[] = {'C', 'M', 'D', '>', ' '};

    cmdBuf[0] = '$';
    cmdBuf[1] = '$';
    cmdBuf[2] = '$';

    RNBD_send_cmd(cmdBuf, 3U);

    return RNBD_read_msg(cmdPrompt, (uint8_t)sizeof (cmdPrompt));
}

bool RNBD_enter_data_mode(void)
{
    const char cmdPrompt[] = {'E', 'N', 'D', '\r', '\n'};

    cmdBuf[0] = '-';
    cmdBuf[1] = '-';
    cmdBuf[2] = '-';
    cmdBuf[3] = '\r';
    cmdBuf[4] = '\n';

    RNBD_send_cmd(cmdBuf, 5U);

    return RNBD_read_msg(cmdPrompt, (uint8_t)sizeof (cmdPrompt));
}

bool RNBD_reboot_cmd(void)
{
    const char reboot[] = {
        'R', 'e', 'b', 'o', 'o', 't', 'i', 'n', 'g', '\r', '\n'
    };

    cmdBuf[0] = 'R';
    cmdBuf[1] = ',';
    cmdBuf[2] = '1';
    cmdBuf[4] = '\r';
    cmdBuf[5] = '\n';

    RNBD_send_cmd(cmdBuf, 5U);

    return RNBD_read_msg(reboot, (uint8_t)sizeof (reboot));
}

bool RNBD_set_name(const char *name, uint8_t nameLen)
{
    uint8_t index;
    const char cmdPrompt[] = {
        'A', 'O', 'K', '\r', '\n', 'C', 'M', 'D', '>', ' '
    };

    cmdBuf[0] = 'S';
    cmdBuf[1] = 'N';
    cmdBuf[2] = ',';

    for (index = 0; index < nameLen; index++) {
        cmdBuf[3 + index] = name[index];
    }
    index = index + 3;

    cmdBuf[index++] = '\r';
    cmdBuf[index++] = '\n';
    RNBD_send_cmd(cmdBuf, nameLen + 5);
//    RNBD_delay_ms(10);
    return RNBD_read_msg(cmdPrompt, (uint8_t)sizeof(cmdPrompt));
}

bool RNBD_factory_reset(RNBD_FACTORY_RESET_MODE_t resetMode)
{
    const char reboot[] = {
        'R', 'e', 'b', 'o', 'o', 't', ' ', 'a', 'f', 't', 'e', 'r', ' ', 
        'F', 'a', 'c', 't', 'o', 'r', 'y', ' ', 
        'R', 'e', 's', 'e', 't', '\r', '\n'
    };
    
    cmdBuf[0] = 'S';
    cmdBuf[1] = 'F';
    cmdBuf[2] = ',';
    cmdBuf[4] = (char)resetMode;
    cmdBuf[5] = '\r';
    cmdBuf[6] = '\n';

    RNBD_send_cmd(cmdBuf, 6U);

    return RNBD_read_msg(reboot, (uint8_t)sizeof (reboot));
}

bool RNBD_disconnect(void)
{
    cmdBuf[0] = 'K';
    cmdBuf[1] = ',';
    cmdBuf[2] = '1';
    cmdBuf[3] = '\r';
    cmdBuf[4] = '\n';

    RNBD_send_cmd(cmdBuf, 5U);

    return RNBD_read_default_response();
}

bool RNBD_set_async_message_handler(char* pBuffer, uint8_t len)
{
    if ((pBuffer != NULL) && (len > 1U))
    {
      asyncBuffer = pBuffer;
      asyncBufferSize = len - 1U;
      return true;
    }
    else
    {
        return false;
    }
}

bool RNBD_data_ready(void)
{
    if (dataReady)
    {
        return true;
    }
    
    if (RNBD_is_rx_ready())
    {
        return rnbd_filter_data();
    }
    return false;
}

uint8_t RNBD_read(void)
{   
    RNDB_TIMEOUT to;
    RNDB_StartTimeOut(&to, 200);
    while(RNBD_data_ready() == false)
    {
        if(RNDB_IsTimeoutReached(&to))
        {
            peek = STATUS_MESSAGE_DELIMITER;
            break;            
        }
    } // Wait
    dataReady = false;
    return peek;
}

void RNBD_check_connection()
{
    while (RNBD_available_data())
        RNBD_data_ready();
}

void RNBD_reset(bool value)
{
    if (true == value)
    {
        RNBD_reset_low();
    }
    else
    {
        RNBD_reset_high();
    }
}

void RNBD_set_debug(bool debug_state)
{
    rnbd_debug = debug_state;
}
/*****************************************************
*   Driver Implementation Private API(s)
******************************************************/ 
// Function used mainly for debug purposes, we should decide on a better 
//approach for the future
static void uart_cdc_write_char(char buffer)
{
    if (rnbd_debug)
    {
        TERMINAL_Write(&buffer, 1);
        while(!TERMINAL_TXComplete());  
    }
}
static void uart_cdc_write(char *buffer)
{
    if (rnbd_debug)
    {
        TERMINAL_Write(buffer, strlen(buffer));
        while(!TERMINAL_TXComplete());  
    }
}
/**
 * \ingroup RNBD_INTERFACE
 * \brief Read API to capture data bytes from RNBD module
 * 
 * This API is used to receive data bytes to the RNBD module
 *
 * HINT: This API is in place to give compile time memory allocation.
 *       Functionality exist locally within file.
 *       Use of IN LINE to prevent additional stack depth requirement.
 *       APIs can be injected in place if suitable to save (1) stack depth level
 * 
 * \param N/A
 * \return uint8_t readDataByte - Byte captured from RNBD module
 */
static uint8_t rnbd_internal_read(void)
{
    return uart_ble_read();
}

static uint8_t uart_ble_read(void)
{
    RNBD451_read(readbuffer,1);
    return *(uint8_t*)readbuffer;
}

static size_t uart_ble_write(uint8_t *buffer)
{
    return RNBD451_write(buffer, strlen((char*)buffer));
}

static size_t uart_ble_data_ready(void)
{
    return RNBD_available_data();
}

static bool uart_ble_transmit_done(void)
{
    return RNBD_transmit_ready()? false:true;
}

/*****************************************************
*   *Optional* Message Formatting Private API(s)
******************************************************/  
/**
 * \ingroup RNBD_MESSAGE
 * \brief Prints the START Message "<<< " for UART_CDC
 * 
 * This API prints *Optional Application* Messages
 *
 * \param N/A
 * \return N/A
 */
static inline void rnbd_print_message_start(void)
{
    uart_cdc_write("<<< ");
}

/**
 * \ingroup RNBD_MESSAGE
 * \brief Prints the END Message ">>>\r\n" for UART_CDC
 * 
 * This API prints *Optional Application* Messages
 *
 * \param N/A
 * \return N/A
 */
static inline void rnbd_print_message_end(void)
{
    uart_cdc_write(" >>> \r\n");
}

/**
 * \ingroup RNBD_MESSAGE
 * \brief Prints the Indicator [ or ] to UART_CDC
 *        [ - Disconnected | ] - Connected
 * 
 * This API prints *Optional Application* Messages
 *
 * \param N/A
 * \return N/A
 */
static inline void rnbd_print_indicator_characters(RNBD_MESSAGE_TYPE msgType)
{
    if (DISCONNECT_MSG == msgType)
    {
        uart_cdc_write("[");
    }
    else if (STREAM_OPEN_MSG == msgType)
    {
        uart_cdc_write("]");
    }
    else
    {
    }
}

/* This API prints *Optional Application* Messages */
static inline void rnbd_print_message(char* passedMessage)
{
    char printCharacter [GENERAL_PRINT_SIZE_LIMIT];
    strcpy(printCharacter, passedMessage);
    uart_cdc_write(printCharacter); 
}

/**
 * \ingroup RNBD_INTERFACE
 * \brief Handle RNBD Status Message
 * 
 * This API is used to handle incoming status messages.
 * It prints all status messages, If DISCONNECT or STREAM_OPEN is received it 
 * manages the state of bool connected.
 * 
 * \param message Passed status message
 * \return Nothing
 */
static void rnbd_message_handler(char* message)
{
    RNBD_MESSAGE_TYPE messageType = GENERAL_MSG;

    if (strstr(message, "DISCONNECT")!= NULL)
    {
        messageType = DISCONNECT_MSG;
        connected = false;
        OTAComplete = false;
    }
    else if (strstr(message, "STREAM_OPEN")!= NULL)
    {
        messageType = STREAM_OPEN_MSG;
        connected = true;
    }
    else if (strstr(message, "OTA_REQ")!= NULL)
    {
        OTAComplete = true;
        RNBD_write("\r\nOTAA,01\r\n");
    }
    else
    {
        messageType = GENERAL_MSG;
    }

    rnbd_print_message_start();
    rnbd_print_message(message);
    rnbd_print_message_end();
    rnbd_print_indicator_characters(messageType);
}

static bool rnbd_filter_data(void)
{
    static bool asyncBuffering = false;
    
    uint8_t readChar = rnbd_internal_read();
    
    if(asyncBuffering == true)
    {
        if(readChar == (uint8_t)STATUS_MESSAGE_DELIMITER)
        {
            asyncBuffering = false;
            *pHead = '\0';
            rnbd_message_handler(asyncBuffer);
        }
        else if (pHead < asyncBuffer + asyncBufferSize)
        {
            *pHead++ = (char)readChar;
        }
        else
        {
            //do nothing
        }
    }
    else
    {
        if (readChar == (uint8_t)STATUS_MESSAGE_DELIMITER)
        {
            asyncBuffering = true;
            pHead = asyncBuffer;
        }
        else 
        {
            dataReady = true;
            peek = readChar;
        }
    }
    return dataReady;
}
/* ************************************************************************** 
 * Terminal like module
 * 
 *         
 *  @Company
 *    Microchip Technology Inc.
 *
 *  @File Name
 *    terminal.c
 *
 *  @Summary
 *    Terminal function implementations.
 *
 *  @Description
 *    Implements some terminal like functions to read and execute commands 
 *    typed by the user.
 *
 ***************************************************************************/

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

#include "terminal.h"

#define RX_TERM_SIZE 50

bool terminal_cmd_received = false;

uint8_t term_cnt = 0;
uint8_t rxTerminal_data = 0;

char rxTerminalBuffer[RX_TERM_SIZE] = {'\0'};

void TERMINAL_init(void)
{
    // register terminal callback and start waiting for data
    //TERMINAL_CBReg(TERMINAL_read_callback, 0);
    //TERMINAL_StartRead(&rxTerminal_data, 1); 
}

void TERMINAL_read_callback(uintptr_t context)
{
#ifdef TERMINAL_CALLBACK
    if(TERMINAL_ErrorGet() != USART_ERROR_NONE)
    {
        //Handle error case
        return;
    }

    #ifdef  TERMINAL_Read
    rxTerminal_data = TERMINAL_Read();
    #endif
    rxTerminalBuffer[term_cnt++] = rxTerminal_data;
    
    if (rxTerminal_data == '\n')
    {
        rxTerminalBuffer[term_cnt] = '\0';
        term_cnt = 0;
        terminal_cmd_received = true;
    }
    TERMINAL_StartRead(&rxTerminal_data, 1);     
#endif
}

bool TERMINAL_has_new_input(void)
{
#ifndef TERMINAL_CALLBACK
    while(TERMINAL_NewData())
    {
        rxTerminal_data = TERMINAL_ReadByte();
        rxTerminalBuffer[term_cnt++] = rxTerminal_data;    
        if (rxTerminal_data == '\n')
        {
            rxTerminalBuffer[term_cnt] = '\0';
            term_cnt = 0;
            terminal_cmd_received = true;
        }
    }
#endif
    return terminal_cmd_received;
}

char* TERMINAL_get_buffer(void)
{
    terminal_cmd_received = false;
    return rxTerminalBuffer;
}


/* *****************************************************************************
 End of File
 */

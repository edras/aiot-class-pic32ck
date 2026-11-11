//DOM-IGNORE-BEGIN
/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/
//DOM-IGNORE-END
/*******************************************************************************
  MCU specific stub code for OpenAlliance TC6 10BASE-T1S MACPHY via SPI protocol

  Company:
    Microchip Technology Inc.

  File Name:
    tc6-stub.c

  Summary:
    MCU specifc stub code

  Description:
    This file acts as a bridge between the TC6 library and the Board Support Package
    for the dedicated MCU
*******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "tc6/tc6-conf.h"
#include "tc6/tc6.h"
#include "t1s_lwip_hal.h"

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                          USER ADJUSTABLE                             */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

#define DEFAULT_SPI_SPEED       (15000000)
#define FIRST_TC6_INSTANCE      (0)
#define SPI_OFFSET              (0)
#define PRINT(...)              (void)printf(__VA_ARGS__)
#ifdef DEBUG
#define ASSERT(x)               __conditional_software_breakpoint(x)
#else
#define ASSERT(x)
#endif

static const uint8_t FALLBACK_MAC[] = {0x00u, 0x80u, 0xC2u, 0x00u, 0x01u, 0xCCu};

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                      DEFINES AND LOCAL VARIABLES                     */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

typedef struct
{
    uint8_t mac[6];
    uint8_t idx;
    bool opened;
    bool firstTransaction;
    uint8_t intIn;
    uint8_t intOut;
    uint8_t intReported;
    volatile bool busy;
} Stub_Local_t;

static Stub_Local_t d[TC6_MAX_INSTANCES] = { 0 };

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                      PRIVATE FUNCTION PROTOTYPES                     */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

static void IntHandler(uintptr_t context);
static bool GetMacAddress(uint8_t *);

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                         PUBLIC FUNCTIONS                             */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

bool TC6Stub_Init(uint8_t idx, uint8_t pMac[6])
{
    bool success = false;
    if (idx >= TC6_MAX_INSTANCES) {
        ASSERT(false);
    } else {
        Stub_Local_t *ps = &d[idx];
        ps->idx = idx;
        ps->firstTransaction = true;
        if (GetMacAddress(ps->mac)) 
        {
            memcpy(pMac, ps->mac, 6u);
        } 
        else 
        {
            memcpy(pMac, FALLBACK_MAC, 6u);
        }
        
        T1S_EIC_CB_Register(IntHandler, (uintptr_t)ps);
        T1S_RST_Clear();
        TIMER_DelayMs(10);
        T1S_RST_Set();
        TIMER_DelayMs(10);
        
        success = true;
    }
    return success;
}

bool TC6Stub_IntActive(uint8_t idx)
{
    Stub_Local_t *ps = &d[idx];
    ASSERT(idx < TC6_MAX_INSTANCES);
    ps->intReported = ps->intIn;
    return (ps->intReported != ps->intOut);
}

void TC6Stub_ReleaseInt(uint8_t idx)
{
    Stub_Local_t *ps = &d[idx];
    ASSERT(idx < TC6_MAX_INSTANCES);
    if (T1S_INT_Get()) 
    {
        ps->intOut = ps->intReported;
    }
}

bool TC6Stub_SpiTransaction(uint8_t idx, const uint8_t *pTx, uint8_t *pRx, uint16_t len)
{
    T1S_CS_Clear();
    T1S_SPI_WriteRead((void *)pTx, len, pRx, len);
    while(T1S_SPI_IsBusy());
    T1S_CS_Set();
    TC6_SpiBufferDone(idx, true);
    return true;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                  PRIVATE FUNCTION IMPLEMENTATIONS                    */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

static bool GetMacAddress(uint8_t *rData)
{
    uint8_t MAC_EEPROM_I2C_SLAVE_ADDR = 0x58;
    uint8_t MAC_EEPROM_EUI_REG_ADDR = 0x9A;
    bool success = false;
    if (NULL != rData) {
        if(T1S_I2C_WriteRead(MAC_EEPROM_I2C_SLAVE_ADDR, &MAC_EEPROM_EUI_REG_ADDR, 1, rData, 6))
        {
            while(T1S_I2C_IsBusy());
            success = true;
        }
        else
        {
            // Error
            PRINT("Failed to obtain MAC address from EEPROM\r\n");
        }
    }
    return success;
}

static void IntHandler(uintptr_t context)
{
    Stub_Local_t *ps = &d[0];
    ps->intIn++;
}

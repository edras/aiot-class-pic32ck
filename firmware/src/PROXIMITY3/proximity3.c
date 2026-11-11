/*------------------------------------------------------------------------------*/
/* Copyright 2020, Microchip Technology Inc. and its subsidiaries.              */
/*                                                                              */
/* Redistribution and use in source and binary forms, with or without           */
/* modification, are permitted provided that the following conditions are met:  */
/*                                                                              */
/* 1. Redistributions of source code must retain the above copyright notice,    */
/*    this list of conditions and the following disclaimer.                     */
/*                                                                              */
/* 2. Redistributions in binary form must reproduce the above copyright notice, */
/*    this list of conditions and the following disclaimer in the documentation */
/*    and/or other materials provided with the distribution.                    */
/*                                                                              */
/* 3. Neither the name of the copyright holder nor the names of its             */
/*    contributors may be used to endorse or promote products derived from      */
/*    this software without specific prior written permission.                  */
/*                                                                              */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"  */
/* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE    */
/* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   */
/* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE    */
/* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,             */
/* OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF      */
/* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;                 */
/* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,     */
/* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR      */
/* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,               */
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/*------------------------------------------------------------------------------*/

/******************************************************************************
 *  INCLUDES
 ******************************************************************************/

#include <string.h>
#include "proximity3.h"
#include "proximity3_i2c_hal.h"


/******************************************************************************
 *  DEFINES & MACROS
 ******************************************************************************/
#define VCNL4200_SLAVE_ADDRESS         0x51

/* Device ID Value. */
#define VCNL4200_DEVICE_ID_VALUE       0x1058

/* Table 1 command code and Register Description (Refer Table 1 Datasheet page 9). */
#define VCNL4200_ALS_CONF              0x00
#define VCNL4200_ALS_THDH              0x01
#define VCNL4200_ALS_THDL              0x02
#define VCNL4200_PS_CONF1              0x03
#define VCNL4200_PS_CONF2              0x03
#define VCNL4200_PS_CONF3              0x04
#define VCNL4200_PS_MS                 0x04
#define VCNL4200_PS_CANC               0x05
#define VCNL4200_PS_THDL               0x06
#define VCNL4200_PS_THDH               0x07
#define VCNL4200_PS_Data               0x08
#define VCNL4200_ALS_Data              0x09
#define VCNL4200_White_Data            0x0A
#define VCNL4200_INT_Flag              0x0D
#define VCNL4200_ID                    0x0E

/* Table 2 Register: ALS_Conf Description (Refer Table 2 Datasheet page 10). */
#define VCNL4200_ALS_SD_ON             0x00
#define VCNL4200_ALS_SD_OFF            0x01
#define VCNL4200_ALS_INT_EN            0x00
#define VCNL4200_ALS_INT_DIS           0x02
#define VCNL4200_ALS_PERS_1            0x00
#define VCNL4200_ALS_PERS_2            0x04
#define VCNL4200_ALS_PERS_3            0x08
#define VCNL4200_ALS_PERS_4            0x0C
#define VCNL4200_ALS_INT_SWITCH_0      0x00
#define VCNL4200_ALS_INT_SWITCH_1      0x20
#define VCNL4200_ALS_IT_50ms           0x00
#define VCNL4200_ALS_IT_100ms          0x40
#define VCNL4200_ALS_IT_200ms          0x80
#define VCNL4200_ALS_IT_400ms          0xC0

/* Table 6 Register: PS_CONF1 Description (Refer Table 6 Datasheet page 10). */
#define VCNL4200_PS_SD_ON              0x00
#define VCNL4200_PS_SD_OFF             0x01
#define VCNL4200_PS_IT_1T              0x00
#define VCNL4200_PS_IT_1_5T            0x02
#define VCNL4200_PS_IT_2T              0x04
#define VCNL4200_PS_IT_4T              0x06
#define VCNL4200_PS_IT_8T              0x08
#define VCNL4200_PS_IT_9T              0x0A
#define VCNL4200_PS_PERS_1             0x00
#define VCNL4200_PS_PERS_2             0x10
#define VCNL4200_PS_PERS_3             0x20
#define VCNL4200_PS_PERS_4             0x30
#define VCNL4200_PS_DUTY_1_160         0x00
#define VCNL4200_PS_DUTY_1_320         0x40
#define VCNL4200_PS_DUTY_1_640         0x80
#define VCNL4200_PS_DUtY_1_1280        0xC0

/* Table 7 Register: PS_CONF2 Description (Refer Table 7 Datasheet page 11). */
#define VCNL4200_PS_INT_DIS            0x00
#define VCNL4200_PS_INT_Trig_Clos      0x01
#define VCNL4200_PS_INT_Trig_away      0x02
#define VCNL4200_PS_INT_Trig_Clos_away 0x03
#define VCNL4200_PS_HD_12bits          0x00
#define VCNL4200_PS_HD_16bits          0x04

/* Table 8 Register: PS_CONF3 Description (Refer Table 8 Datasheet page 11). */
#define VCNL4200_PS_SC_EN_Enable       0x01
#define VCNL4200_PS_SC_EN_Disable      0x00
#define VCNL4200_PS_SC_ADV_Typ_sunx1   0x00
#define VCNL4200_PS_SC_ADV_Typ_Sunx2   0x02
#define VCNL4200_PS_TRIG               0x04
#define VCNL4200_PS_AF_Enable          0x08
#define VCNL4200_PS_AF_Disable         0x00
#define VCNL4200_PS_SMART_PERS_Disable 0x00
#define VCNL4200_PS_SMART_PERS_Enable  0x10
#define VcNL4200_PS_MPS_1              0x00
#define VCNL4200_PS_MPS_2              0x20
#define VCNL4200_PS_MPS_4              0x40
#define VCNL4200_PS_MPS_8              0x60

/* Table 9 Register: PS_MS Description (Refer Table 9 Datasheet page 11). */
#define VCNL4200_LED_I_50mA            0x00
#define VCNL4200_LED_I_75mA            0x01
#define VCNL4200_LED_I_100mA           0x02
#define VCNL4200_LED_I_120mA           0x03
#define VCNL4200_LED_I_140mA           0x04
#define VCNL4200_LED_I_160mA           0x05
#define VCNL4200_LED_I_180mA           0x06
#define VCNL4200_LED_I_200mA           0x07
#define VCNL4200_PS_SPO_00h            0x00
#define VCNL4200_PS_SPO_FFh            0x08
#define VCNL4200_PS_SP_1               0x00
#define VCNL4200_PS_SP_1_5             0x10
#define VCNL4200_Ps_MS_Normal          0x00
#define VCNL4200_PS_MS_Logic           0x20

/******************************************************************************
 *  VARIABLES
 ******************************************************************************/
/* 2 buffer used one for proximity and ambience. */
static uint8_t m_buffer[2][2];
bool vcnl4200_debug = false;
bool vcnl4200_ok = false;
uint16_t proximity = 0xFFFF;
uint16_t ambient = 0xFFFF;

/******************************************************************************
 *  FUNCTION DECLARATIONS
 ******************************************************************************/
static uint16_t vcnl4200_get_id(void);
static bool vcnl4200_ambient_init(void);
static bool vcnl4200_proximity_init(void);
static bool vcnl4200_i2c_transfer(uint8_t address, uint8_t buffer_index);

/******************************************************************************
 *  FUNCTION DEFINITIONS
 *****************************************************************************/
/******************************************************************************
 ** Function Name	: vcnl4200_init
 ** Description		: Initialize the VCNL4200 sensor.
 ******************************************************************************/
void vcnl4200_init(void)
{
    /* Initialize the Ambient and proximity sensor. */
    vcnl4200_ok = vcnl4200_proximity_init() &&
        vcnl4200_ambient_init() &&
        (vcnl4200_get_id() == VCNL4200_DEVICE_ID_VALUE);

    if(vcnl4200_debug)
    {
        if (vcnl4200_ok)  {
            printf("> VCNL4200: Initialized. \r\n");
        } else {
            printf("> VCNL4200: Initialization failed. \r\n");
        }
    }
}

bool vcnl4200_available(void)
{
    return vcnl4200_ok;
}

void vcnl4200_set_debug(bool state)
{
    vcnl4200_debug = state;
}

bool vcnl4200_get_values(uint16_t *pProximity, uint16_t *pAmbience)
{
    bool success = false;
    if(!vcnl4200_ok) return false;
    if (vcnl4200_i2c_transfer(VCNL4200_PS_Data, 0) && vcnl4200_i2c_transfer(VCNL4200_ALS_Data, 1)) {
        if (NULL != pProximity) {
            *pProximity = ((uint16_t)(m_buffer[0][1] << 8) | m_buffer[0][0]);
        }
        if (NULL != pAmbience) {
            *pAmbience = (uint16_t)(m_buffer[1][1] << 8) | m_buffer[1][0];
        }
        success = true;
    }
    return success;
}

void vcnl4200_read_data(void)
{
    if(!vcnl4200_ok) return;
    bool rslt = vcnl4200_get_values(&proximity, &ambient);    
    if(vcnl4200_debug)
    {
        if(rslt)    printf("\rProximity=%d  Ambient=%d\r\n", proximity, ambient);
        else        printf("\rProximity read failed\r\n");
    }
}

/******************************************************************************
 *  STATIC FUNCTION DEFINITIONS
 ******************************************************************************/
/******************************************************************************
 ** Function Name	: vcnl4200_ambient_init
 ** Description		: Initialize the Ambient light measurement with the default values.
 ******************************************************************************/
static bool vcnl4200_ambient_init(void)
{
    bool success = false;
#ifdef MIKROE_PROXIMITY3
    uint8_t writeBuffer[3];
    writeBuffer[0] = VCNL4200_ALS_CONF;
    writeBuffer[1] = 0;
    writeBuffer[2] = 0;

    /* Wait till transmission is complete. */
    while (PROXIMITY3_I2C_IsBusy());

    success = PROXIMITY3_I2C_WRITE(VCNL4200_SLAVE_ADDRESS, writeBuffer, 3);

    /* Wait till transmission is complete. */
    while (PROXIMITY3_I2C_IsBusy());
#endif
    return success;
}

/******************************************************************************
 ** Function Name	: vcnl4200_proximity_init
 ** Description		: Initialize the Proximity measurement with the default values.
 ******************************************************************************/
static bool vcnl4200_proximity_init(void)
{
    bool success = false;
#ifdef MIKROE_PROXIMITY3
    uint8_t writeBuffer[3];

    /* Wait till transmission is complete. */
    while (PROXIMITY3_I2C_IsBusy());

    writeBuffer[0] = VCNL4200_PS_CONF1;
    writeBuffer[1] = VCNL4200_PS_IT_9T | VCNL4200_PS_DUTY_1_320;
    writeBuffer[2] = VCNL4200_PS_AF_Enable;
    success = PROXIMITY3_I2C_WRITE(VCNL4200_SLAVE_ADDRESS, writeBuffer, 3);

    /* Wait till transmission is complete. */
    while (PROXIMITY3_I2C_IsBusy());

    writeBuffer[0] = VCNL4200_PS_CONF3;
    writeBuffer[1] = VCNL4200_PS_SC_EN_Enable;
    writeBuffer[2] = VCNL4200_LED_I_50mA;
    success = success && PROXIMITY3_I2C_WRITE(VCNL4200_SLAVE_ADDRESS, writeBuffer, 3);

    /* Wait till transmission is complete. */
    while (PROXIMITY3_I2C_IsBusy());
#endif
    return success;
}

/******************************************************************************
 ** Function Name	: vcnl4200_get_id
 ** Description		: Read the sensor Id.
 ** Return		    : Returns the sensor Id.
 ******************************************************************************/
static uint16_t vcnl4200_get_id(void)
{
    vcnl4200_i2c_transfer(VCNL4200_ID, 0);
    return (uint16_t)(m_buffer[0][1] << 8) | m_buffer[0][0];
}

/******************************************************************************
 ** Function Name	: vcnl4200_i2c_transfer
 ** Description		: Read data from Proximity sensor ( using I2C restart).
 ** Return		    : Returns the transmission status.
 ******************************************************************************/
static bool vcnl4200_i2c_transfer(uint8_t address, uint8_t buffer_index)
{
    bool success = false;
    m_buffer[buffer_index][0] = address;

    while (PROXIMITY3_I2C_IsBusy()) {};
    success = PROXIMITY3_I2C_WRITEREAD(VCNL4200_SLAVE_ADDRESS, m_buffer[buffer_index], 1, m_buffer[buffer_index], 2);
    if (success) {
        while (PROXIMITY3_I2C_IsBusy()) {};
    }
    return success;
}


/*------------------------------ End of File ----------------------------------*/
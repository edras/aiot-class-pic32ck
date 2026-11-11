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
#include "apc1_i2c_hal.h"
#include "APC1/apc1.h"

#ifndef MIKROE_APC1
int8_t APC1_init(){ return -1; }
void APC1_set_debug(bool state) {}
void APC1_print_csv_data(){}
void APC1_print_data(){}
bool APC1_available(){ return false; }
APC1_RC APC1_get_measurement(void){ return -1; }
#endif

bool apc1_i2c_read(uint8_t devaddr, uint8_t *regdata, uint16_t len)
{    
    APC1_I2C_Read(devaddr, regdata, len);
    
    while(APC1_I2C_IsBusy());
    
    if(APC1_I2C_ErrorGet() == APC1_I2C_ERROR_NONE)
    {
        return true;
    }
    else 
    {
        return false;
    }
}

bool apc1_i2c_read_from_addr(uint8_t devaddr, uint8_t regaddr, uint8_t *regdata, uint16_t len)
{    
    APC1_I2C_WriteRead(devaddr, &regaddr, 1, regdata, len);
    
    while(APC1_I2C_IsBusy());
    
    if(APC1_I2C_ErrorGet() == APC1_I2C_ERROR_NONE)
    {
        return true;
    }
    else 
    {
        return false;
    }
}

bool apc1_i2c_write(uint8_t devaddr, uint8_t regaddr, uint8_t *regdata, uint16_t len)
{
#ifdef MIKROE_APC1
	uint8_t temp_data[64], i;
	temp_data[0] = regaddr;
	for (i = 0; i < len; i++) {
		temp_data[i + 1] = regdata[i];
	}

    APC1_I2C_Write(devaddr, temp_data, len+1);
//    APC1_I2C_Write(devaddr, regdata, len);
    
    while(APC1_I2C_IsBusy());
    
    if(APC1_I2C_ErrorGet() == APC1_I2C_ERROR_NONE)
    {
        return true;
    }
    else 
    {
        return false;
    }
#else
    return true;
#endif
}
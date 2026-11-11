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
#include "apc1.h"
#include "apc1_i2c_hal.h"
#include <string.h>

apc1_data apc1_init_data;
apc1_measurement apc1;
bool apc1_debug = false;
static char message[100];
static bool apc1_connected = false;

/*****************************************************
*   Local helper functions
******************************************************/
static void apc1_print_msg(void)
{
    uint8_t len = strlen(message);
    TERMINAL_Write(&message, len);
    while(!TERMINAL_TXComplete());  
}

static uint16_t apc1_chksum (uint8_t *data_in, uint8_t len)
{
    uint8_t i;
    uint16_t chksum = 0;
    for (i = 0; i < len; i++)
    {
        chksum += data_in[i];
    }
    return chksum;
}

static bool check_msg (uint8_t *data_buf, uint32_t len)
{
    uint8_t i;
    uint16_t chksum = 0;
    
    if ((APC1_HEADER_0 != data_buf[0]) || (APC1_HEADER_1 != data_buf[1]))
    {
        if (apc1_debug)
        {
            sprintf(message, "\nAPC1 Dbg: Message header invalid!\r\n");
            apc1_print_msg();
        }
        return false;
    }

    for (i = 0; i < len - 2; i++)
    {
        chksum += data_buf[i];
    }
    
    if (((uint8_t)((chksum >> 8) & 0xFF) != data_buf[len-2]) || 
         (uint8_t)((chksum & 0xFF) != data_buf[len-1]))
    {
        if (apc1_debug)
        {
            sprintf(message, "\nAPC1 Dbg: Message checksum invalid!\r\n");
            apc1_print_msg();
        }
        return false;
    }
    
    return true;
}

static APC1_RC apc1_send_command(uint8_t cmd, uint16_t mode)
{
    uint8_t data_buf[7] = {0};
    uint16_t chksum = 0;
    
    if (((APC1_CMD_SET_MODE != cmd)  && (APC1_CMD_GET_INFO != cmd)) || 
        ((APC1_MODE_IDLE != mode ) && (APC1_MODE_MEASUREMENT != mode) && (APC1_MODE_RESET != mode)))
    {
        if (apc1_debug)
        {
            sprintf(message, "\nAPC1 Dbg: Invalid Cmd or Mode!\r\n");
            apc1_print_msg();
        }
        return APC1_E_INVALID;
    }

    data_buf[0] = APC1_HEADER_0;
    data_buf[1] = APC1_HEADER_1;
    data_buf[2] = cmd;
    data_buf[3] = (uint8_t) ((mode >> 8) & 0xFF);
    data_buf[4] = (uint8_t) (mode & 0xFF);
    chksum = apc1_chksum (&data_buf[0], 5);
    data_buf[5] = (uint8_t) ((chksum >> 8) & 0xFF);
    data_buf[6] = (uint8_t) (chksum & 0xFF);

    apc1_i2c_write(APC1_DEV_ADDR, APC1_ADDRESS_CMD, data_buf, 7);

	return APC1_OK;
}

static APC1_RC apc1_get_cmd(uint8_t *data_buf, uint32_t len)
{
    apc1_i2c_read_from_addr(APC1_DEV_ADDR, APC1_ADDRESS_RESPONSE, data_buf, len);
    if (!check_msg(data_buf, len))
    {
        return APC1_E_COMM_FAIL;
    }
    
    return APC1_OK;
}

static APC1_RC apc1_get_meas(uint8_t *data_buf, uint32_t len)
{
    apc1_i2c_read(APC1_DEV_ADDR, data_buf, len);
    if (!check_msg(data_buf, len))
    {
        if (apc1_debug)
        {
            sprintf(message, "\nAPC1 Dbg: Measurement read failed!\r\n");
            apc1_print_msg();
        }
        return APC1_E_COMM_FAIL;
    }
    
    return APC1_OK;
}
/*****************************************************
*   Driver Public API
******************************************************/

APC1_RC APC1_init()
{
    uint8_t try_count = 5;
    uint8_t data_buf[APC1_INFO_LEN] = {0};

    APC1_cs_high();
    APC1_rst_high();
    APC1_delay_ms(200);
    
    /* Don't take into account the comm fct return value as this is the init phase */
    while (try_count) {
        /* Read the module type, ID and FW version of APC1 sensor */
        apc1_send_command(APC1_CMD_GET_INFO, APC1_MODE_IDLE);
        APC1_delay_ms(APC1_ANSWER_TIME);
        apc1_get_cmd(data_buf, APC1_INFO_LEN);
        /* Check for chip validity */
        if ((data_buf[0] == 0x42) && (data_buf[1] == 0x4D) &&
            (data_buf[2] == 0x00) && (data_buf[3] == 0x13) &&
            (data_buf[4] == 0x41) && (data_buf[5] == 0x50) && (data_buf[9] == 0x49))
        {
            /* Check for message integrity only if the expected characters are found*/
            if (!check_msg(data_buf, APC1_INFO_LEN))
            {
                if (apc1_debug)
                {
                    sprintf(message, "\nAPC1 Dbg: Communication failed due to message check\r\n");
                    apc1_print_msg();
                }
                return APC1_E_COMM_FAIL;
            }
            memcpy(apc1_init_data.name, &data_buf[4], 4);
            apc1_init_data.name[4] = '\0';
            apc1_init_data.type = data_buf[9];
            memcpy(apc1_init_data.serial_number, &data_buf[10], 8);
            apc1_init_data.firmware = ((uint16_t)data_buf[19] << 8) | data_buf[20];
    
            break; 
        }
        /* Wait for 10 ms */
        APC1_delay_ms(10);
        --try_count;
    }
    /* Chip id check failed */
    if (!try_count)
    {
        return APC1_E_DEV_NOT_FOUND;
    }
    apc1_connected = true;
	return APC1_OK;
}

bool APC1_available(void)
{
    return apc1_connected;
}

APC1_RC APC1_get_info(apc1_data *data)
{
    APC1_RC rslt;
    uint8_t data_buf[APC1_INFO_LEN] = {0};

    APC1_delay_ms(200);
    /* Read the module type, ID and FW version of APC1 sensor */
    rslt = apc1_send_command(APC1_CMD_GET_INFO, APC1_MODE_IDLE);
    if (rslt != APC1_OK)
    {
        return rslt;
    }
    APC1_delay_ms(APC1_ANSWER_TIME);
    rslt = apc1_get_cmd(data_buf, APC1_INFO_LEN);
    if (rslt != APC1_OK)
    {
        return rslt;
    }

    memcpy(data->name, &data_buf[4], 4);
    data->name[4] = '\0';
    data->type = data_buf[9];
    memcpy(data->serial_number, &data_buf[10], 8);
    data->firmware = ((uint16_t)data_buf[19] << 8) | data_buf[20];
    
	return rslt;
}

APC1_RC APC1_deep_sleep(bool state)
{
    if (state)
    {
        APC1_cs_low();
        APC1_rst_high();
    }
    else
    {
        APC1_cs_high();
        APC1_rst_high();
    }
    
    return APC1_OK;
}

APC1_RC APC1_set_idle_mode()
{
    APC1_RC rslt;
    uint8_t data_buf[8];
    uint8_t expected_data[8] = {0x42, 0x4D, 0x00, 0x04, 0xE4, 0x00, 0x01, 0x77}; // The expected response

    rslt = apc1_send_command(APC1_CMD_SET_MODE, APC1_MODE_IDLE);
    if (rslt != APC1_OK)
    {
        return rslt;
    }
    APC1_delay_ms(APC1_ANSWER_TIME);
    rslt = apc1_get_cmd(data_buf, 8);
    if (rslt != APC1_OK)
    {
        return rslt;
    }
    
    // Compare the received data with the expected data
    if (memcmp(data_buf, expected_data, sizeof(expected_data)) == 0)
    {
        return APC1_OK;
    } 
    else
    {
        return APC1_E_NOK;
    }
}

APC1_RC APC1_set_meas_mode()
{
    APC1_RC rslt;
    uint8_t data_buf[APC1_MEAS_LEN];

    rslt = apc1_send_command(APC1_CMD_SET_MODE, APC1_MODE_MEASUREMENT);
    if (rslt != APC1_OK)
    {
        return rslt;
    }
    APC1_delay_ms(APC1_ANSWER_TIME);
    rslt = apc1_get_meas(data_buf, APC1_MEAS_LEN);
    if (rslt != APC1_OK)
    {
        return rslt;
    }
    
    // Check the ERROR code
    if (data_buf[61] == 0)
    {
        return APC1_OK;
    }
    else
    {
        return APC1_E_NOK;
    }
}

apc1_measurement* APC1_get_data(void)
{
    return &apc1;
}

APC1_RC APC1_get_measurement(void)
{
    APC1_RC rslt;
    uint8_t data_buf[APC1_MEAS_LEN] = {0};
    rslt = apc1_get_meas(data_buf, APC1_MEAS_LEN);
    
    apc1.pm1_0 = ((uint16_t)data_buf[4] << 8) | data_buf[5];
    apc1.pm2_5 = ((uint16_t) data_buf[6] << 8) | data_buf[7];
    apc1.pm10 = ((uint16_t) data_buf[8] << 8) | data_buf[9];
    apc1.pm1_0_air = ((uint16_t) data_buf[10] << 8) | data_buf[11];
    apc1.pm2_5_air = ((uint16_t) data_buf[12] << 8) | data_buf[13];
    apc1.pm10_air = ((uint16_t) data_buf[14] << 8) | data_buf[15];
    apc1.part_over_0_3um = ((uint16_t) data_buf[16] << 8) | data_buf[17];
    apc1.part_over_0_5um = ((uint16_t) data_buf[18] << 8) | data_buf[19];
    apc1.part_over_1_0um = ((uint16_t) data_buf[20] << 8) | data_buf[21];
    apc1.part_over_2_5um = ((uint16_t) data_buf[22] << 8) | data_buf[23];
    apc1.part_over_5_0um = ((uint16_t) data_buf[24] << 8) | data_buf[25];
    apc1.part_over_10um = ((uint16_t) data_buf[26] << 8) | data_buf[27];
    apc1.tvoc = ((uint16_t) data_buf[28] << 8) | data_buf[29];
    apc1.eco2 = ((uint16_t) data_buf[30] << 8) | data_buf[31];
    apc1.t_comp = (((uint16_t) data_buf[34] << 8) | data_buf[35]) * 0.1f;
    apc1.rh_comp = (((uint16_t) data_buf[36] << 8 ) | data_buf[37]) * 0.1f;
    apc1.t_raw = (((uint16_t) data_buf[38] << 8 ) | data_buf[39]) * 0.1f;
    apc1.rh_raw = (((uint16_t) data_buf[40] << 8 ) | data_buf[41]) * 0.1f;
    apc1.rs0 = ((uint32_t) data_buf[42] << 24) | ((uint32_t) data_buf[43] << 16) | 
                 ((uint16_t) data_buf[44] << 8) | data_buf[45];
    apc1.rs1 = ((uint32_t) data_buf[46] << 24) | ((uint32_t) data_buf[47] << 16) | 
                 ((uint16_t) data_buf[48] << 8) | data_buf[49];
    apc1.rs2 = ((uint32_t) data_buf[50] << 24) | ((uint32_t) data_buf[51] << 16) | 
                 ((uint16_t) data_buf[52] << 8) | data_buf[53];
    apc1.rs3 = ((uint32_t) data_buf[54] << 24) | ((uint32_t) data_buf[55] << 16) | 
                 ((uint16_t) data_buf[56] << 8) | data_buf[57];
    apc1.aqi = data_buf[58];
    apc1.firmware = data_buf[60];
    apc1.error_code = data_buf[61];
    
    if (rslt != APC1_OK)
    {
        return rslt;
    }
    if (apc1.error_code != 0)
    {
        return APC1_E_NOK;
    }
    else
    {
        return APC1_OK;
    }
}

APC1_RC APC1_soft_reset()
{
    APC1_RC rslt;

    rslt = apc1_send_command(APC1_CMD_SET_MODE, APC1_MODE_RESET);
    APC1_delay_ms(APC1_RESET_TIME);
    
    return rslt;
}

APC1_RC APC1_hard_reset()
{
    APC1_rst_low();
    APC1_delay_ms(1000);
    APC1_rst_high();
    APC1_delay_ms(APC1_RESET_TIME);
    
    return APC1_OK;
}

void APC1_set_debug(bool state)
{
    apc1_debug = state;
}


void APC1_print_data()
{
    if (APC1_get_measurement() == APC1_OK)
    {
        printf("**** APC1 Mikroe - sensor ****\r\n");
        /* t_comp and rh_comp are already in degC and %RH - the raw 0.1-unit
           readings are scaled once when they are parsed (see the * 0.1f at the
           assignments above). Scaling again here made the terminal report a
           tenth of the real value, e.g. "2.2 C" for 22.4 degC and "6.4 %RH" for
           64 %RH, and disagree with both the OLED and the MQTT topics, which
           use these fields directly. */
        printf("APC1 Temperature: %.1f C\r\n", apc1.t_comp);
        printf("APC1 Humidity: %.1f %%RH\r\n", apc1.rh_comp);
        printf("APC1 CO2:  %d ppm\r\n", apc1.eco2);
        printf("APC1 Air Quality Index:  %d\r\n", apc1.aqi);
        printf("APC1 TVOC:  %u\r\n", apc1.tvoc);
        printf("APC1 PM1.0 mass concentration:  %u\r\n", apc1.pm1_0);
        printf("APC1 PM2.5 mass concentration:  %u\r\n", apc1.pm2_5);
        printf("APC1 PM10 mass concentration:  %u\r\n", apc1.pm10);
        printf("APC1 PM1.0 mass concentration in atmospheric environment:  %u\r\n", apc1.pm1_0_air);
        printf("APC1 PM2.5 mass concentration in atmospheric environment:  %u\r\n", apc1.pm2_5_air);
        printf("APC1 PM10 mass concentration in atmospheric environment:  %u\r\n", apc1.pm10_air);
        printf("APC1 Number of particles with diameter > 0.3um in 0.1L of air:  %u\r\n", apc1.part_over_0_3um);
        printf("APC1 Number of particles with diameter > 0.5um in 0.1L of air:  %u\r\n", apc1.part_over_0_5um);
        printf("APC1 Number of particles with diameter > 1.0um in 0.1L of air:  %u\r\n", apc1.part_over_1_0um);
        printf("APC1 Number of particles with diameter > 2.5um in 0.1L of air:  %u\r\n", apc1.part_over_2_5um);
        printf("APC1 Number of particles with diameter > 10um in 0.1L of air:  %u\r\n", apc1.part_over_10um);
    }
    else
    {
        printf("APC1 Error Code:  0x%x\r\n", apc1.error_code);
    }    
}
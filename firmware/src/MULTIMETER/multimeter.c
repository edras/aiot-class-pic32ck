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

#include "multimeter.h"
#include "multimeter_spi_hal.h"

#ifdef MIKROE_MULTIMETER

static uint8_t I_CHANNEL = 0x00;
static uint8_t U_CHANNEL = 0x01;
static uint8_t R_CHANNEL = 0x02;
static uint8_t C_CHANNEL = 0x03;

static float calibrationC = 0;
static float calibrationI = 0;
static float calibrationU = 0;
static uint8_t resistor_select;
static bool multimeter_available = false;

/* Resistor values */
static float resistor_array[6]={ 100.0, 1000.0, 10000.0, 100000.0, 470000.0, 1000000.0 };
static float V_max = 4095.0;

static uint16_t read_channel_value(uint8_t channel)
{
    uint16_t result = 0;
    uint8_t wr[3] = {0x06, (channel & 0x03) << 6, 0};
    uint8_t rd[3] = {0, 0, 0};

    MULTIMETER_cs_low();
    MULTIMETER_delay_ms(1);

    if(!MULTIMETER_spi_write_read(wr, 3, rd, 3))
    {
        MULTIMETER_cs_high();
        return 0;  // SPI error
    }

    result = rd[1] & 0x0F;
    result <<= 8;
    result |= rd[2];

    MULTIMETER_cs_high();

    return result;
}

static void calibrate_R()
{
  resistor_select = 0;
}

static void calibrate_C()
{
    float aux;
    aux = read_channel_value(C_CHANNEL)/2;
    calibrationC = (64285 / (aux*4));
}

static void calibrate_I()
{
    calibrationI =  read_channel_value(I_CHANNEL)/2;
}

static void calibrate_U()
{
    calibrationU =  read_channel_value(U_CHANNEL)/2;
}

static uint16_t multimeter_measure_R_range(uint8_t range)
{
    uint16_t Rv;

    if (range & 0x1)
        MULTIMETER_range_sel_0_high();
    else
        MULTIMETER_range_sel_0_low();
    if (range & 0x2)
        MULTIMETER_range_sel_1_high();
    else
        MULTIMETER_range_sel_1_low();
    if (range & 0x4)
        MULTIMETER_range_sel_2_high();
    else
        MULTIMETER_range_sel_2_low();

    MULTIMETER_delay_ms(100);
    
    Rv = read_channel_value(R_CHANNEL);

    return Rv;
}

/* --------------------------------------------------------- PUBLIC FUNCTIONS */
bool MULTIMETER_init(void)
{
    uint16_t test_value;

    // Initialize range selection pins to default (lowest range)
    MULTIMETER_range_sel_0_low();
    MULTIMETER_range_sel_1_low();
    MULTIMETER_range_sel_2_low();
    MULTIMETER_cs_high();

    MULTIMETER_delay_ms(10);

    // Test SPI communication by reading a channel
    test_value = read_channel_value(U_CHANNEL);

    // MCP3204 returns 12-bit values (0-4095), a valid response indicates presence
    // If SPI fails or no device, we typically get 0 or 0xFFF
    multimeter_available = (test_value > 0 && test_value < 4095);

    if(multimeter_available)
    {
        MULTIMETER_calibrate();
    }

    return multimeter_available;
}

bool MULTIMETER_available(void)
{
    return multimeter_available;
}

void MULTIMETER_calibrate(void)
{
    calibrate_U();
    calibrate_R();
    calibrate_C();
    calibrate_I();
}

float MULTIMETER_measure_I(void)
{
    float aux, current;

    aux = read_channel_value(I_CHANNEL) / 2.0f;
    current = aux - calibrationI;

    return current;
}

float MULTIMETER_measure_U(void)
{
    float aux, voltage;

    aux = read_channel_value(U_CHANNEL) / 2.0f;
    voltage = (aux - calibrationU) * 33.0f / 2.0f;

    return voltage;
}

float MULTIMETER_measure_C(void)
{
    float aux, capacitance;

    aux = read_channel_value(C_CHANNEL);

    // Avoid division by zero
    if (aux == 0)
    {
        return 0.0f;
    }

    capacitance = ((64285.0f / (aux * 2.0f)) - calibrationC) * 2.0f;  // Calculate capacitance in nF

    if (capacitance < 1.0f)
    {
        capacitance = 0.0f;
    }

    return capacitance;
}

float MULTIMETER_measure_R(void)
{
    float resistance;
    uint16_t aux;
    uint8_t attempts;

    // Auto-range: find the optimal range for measurement
    for (attempts = 0; attempts < 6; attempts++)
    {
        aux = multimeter_measure_R_range(resistor_select);

        // Value too high (overflow) - need lower resistor range
        if (aux > 4090)
        {
            if (resistor_select > 0)
            {
                resistor_select--;
                continue;  // Retry with lower range
            }
            break;  // Already at lowest range
        }

        // Value too low (underflow) - need higher resistor range
        if (aux < 100)
        {
            if (resistor_select < 5)
            {
                resistor_select++;
                continue;  // Retry with higher range
            }
            break;  // Already at highest range
        }

        // Value is in valid range
        break;
    }

    // Avoid division by zero
    if (aux == 0)
    {
        return 0.0f;
    }

    // Calculate resistance: R_x = R_ref * (V_max / V_measured - 1)
    resistance = (((resistor_array[resistor_select] * V_max) / aux) - resistor_array[resistor_select]);

    // Ensure non-negative result
    if (resistance < 0)
    {
        resistance = 0.0f;
    }

    return resistance;
}

#endif /* MIKROE_MULTIMETER */
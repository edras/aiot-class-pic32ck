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
#include <stdio.h>
#include <string.h>

#include "re46c42x.h"
#include "re46c42x_spi_hal.h"

/*****************************************************
*   Driver Public API
******************************************************/

static uint16_t pa1_normal_val; //used for comparison when check is done to see if smoke was detected 
static uint16_t pa2_normal_val;
static uint8_t pa1_conf = RE46C42X_REG6_CONF_200US_GAIN_2; // default reg 6 that will be used by PA1 when checking for smoke
static uint8_t pa2_conf = RE46C42X_REG6_CONF_200US_GAIN_2; // default reg 6 that will be used by PA2 when checking for smoke
static void RE46C42X_save_pa1_pa2_no_smoke_val();
bool RE46C42X_debug = false;
bool RE46C42X_connected = false;
char message[100];

static void RE46C42X_print_msg(void)
{
    uint8_t len = strlen(message);
    TERMINAL_Write(&message, len);
    while(!TERMINAL_TXComplete());  
}

bool RE46C42X_available(void)
{
    return RE46C42X_connected;
}

void RE46C42X_set_debug(bool state)
{
    RE46C42X_debug = state;
}

void RE46C42X_write_reg(uint8_t reg, uint8_t data)
{
    uint8_t spi_data[2];
    spi_data[0] = reg;
    spi_data[1] = data;
    RE46C42X_cs_low();
    RE46C42X_delay_ms(1);
    RE46C42X_spi_Write(spi_data, 2);
    RE46C42X_cs_high();
}

void RE46C42X_write_reg_no_cs(uint8_t reg, uint8_t data)
{
    uint8_t spi_data[2];
    spi_data[0] = reg;
    spi_data[1] = data;
    RE46C42X_spi_Write(spi_data, 2);
}

bool RE46C42X_init()
{
#ifndef MIKROE_RE46C42X
    return false;
#endif
    
    RE46C42X_cs_low();
    RE46C42X_delay_us(100);
     //A bit of house cleaning to get part up - 3.0V Vbat operation
    //Vreg=on, Vdd=Vbat, CO=off
    RE46C42X_write_reg_no_cs(RE46C42X_REG4_REG_CO_CONTROL, 0x05);
    
    //Battery switch on, regulator switch on
    //regulator switch disables BG startup - auto boost @10ms
    RE46C42X_write_reg_no_cs(RE46C42X_REG4_REG_CO_CONTROL, 0x07);
        
    //Battery switch on, regulator switch off
    RE46C42X_write_reg_no_cs(RE46C42X_REG4_REG_CO_CONTROL, 0x04);
        
    //Reinforce Register values - write all registers
    
    //Set SPI to RdNow high - SPI read output immediately available
    //turn off watch_dog timer - Otherwise there will be a chirp every 20sec
    //unless there is SPI communications, leave Boost at default of 5.2V
    RE46C42X_write_reg_no_cs(RE46C42X_REG5_BOOST_IRCAP_SPI_WD_SETUP, 0x11); 
    
    // ABuf=off
    RE46C42X_write_reg_no_cs(RE46C42X_REG0_ABUF_CONTROL, 0x00);
    
    //Photo=off, reset
    RE46C42X_write_reg_no_cs(RE46C42X_REG2_PHOTO_AMP_CONTROL, 0x02);
    
    //Horn,IO,RLED=off
    RE46C42X_write_reg_no_cs(RE46C42X_REG3_IO_HORN_CO_CONTROL, 0x00);
    
    //Photo:Gain=1,timing=internal,Tint=100us,ABuf=Vp2
    RE46C42X_write_reg_no_cs(RE46C42X_REG6_PHOTO_AMP_SETUP, 0x00);
    
    //IRED1 = 100mA, IRED2 = 80mA
    RE46C42X_write_reg_no_cs(RE46C42X_REG7_LED_SETUP, 0x45);
    
    //Horn= normal (no:direct_drive,Enable,bridge)
    RE46C42X_write_reg_no_cs(RE46C42X_REG8_IO_SETUP, 0x00);
    
    // Initialization of the IRCAP
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x01);           //Start the low boost regulator and allow for soft start
    RE46C42X_delay_ms(5);                  //soft start time 4ms type, plus 25% for margin
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x05);           //Start the IRCAP charge
    RE46C42X_delay_ms(20);
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x00);           //stop charging IRCAPP
    RE46C42X_cs_high();
    
    // check if module is connected to the bus
    if(RE46C42X_read_reg(RE46C42X_REG4_REG_CO_CONTROL) == 0)
    {
        return false;
    }
    RE46C42X_connected = true;
    
    ADC1_ENABLE(); // Enable ADC as we will need it for measurements
    
    // Save photo amp adc value when no smoke is detected
    // This presumes when we start the board, no smoke will be around.
    RE46C42X_save_pa1_pa2_no_smoke_val();
    return true;
}

static uint16_t measureAndSentADCReading ()
{
    if(RE46C42X_connected)
    {
        return RE46C42X_adc_read();
    }
    return 0;
}

/*
 * Function perform a integration on PA1
 * Parameters:
 *      - chosen pa (e.g. PA1 or PA2)
 *      - integration duration
 */
static uint16_t PA_Integration(uint8_t chosen_pa, uint32_t duration)
{
    uint16_t value;
    uint32_t delay_value;
    uint8_t conf_reset, conf_enable;
    
    (void)delay_value; //needed to avoid compiler warning in cases where SmokeDetector sensor is not defined
    
    switch (chosen_pa)
    {
        case PA1:
            conf_reset = 0x01;
            conf_enable = 0x0B;
            break;
        case PA2:
            conf_reset = 0x11;
            conf_enable = 0x13;
            break;
        default:
            conf_reset = 0x0;
            conf_enable = 0x0;
            break;
    }
    
    switch (duration)
    {
        case 100:
            delay_value = 125;
            break;
        case 200:
            delay_value = 250;
            break;
        case 300:
            delay_value = 375;
            break;
        case 400:
            delay_value = 500;
            break;
        default:
            delay_value = 125;
            break;
    }

    RE46C42X_enable_high();               //start dark integration utilizing Enable Pin
    RE46C42X_delay_us(5);              
    RE46C42X_enable_low();
    RE46C42X_delay_us(delay_value);                //allow enough time for the integration with some slack
    value = measureAndSentADCReading();    //read the integration value and send out
    RE46C42X_write_reg_no_cs(RE46C42X_REG2_PHOTO_AMP_CONTROL, conf_reset);          //reset integration capacitors - RESET State
    RE46C42X_delay_us(10);                 //second integration will be a Lit - LEDXEn is high
    RE46C42X_write_reg_no_cs(RE46C42X_REG2_PHOTO_AMP_CONTROL, conf_enable);          //enable the photo-amp, and set the Status bit to 1, enable LEDX - READY State
    RE46C42X_delay_us(10);                
    RE46C42X_enable_high();               //start Lit integration utilizing Enable Pin
    RE46C42X_delay_us(5);
    RE46C42X_enable_low();
    RE46C42X_delay_us(delay_value);
    value = measureAndSentADCReading() - value;    //read the integration value and send out
    RE46C42X_delay_ms(1);
    RE46C42X_write_reg_no_cs(RE46C42X_REG2_PHOTO_AMP_CONTROL, 0x00);          // disable the photo-amp, will put into RESET State
    RE46C42X_delay_ms(1);                  //allow time to switch back to CO for ABUF 
    measureAndSentADCReading();    //take CO reading -WAK 08102018
    
    return value;
}

void RE46C42X_set_pa1_conf(uint8_t reg6_pa1_conf)
{
    pa1_conf = reg6_pa1_conf;
    RE46C42X_save_pa1_pa2_no_smoke_val();
}

void RE46C42X_set_pa2_conf(uint8_t reg6_pa2_conf)
{
    pa2_conf = reg6_pa2_conf;
    RE46C42X_save_pa1_pa2_no_smoke_val();
}

bool RE46C42X_is_smoke_detected()
{
    if(!RE46C42X_connected) return false;
    
    uint8_t integration_time;
    uint16_t pa1_val = 0, pa2_val = 0;

    RE46C42X_cs_low();
    RE46C42X_delay_us(100);
    /*Set up and run PA1*/
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x01);           //Start the low boost regulator and allow for soft start
    RE46C42X_delay_ms(4);                  //soft start time 4ms type
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x05);           //Start the IRCAP charge
    RE46C42X_delay_ms(2);
    RE46C42X_write_reg_no_cs(RE46C42X_REG2_PHOTO_AMP_CONTROL, 0x03);          //enable the photo-amp, and set the Status bit to 1 - READY State
    RE46C42X_delay_us(110);                //wait for internal photo amp to stabilize
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x00);           //stop charging IRCAP
        
    RE46C42X_write_reg_no_cs(RE46C42X_REG6_PHOTO_AMP_SETUP, pa1_conf);   //similar to method Photo_Integrate write setup data to reg 6
    integration_time = pa1_conf>>4;    //shift the register 6 bits so just the integration time
                                    //bits are left, put them into var. for decision making below

    if (integration_time == 8)            //determine which integration time method to use 
        pa1_val = PA_Integration(PA1, 100); //PA1_100us_Integration();
    else if (integration_time == 9)
        pa1_val = PA_Integration(PA1, 200); //PA1_200us_Integration();
    else if (integration_time == 0xA)
        pa1_val = PA_Integration(PA1, 300); //PA1_300us_Integration();
    else if (integration_time == 0xB)
        pa1_val = PA_Integration(PA1, 400); //PA1_400us_Integration();

    RE46C42X_delay_ms(100);

    /*Set up and run PA2*/
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x01);           //Start the low boost regulator and allow for soft start
    RE46C42X_delay_ms(4);                  //soft start time 4ms typ
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x05);           //Start the IRCAP charge
    RE46C42X_delay_ms(2);
    RE46C42X_write_reg_no_cs(RE46C42X_REG2_PHOTO_AMP_CONTROL, 0x03);          //enable the photo-amp, and set the Status bit to 1 - READY State
    RE46C42X_delay_us(110);                //wait for internal photo amp to stabilize
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x00);           //stop charging IRCAP
        
    RE46C42X_write_reg_no_cs(RE46C42X_REG6_PHOTO_AMP_SETUP, pa2_conf);
    integration_time = pa2_conf>>4;

    if (integration_time == 8)
        pa2_val = PA_Integration(PA2, 100); //PA2_100us_Integration();
    else if (integration_time == 9)
        pa2_val = PA_Integration(PA2, 200); //PA2_200us_Integration();
    else if (integration_time == 0xA)
        pa2_val = PA_Integration(PA2, 300); //PA2_300us_Integration();
    else if (integration_time == 0xB)
        pa2_val = PA_Integration(PA2, 400); //PA2_400us_Integration();

    RE46C42X_cs_high();
    
    //what should we do if calibration values are smaller than actual read values?
    //for now we save the smaller values as the non-smoke correct values
    if (pa1_val < pa1_normal_val)
        pa1_normal_val = pa1_val;
    if (pa2_val < pa2_normal_val)
        pa2_normal_val = pa2_val;

    if(RE46C42X_debug)
    {
        sprintf(message, "\nRE46C42X pa1: %d, pa1_normal: %d, threshold1: %d\n", pa1_val, pa1_normal_val, PA1_SMOKE_THRESHOLD);
        RE46C42X_print_msg();
        sprintf(message, "RE46C42X pa2: %d, pa2_normal: %d, threshold2: %d\n", pa2_val, pa2_normal_val, PA2_SMOKE_THRESHOLD);
        RE46C42X_print_msg();
    }

    if ((pa1_val - pa1_normal_val > PA1_SMOKE_THRESHOLD) || (pa2_val - pa2_normal_val > PA2_SMOKE_THRESHOLD))
    {
        if(RE46C42X_debug)
        {
            sprintf(message, "RE46C42X SMOKE DETECTED thr1: %d, thr2: %d\n\n", 
                    pa1_val - pa1_normal_val, pa2_val - pa2_normal_val);
            RE46C42X_print_msg();
        }
        return true;
    }

    return false;
}

static void RE46C42X_save_pa1_pa2_no_smoke_val()
{
    uint8_t integration_time;
    
    RE46C42X_cs_low();
    RE46C42X_delay_us(100);
    
    /*Set up and run PA1*/
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x01);           //Start the low boost regulator and allow for soft start
    RE46C42X_delay_ms(4);                  //soft start time 4ms type
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x05);           //Start the IRCAP charge
    RE46C42X_delay_ms(2);
    RE46C42X_write_reg_no_cs(RE46C42X_REG2_PHOTO_AMP_CONTROL, 0x03);       //enable the photo-amp, and set the Status bit to 1 - READY State
    RE46C42X_delay_us(110);                //wait for internal photo amp to stabilize
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x00);           //stop charging IRCAP
        
    RE46C42X_write_reg_no_cs(RE46C42X_REG6_PHOTO_AMP_SETUP, pa1_conf);   //similar to method Photo_Integrate write setup data to reg 6
    integration_time = pa1_conf>>4;    //shift the register 6 bits so just the integration time
                                    //bits are left, put them into var. for decision making below
    if (integration_time == 8)            //determine which integration time method to use 
        pa1_normal_val = PA_Integration(PA1, 100); //PA1_100us_Integration();
    else if (integration_time == 9)
        pa1_normal_val = PA_Integration(PA1, 200); //PA1_200us_Integration();
    else if (integration_time == 0xA)
        pa1_normal_val = PA_Integration(PA1, 300); //PA1_300us_Integration();
    else if (integration_time == 0xB)
        pa1_normal_val = PA_Integration(PA1, 400); //PA1_400us_Integration();

    RE46C42X_delay_ms(100);
        
    /*Set up and run PA2*/
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x01);           //Start the low boost regulator and allow for soft start
    RE46C42X_delay_ms(4);                  //soft start time 4ms type
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x05);           //Start the IRCAP charge
    RE46C42X_delay_ms(2);
    RE46C42X_write_reg_no_cs(RE46C42X_REG2_PHOTO_AMP_CONTROL, 0x03);       //enable the photo-amp, and set the Status bit to 1 - READY State
    RE46C42X_delay_us(110);                //wait for internal photo amp to stabilize
    RE46C42X_write_reg_no_cs(RE46C42X_REG1_BOOST_CONTROL, 0x00);           //stop charging IRCAP
        
    RE46C42X_write_reg_no_cs(RE46C42X_REG6_PHOTO_AMP_SETUP, pa2_conf);
    integration_time = pa2_conf>>4;
    if (integration_time == 8)
        pa2_normal_val = PA_Integration(PA2, 100); //PA2_100us_Integration();
    else if (integration_time == 9)
        pa2_normal_val = PA_Integration(PA2, 200); //PA2_200us_Integration();
    else if (integration_time == 0xA)
        pa2_normal_val = PA_Integration(PA2, 300); //PA2_300us_Integration();
    else if (integration_time == 0xB)
        pa2_normal_val = PA_Integration(PA2, 400); //PA2_400us_Integration();
        
    RE46C42X_cs_high();
}

void RE46C42X_rled_on()
{
    uint8_t rled_on;
    uint8_t reg_value = RE46C42X_read_reg(RE46C42X_REG3_IO_HORN_CO_CONTROL); // will be used to not lose already written reg values
    
    rled_on = reg_value | 0x08;

    RE46C42X_write_reg(RE46C42X_REG3_IO_HORN_CO_CONTROL, rled_on);
}

void RE46C42X_rled_off()
{
    uint8_t rled_off;
    uint8_t reg_value = RE46C42X_read_reg(RE46C42X_REG3_IO_HORN_CO_CONTROL); // will be used to not lose already written reg values

    rled_off = reg_value & 0xF7;
    
    RE46C42X_write_reg(RE46C42X_REG3_IO_HORN_CO_CONTROL, rled_off);
}

void RE46C42X_horn_on()
{
    uint8_t horn_on;
    uint8_t reg_value = RE46C42X_read_reg(RE46C42X_REG3_IO_HORN_CO_CONTROL); // will be used to not lose already written reg values
    
    horn_on = reg_value | 0x01;
    
    RE46C42X_write_reg(RE46C42X_REG3_IO_HORN_CO_CONTROL, horn_on);
}

void RE46C42X_horn_off()
{
    uint8_t horn_off;
    uint8_t reg_value = RE46C42X_read_reg(RE46C42X_REG3_IO_HORN_CO_CONTROL); // will be used to not lose already written reg values
    
    horn_off = reg_value & 0xFE;
    
    RE46C42X_write_reg(RE46C42X_REG3_IO_HORN_CO_CONTROL, horn_off);
}

void RE46C42X_set_led1_current(uint32_t current)
{
    uint8_t led1_current;
    uint8_t reg_value = RE46C42X_read_reg(RE46C42X_REG7_LED_SETUP); // will be used to not lose already written reg values

    led1_current = reg_value & ~0x07;
    switch (current)
    {
        case 20:
            led1_current |= 0x00;
            break;
        case 40:
            led1_current |= 0x01;
            break;
        case 50:
            led1_current |= 0x02;
            break;
        case 60:
            led1_current |= 0x03;
            break;
        case 80:
            led1_current |= 0x04;
            break;
        case 100:
            led1_current |= 0x05;
            break;
        case 150:
            led1_current |= 0x06;
            break;
        case 200:
            led1_current |= 0x07;
            break;                    
    }

    RE46C42X_write_reg(RE46C42X_REG7_LED_SETUP, led1_current);
}

void RE46C42X_set_led2_current(uint32_t current)
{
    uint8_t led2_current = 0x00;
    uint8_t reg_value = RE46C42X_read_reg(RE46C42X_REG7_LED_SETUP); // will be used to not lose already written reg values

    led2_current = reg_value & ~0x70;
    switch (current)
    {
        case 20:
            led2_current |= 0x00;
            break;
        case 40:
            led2_current |= 0x10;
            break;
        case 50:
            led2_current |= 0x20;
            break;
        case 60:
            led2_current |= 0x30;
            break;
        case 80:
            led2_current |= 0x40;
            break;
        case 100:
            led2_current |= 0x50;
            break;
        case 150:;
            led2_current |= 0x60;
            break;
        case 200:;
            led2_current |= 0x70;
            break;                    
    }
        
    RE46C42X_write_reg(RE46C42X_REG7_LED_SETUP, led2_current);
}

uint8_t RE46C42X_read_reg(uint8_t reg)
{
    uint8_t reg_to_read[3];
    
    reg_to_read[0] = reg | (1<<7); //append the Read bit
    
    RE46C42X_cs_low();
    RE46C42X_delay_ms(1);
    RE46C42X_spi_Read(&reg_to_read[0], &reg_to_read[1]);
    RE46C42X_cs_high();

    return reg_to_read[2];
}

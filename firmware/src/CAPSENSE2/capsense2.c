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
#include "capsense2.h"
#include "capsense2_i2c_hal.h"

struct capsense2_dev dev;
static bool connected = false;

typedef struct 
{
    uint8_t s1_pressed  : 1;
    uint8_t s1_released : 1;
    uint8_t s2_pressed  : 1;
    uint8_t s2_released : 1;
    uint8_t slider_up   : 1;
    uint8_t slider_down : 1;
    uint8_t reserved    : 2;
    uint8_t slider_val;
} _capsense_bits;

typedef union
{
    _capsense_bits bits;
    uint8_t status[2];
} _capsense_buttons;

_capsense_buttons buttons;

static uint8_t button_press_state = 0;

/*!
 *  @brief This API is the entry point.
 *  It reads the chip-id from the sensor.
 */
int8_t CAPSENSE2_init()
{
    if(!CAPSENSE2_AVAILABLE) return CAPSENSE2_E_DEV_NOT_FOUND;
    
    uint8_t rslt = 0;
	/* chip id read try count */
	uint8_t try_count = 5;
	uint8_t vendor_id   = 0;

    CAPSENSE2_reset_high();
    CAPSENSE2_delay_ms(20);
    CAPSENSE2_reset_low();
    CAPSENSE2_delay_ms(100);
    
	dev.dev_id   = CAPSENSE2_DEV_ADDR;
    
    while (try_count) {
        /* Read the Vendor ID of CAPSENSE2 sensor */
        rslt = capsense2_get_reg(CAPSENSE2_VENDOR_ID_ADDR, &vendor_id, 1);
        /* Check for chip id validity */
        if ((rslt == CAPSENSE2_OK) && (vendor_id == CAPSENSE2_VENDOR_ID)) {
            dev.vendor_id = vendor_id;
            break;
        }
        /* Wait for 1 ms */
        CAPSENSE2_delay_ms(1);
        --try_count;
    }
    /* Chip id check failed */
    if (!try_count)
    {
        rslt = CAPSENSE2_E_DEV_NOT_FOUND;
    }
    
    connected = (rslt == CAPSENSE2_OK);
	return rslt;
}

bool CAPSENSE2_available(void)
{
    return connected;
}

/*!
 * @brief This API reads the data from the given register address of the sensor.
 */
int8_t capsense2_get_reg(uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	int8_t rslt;

    /* Read the data  */
    rslt = capsense2_i2c_read(dev.dev_id, reg_addr, reg_data, len);
    /* Check for communication error */
    if (rslt != CAPSENSE2_OK)
    {
        rslt = CAPSENSE2_E_COMM_FAIL;
    }

	return rslt;
}

/*!
 * @brief This API reads the interrupt pin of the sensor.
 */
int8_t capsense2_check_alert_pin()
{
	return CAPSENSE2_get_alert();
}

/*!
 * @brief This API clears the interrupt pin of the sensor.
 */
int8_t capsense2_clear_interrupt()
{
	int8_t rslt;
    uint8_t main_control = 0;

    /* Read the data  */
    rslt = capsense2_i2c_read(dev.dev_id, CAPSENSE2_REG_MAIN_STATUS_CONTROL, &main_control, 1);
    /* Check for communication error */
    if (rslt != CAPSENSE2_OK)
    {
        return CAPSENSE2_E_COMM_FAIL;
    }
    main_control &= ~CAPSENSE2_MAIN_STATUS_CONTROL_INT;
    rslt = capsense2_i2c_write (dev.dev_id, CAPSENSE2_REG_MAIN_STATUS_CONTROL, &main_control, 1);

	return rslt;
}

int8_t capsense2_read_reg_btn_status_1(uint8_t *button_status)
{
    int8_t rslt;

    /* Read the data  */
    rslt = capsense2_i2c_read(dev.dev_id, CAPSENSE2_REG_BUTTON_STATUS_1, button_status, 1);
    
	return rslt;
}

void CAPSENSE2_execute(void)
{
    uint8_t slider = 0;
    uint8_t btn_status = 0;    
    
    if (CAPSENSE2_OK == capsense2_read_reg_btn_status_1(&btn_status))
    {
        if (btn_status & CAPSENSE2_BUTTON_STATUS_1_BUTTON_1)
        {
            if (!(button_press_state & CAPSENSE2_BUTTON_STATUS_1_BUTTON_1))
            {
                buttons.bits.s1_pressed = 1;
                button_press_state |= CAPSENSE2_BUTTON_STATUS_1_BUTTON_1;
            }
        }
        if (btn_status & CAPSENSE2_BUTTON_STATUS_1_BUTTON_2)
        {
            if (!(button_press_state & CAPSENSE2_BUTTON_STATUS_1_BUTTON_2))
            {
                buttons.bits.s2_pressed = 1;
                button_press_state |= CAPSENSE2_BUTTON_STATUS_1_BUTTON_2;
            }
        }
        if (btn_status & CAPSENSE2_BUTTON_STATUS_1_UP_SLIDER)
        {
            if (CAPSENSE2_BUTTON_STATUS_1_UP_SLIDER != button_press_state)
            {
                buttons.bits.slider_up = 1;
                button_press_state = CAPSENSE2_BUTTON_STATUS_1_UP_SLIDER;
            }
        }
        if (btn_status & CAPSENSE2_BUTTON_STATUS_1_DOWN_SLIDER)
        {
            if (CAPSENSE2_BUTTON_STATUS_1_DOWN_SLIDER != button_press_state)
            {
                buttons.bits.slider_down = 1;
                button_press_state = CAPSENSE2_BUTTON_STATUS_1_DOWN_SLIDER;
            }
        }
    }
    capsense2_clear_interrupt();
    // check if buttons are released
    if (CAPSENSE2_OK == capsense2_read_reg_btn_status_1(&btn_status))
    {
        if ((button_press_state & CAPSENSE2_BUTTON_STATUS_1_BUTTON_1) &&
            !(btn_status & CAPSENSE2_BUTTON_STATUS_1_BUTTON_1))
        {
            buttons.bits.s1_released = 1;
            button_press_state &= ~CAPSENSE2_BUTTON_STATUS_1_BUTTON_1;
        }
        if ((button_press_state & CAPSENSE2_BUTTON_STATUS_1_BUTTON_2) &&
            !( btn_status & CAPSENSE2_BUTTON_STATUS_1_BUTTON_2 ) )
        {
            buttons.bits.s2_released = 1;
            button_press_state &= ~CAPSENSE2_BUTTON_STATUS_1_BUTTON_2;
        }
    }
    // check if swipe event is finished and display the slider position
    if (CAPSENSE2_OK == capsense2_get_reg (CAPSENSE2_REG_SLIDER_POSITION_DATA, &slider, 1))
    {
        if (slider)
        {
            if (button_press_state & CAPSENSE2_BUTTON_STATUS_1_UP_SLIDER)
            {
                buttons.bits.slider_up = 1;
                buttons.bits.slider_val = slider;
            }
            else if (button_press_state & CAPSENSE2_BUTTON_STATUS_1_DOWN_SLIDER)
            {
                buttons.bits.slider_down = 1;
                buttons.bits.slider_val = slider;
            }
        }
        else
        {
            button_press_state &= ~CAPSENSE2_BUTTON_STATUS_1_UP_SLIDER;
            button_press_state &= ~CAPSENSE2_BUTTON_STATUS_1_DOWN_SLIDER;
        }
    }
}

bool CAPSENSE2_action_required(void)
{
    return (buttons.status[0] != 0);
}

CAPSENSE2_ACTIONS CAPSENSE2_get_s1_action(void)
{
    if(buttons.bits.s1_pressed)
    {
        buttons.bits.s1_pressed = 0;
        return CAPSENSE2_PRESS;
    }
    else if(buttons.bits.s1_released)
    {
        buttons.bits.s1_released = 0;
        return CAPSENSE2_RELEASE;
    }
    else
        return CAPSENSE2_NONE;
}

CAPSENSE2_ACTIONS CAPSENSE2_get_s2_action(void)
{
    if(buttons.bits.s2_pressed)
    {
        buttons.bits.s2_pressed = 0;
        return CAPSENSE2_PRESS;
    }
    else if(buttons.bits.s2_released)
    {
        buttons.bits.s2_released = 0;
        return CAPSENSE2_RELEASE;
    }
    else
        return CAPSENSE2_NONE;

}

CAPSENSE2_ACTIONS CAPSENSE2_get_slider_action(void)
{
    if(buttons.bits.slider_up)
    {
        buttons.bits.slider_up = 0;
        return CAPSENSE2_SLIDER_UP;
    }
    else if(buttons.bits.slider_down)
    {
        buttons.bits.slider_down = 0;
        return CAPSENSE2_SLIDER_DOWN;
    }
    else
        return CAPSENSE2_NONE;
}

uint8_t CAPSENSE2_get_slider_value(void)
{
    return buttons.bits.slider_val;
}

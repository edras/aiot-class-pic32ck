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
#include "fan.h"
#include "fan_i2c_hal.h"

fan_cfg_t fan_cfg;

struct fan_dev_t {
	/*! Vendor Id */
	uint8_t vendor_id;
	/*! Device Id */
	uint8_t dev_id;
};
struct fan_dev_t fan_dev;
static bool fan_connected = false;

/*!
 *  @brief This API is the entry point.
 *  It reads the chip-id from the sensor.
 */
int8_t fan_init(void)
{

    uint8_t rslt = 0;
    //uint8_t duty = 0;
	/* chip id read try count */
	uint8_t try_count = 5;
	uint8_t product_id = 0;

   	fan_dev.dev_id = FAN_DEVICE_ADDR;
    
    while (try_count) {
        /* Read the Vendor ID of CAPSENSE2 sensor */
        rslt = fan_get_reg(FAN_PRODUCT_ID, &product_id, 1);
        /* Check for chip id validity */
        if ((rslt == FAN_E_OK) && (product_id == FAN_PRODUCT_ID_VAL)) 
        {
            rslt = fan_get_reg(FAN_MANUFACTURER_ID, &product_id, 1);
            if ((rslt == FAN_E_OK) && (product_id == FAN_MANUFACTURER_ID_VAL)) 
            {
                fan_dev.vendor_id = product_id;
                break;
            }
        }
        /* Wait for 1 ms */
        FAN_DelayMs(1);
        --try_count;
    }
    /* Chip id check failed */
    if (!try_count)
    {
        rslt = FAN_E_DEV_NOT_FOUND;
        return rslt;
    }

    fan_set_frequency(&fan_cfg, 32768);
    fan_set_default_setting(&fan_cfg);
    fan_set_mode (&fan_cfg, FAN_DIRECT_MODE);
    fan_set_sw_lock (&fan_cfg, 0);
    fan_set_edges (&fan_cfg, FAN_9_EDGES);
    fan_set_range (&fan_cfg, FAN_500_RPM);      
    fan_set_target_tach(&fan_cfg, 480);
    
    // turn fan off at initialization
    fan_set_duty_cycle_percentage(&fan_cfg, 0);
    fan_connected = (rslt == FAN_E_OK);
	return rslt;
}

/*!
 * @brief This API reads the data from the given register address of the sensor.
 */
int8_t fan_get_reg(uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	int8_t ret = FAN_E_OK;

    /* Read the data  */
    ret = fan_i2c_read(fan_dev.dev_id, reg_addr, reg_data, len);

	return ret;
}


/* The Configuration register controls the basic functionality of the EMC2301/2/3/5. 
 * RegAddress 0x20h
 * bit7 = MASK: Blocks the ALERT pin from being asserted
 * bit6 = DIS_TO: Disables the SMBus Time-Out function for the SMBus client (if enabled)
 * bit5 = WD_EN: Enables the Watchdog Timer to operate in Continuous Mode
 * bit1 = DRECK: Enables the internal tachometer clock to be driven out on the CLK pin so that multiple devices can be synced to the same source
 * bit0 = USECK: Enables the EMC2301/2/3/5 to use a clock present on the CLK pin as the tachometer clock.
 *               If the DR_EXT_CLK bit is set, then this bit is ignored and the device will use the internal oscillator.
 * SWLock protected.
 */
int8_t fan_device_cfg(fan_cfg_t *fan_cfg, uint8_t config_reg)
{
    int8_t ret = FAN_E_OK;
    
    /* The Configuration Register is software locked. */
    if (fan_cfg->fan_setup_cfg.lock_cfg)
        return FAN_E_INVALID;

    ret = fan_i2c_write(fan_dev.dev_id, FAN_CONFIGURATION, &config_reg, 1);

	return ret;
}

/* The Fan Status register indicates that the fan driver has stalled or failed or that the Watchdog Timer has expired.
 * RegAddress 0x24h
 * bit0 = FNSTL: Indicates that one or more fan drivers have stalled
 * bit1 = FNSPIN: Indicates that one or more fan drivers cannot spin up.
 * bit2 = DVFAIL: Indicates that one or more fan drivers cannot meet the programmed fan speed at maximum PWM duty cycle.
 * bit7 = WATCH: Indicates that the Watchdog Timer has expired.
 */
int8_t fan_get_status (fan_cfg_t *fan_cfg, uint8_t *status)
{
    int8_t ret = FAN_E_OK;
    
    /* Read the data  */
    ret = fan_i2c_read(fan_dev.dev_id, FAN_STATUS, status, 1);

	return ret;
}

/* The Fan Stall Status register indicates which fan driver has detected a stalled condition.
 * RegAddress 0x25h
 * bit0=Fan 1 Stall Status .. bit4=Fan 5 Stall Status
 * 1 = Fan x tachometer count has exceeded maximum valid TACH count, indicating stall.
 * 0 = Fan x Stall has not been detected.
 */
int8_t fan_get_stall_status (fan_cfg_t *fan_cfg, uint8_t *status)
{
    int8_t ret = FAN_E_OK;
    
    /* Read the data  */
    ret = fan_i2c_read(fan_dev.dev_id, FAN_STALL_STATUS, status, 1);

	return ret;
}

/* The Fan Spin Status register indicates which fan driver has failed to spin-up 
 * RegAddress 0x26h
 * bit0=Fan 1 Spin-Up Status .. bit4=Fan 5 Spin-Up Status
 * 1 = Fan x spin-up routine failed to start the fan.
 * 0 = Fan x spin-up routine successful.
 */
int8_t fan_get_spin_status (fan_cfg_t *fan_cfg, uint8_t *status)
{
    int8_t ret = FAN_E_OK;
    
    /* Read the data  */
    ret = fan_i2c_read(fan_dev.dev_id, FAN_SPIN_STATUS, status, 1);

	return ret;
}

/* The Fan Drive Fail Status register indicates which fan driver cannot drive to the programmed speed even at 100% duty cycle 
 * RegAddress 0x27h
 * bit0=Drive Fail Fan 1 Status .. bit4=Drive Fail Fan 5 Status
 * 1 = Fan x is unable to reach the commanded RPM with 100% PWM input.
 * 0 = Fan x is within limits of commanded RPM.
 */
int8_t fan_get_drive_fail_status (fan_cfg_t *fan_cfg, uint8_t *status)
{
    int8_t ret = FAN_E_OK;
    
    /* Read the data  */
    ret = fan_i2c_read(fan_dev.dev_id, FAN_DRIVE_FAIL_STATUS, status, 1);

	return ret;
}

/* The Fan Interrupt Enable register controls the masking for each fan channel. 
 * When a channel is enabled, it will cause the ALERT pin to be asserted when an Error condition is detected.
 * RegAddress 0x29h
 * ?1? - An Error condition (Stall, Spin Up, Drive Fail) on fan X will cause the ALERT pin to be asserted.
 * ?0? (default) - An Error condition on fan X will not cause the ALERT pin to be asserted; however, the status registers will be updated normally.
 * bit0 of "enable" input parameter F1ITEN
 * bit4 of "enable" input parameter F5ITEN
 */
int8_t fan_int_en (fan_cfg_t *fan_cfg, uint8_t enable)
{
    int8_t ret = FAN_E_OK;
    
    ret = fan_i2c_write(fan_dev.dev_id, FAN_INTERRUPT_ENABLE, &enable, 1);

	return ret;
}

/* The PWM Polarity Configuration registers control the output type and polarity of all PWM outputs.
 * RegAddress 0x2Ah
 * bit4 = PLRITY5: Sets the PWM output polarity
 * bit0 = PLRITY0: Sets the PWM output polarity
 * 1 = PWM 5 drive setting of 00h produces 100% duty cycle, drive setting of FFh produces 0% duty cycle.
 * 0 = PWM 5 drive setting of 00h produces 0% duty cycle, drive setting of FFh produces 100% duty cycle.
 */
int8_t fan_set_pwm_polarity (fan_cfg_t *fan_cfg, uint8_t polarity)
{
    int8_t ret = FAN_E_OK;
    
    ret = fan_i2c_write(fan_dev.dev_id, FAN_PWM_POLARITY, &polarity, 1);

	return ret;
}

/* The PWM Output Configuration register controls the PWM output type as push-pull or open drain.
 * RegAddress 0x2Bh 
 * bit4 = PMOT5: Sets the PWM output type
 * bit0 = PMOT1: Sets the PWM output type
 * 1 = PWMx drive is push-pull type.
 * 0 = PWMx drive is open drain type.
 */
int8_t fan_set_pwm_drive_output (fan_cfg_t *fan_cfg, uint8_t mode)
{
    int8_t ret = FAN_E_OK;

    ret = fan_i2c_write(fan_dev.dev_id, FAN_PWM_OUTPUT_CONFIG, &mode, 1);

	return ret;
}

/* Set the PWM BaseF123 Register or PWM BaseF45 Register
 * The PWM BASEF45 Register controls the base frequency of PWM drivers 4 and 5.
 * RegAddress 0x2Ch
 * The PWM BaseF123 Register controls the base frequency of PWM drivers 1, 2 and 3.
 * RegAddress 0x2Dh
 * PMBx[1:0]: Sets the PWMx base frequency
 * 11 = PWMx base is 2.441 kHz.
 * 10 = PWMx base is 4.882 kHz.
 * 01 = PWMx base is 19.53 kHz.
 * 00 = PWMx base is 26.00 kHz.
 */
int8_t fan_set_pwm_base(fan_cfg_t *fan_cfg, fan_pwm_base_t pwm_base, uint8_t pwm_id)
{
    uint8_t reg = 0;
    int8_t ret = FAN_E_OK;

    switch (pwm_id)
    {
        case 1:
        case 2:
        case 3:
            ret = fan_i2c_read(fan_dev.dev_id, FAN_PWM_BASE123_FREQ, &reg, 1);
            reg &= ~(pwm_base << (2 * (pwm_id - 1)));
            reg |= (pwm_base << (2 * (pwm_id - 1)));
            ret = fan_i2c_write(fan_dev.dev_id, FAN_PWM_BASE123_FREQ, &reg, 1);
            break;
        case 4:
        case 5:
            ret = fan_i2c_read(fan_dev.dev_id, FAN_PWM_BASE45_FREQ, &reg, 1);
            reg &= ~(pwm_base << (2 * (pwm_id - 4)));
            reg |= (pwm_base << (2 * (pwm_id - 4)));
            ret = fan_i2c_write(fan_dev.dev_id, FAN_PWM_BASE45_FREQ, &reg, 1);
            break;
        default:
            ret = FAN_E_INVALID;
            break;
    }

    return ret;
}

/* Direct Setting mode - in this mode of operation, the user directly controls the fan drive setting.
 * RegAddress 0x30h
 * Updating the Fan Driver Setting Register (see Register 6-11) will instantly update the PWM fan drive.
 * Fan Speed Control (FSC) mode - in this mode of operation, the user determines a target tachometer
 *  count and the PWM drive setting is automatically updated to achieve this target speed.
 * PWM duty cycle is calculated:
 * Drive = (VALUE/255)* 100%
 */
int8_t fan_set_duty_cycle_percentage (fan_cfg_t *fan_cfg, double percentage)
{
    uint8_t reg = 0;
    int8_t ret = FAN_E_OK;
    
    if (percentage > 100)
        percentage = 100;
    if (percentage < 0)
        percentage = 0;

    reg = percentage / 100 * 255;
    
    /* Changes to Fan Setting register are ignored in this mode. */
    if (fan_cfg->fan_setup_cfg.mode_cfg == FAN_FSC_MODE)
        return FAN_E_INVALID;
    
    ret = fan_i2c_write(fan_dev.dev_id, FAN_SETTING, &reg, 1);

	return ret;
}
#define FAN_DEF_CFG          0x80
int8_t fan_set_default_setting (fan_cfg_t *fan_cfg)
{
    int8_t ret = FAN_E_OK;
    uint8_t reg = FAN_DEF_CFG;
    
    ret = fan_i2c_write(fan_dev.dev_id, FAN_SETTING, &reg, 1);

	return ret;
}

/* The Fan Configuration registers control the general operation of the RPM-based Fan Speed Control algorithm used for the respective Fan Driver 
 * bit7 = ENAGx: Enable Closed Loop algorithm, where ?x? is 1 to 5, depending on device features
 * 1 = Closed Loop algorithm ?x? is enabled. Changes to Fan Setting register are ignored.
 * 0 = Closed Loop algorithm ?x? is disabled, and device is placed in Direct Setting mode
 */
int8_t fan_set_mode (fan_cfg_t *fan_cfg, fan_mode_t fan_mode)
{
    uint8_t reg = 0;
    uint8_t ret = FAN_E_OK;

    ret = fan_i2c_read(fan_dev.dev_id, FAN_CONFIG1, &reg, 1);
    if (ret != FAN_E_OK)
        return ret;

    switch (fan_mode)
    {
        case FAN_DIRECT_MODE:
            /* clear bit7 ENAG */
            reg &= ~FAN_CL_ALG_EN;
            fan_cfg->fan_setup_cfg.mode_cfg = FAN_DIRECT_MODE;
            break;

        case FAN_FSC_MODE:
            /* set bit7 ENAG */
            reg |= FAN_CL_ALG_EN;
            fan_cfg->fan_setup_cfg.mode_cfg = FAN_FSC_MODE;
            break;

        default:
            break;
    }
    ret = fan_i2c_write(fan_dev.dev_id, FAN_CONFIG1, &reg, 1);
    return ret;
}

/* The Fan Configuration registers control the general operation of the RPM-based Fan Speed Control algorithm used for the respective Fan Driver. 
 * bit6-5 = RNGx[1:0]: Range. Sets the minimum fan speed measured and reported, where ?x? is 1 to 5, depending on device features.
 * 11 = 4000 RPM minimum, TACH count multiplier = 8.
 * 10 = 2000 RPM minimum, TACH count multiplier = 4.
 * 01 = 1000 RPM minimum, TACH count multiplier = 2.
 * 00 = 500 RPM minimum, TACH count multiplier = 1.
 */
int8_t fan_set_range (fan_cfg_t *fan_cfg, fan_range_rpm_t fan_range)
{
    uint8_t reg = 0;
    uint8_t ret = FAN_E_OK;

    ret = fan_i2c_read(fan_dev.dev_id, FAN_CONFIG1, &reg, 1);
    if (ret != FAN_E_OK)
        return ret;

    reg &= 0x9F;
    reg |= fan_range;
    if (fan_range == FAN_4000_RPM)
        fan_cfg->fan_setup_cfg.mul_cfg = 8;
    else if (fan_range == FAN_2000_RPM)
        fan_cfg->fan_setup_cfg.mul_cfg = 4;
    else if (fan_range == FAN_1000_RPM)
        fan_cfg->fan_setup_cfg.mul_cfg = 2;
    else if (fan_range == FAN_500_RPM)
        fan_cfg->fan_setup_cfg.mul_cfg = 1;
    else 
        return FAN_E_INVALID;

    ret = fan_i2c_write(fan_dev.dev_id, FAN_CONFIG1, &reg, 1);
    return ret;
}

/* The Fan Configuration registers control the general operation of the RPM-based Fan Speed Control algorithm used for the respective Fan Driver. 
 * bits4-3: EDGx[1:0]: Edges. Sets the number of edges to sample when calculating RPM, where ?x? is 1 to 5, depending on device features.
 * 11 = 9 edges sampled (4 poles) - effective Tach multiplier is 2, based on two pole fans
 * 10 = 7 edges sampled (3 poles) - effective Tach multiplier is 1.5, based on two pole fans
 * 01 = 5 edges sampled (2 poles) - effective Tach multiplier is 1, based on two pole fans
 * 00 = 3 edges sampled (1 pole) - effective Tach multiplier is 0.5, based on two pole fans
 */
int8_t fan_set_edges (fan_cfg_t *fan_cfg, fan_edges_t fan_edge)
{
    uint8_t reg = 0;
    uint8_t ret = FAN_E_OK;

    ret = fan_i2c_read(fan_dev.dev_id, FAN_CONFIG1, &reg, 1);
    if (ret != FAN_E_OK)
        return ret;

    reg &= 0xE7;
    reg |= fan_edge;
    if (fan_edge == FAN_9_EDGES)
    {
        fan_cfg->fan_setup_cfg.edges_cfg = 9;
        fan_cfg->fan_setup_cfg.poles_cfg = 4;
    }
    else if (fan_edge == FAN_7_EDGES)
    {
        fan_cfg->fan_setup_cfg.edges_cfg = 7;
        fan_cfg->fan_setup_cfg.poles_cfg = 3;
    }
    else if (fan_edge == FAN_5_EDGES)
    {
        fan_cfg->fan_setup_cfg.edges_cfg = 5;
        fan_cfg->fan_setup_cfg.poles_cfg = 2;
    }
    else if (fan_edge == FAN_3_EDGES)
    {
        fan_cfg->fan_setup_cfg.edges_cfg = 3;
        fan_cfg->fan_setup_cfg.poles_cfg = 1;
    }
    else 
    {  
        return FAN_E_INVALID;
    }

    ret = fan_i2c_write(fan_dev.dev_id, FAN_CONFIG1, &reg, 1);
    return ret;
}


/* The Fan Configuration registers control the general operation of the RPM-based Fan Speed Control algorithm used for the respective Fan Driver. 
 * bits2-0: UDTx[2:0]: Update Time. Sets the PID update rate for closed loop control, where ?x? is 1 to 5, depending on device features.
 * The Update Time, along with the Fan Step register, is used to control the ramp rate of the drive response to provide a cleaner transition of the actual fan operation as the desired fan speed changes.
 * 111 = 1600 ms update interval
 * 110 = 1200 ms update interval
 * 101 = 800 ms update interval
 * 100 = 500 ms update interval
 * 011 = 400 ms update interval
 * 010 = 300 ms update interval
 * 001 = 200 ms update interval
 * 000 = 100 ms update interval
 */
int8_t fan_set_update (fan_cfg_t *fan_cfg, fan_update_interval_t fan_update)
{
    uint8_t reg = 0;
    uint8_t ret = FAN_E_OK;

    ret = fan_i2c_read(fan_dev.dev_id, FAN_CONFIG1, &reg, 1);
    if (ret != FAN_E_OK)
        return ret;

    reg &= 0xF8;
    reg |= fan_update;
    if (fan_update > FAN_UPDATE_1600_MS)
    {
        return FAN_E_INVALID;
    }

    ret = fan_i2c_write(fan_dev.dev_id, FAN_CONFIG1, &reg, 1);
    return ret;
}

/* The Fan Configuration 2 registers control the tachometer measurement and advanced features of the RPMbased Fan Speed Control algorithm.
 * RegAddress 0x33h
 * bit6 = ENRCx: Enable Ramp Rate Control, where ?x? is 1 to 5, depending on device features. This is available only when ENAGx = 0.
 * 1 = Ramp Rate Control enabled.
 * 0 = Ramp Rate Control disabled.
 */
int8_t fan_enable_ramp(fan_cfg_t *fan_cfg, fan_ramp_rate_t ramp_control )
{
    uint8_t reg = 0;
    int8_t ret = FAN_E_OK;
    
    ret = fan_i2c_read(fan_dev.dev_id, FAN_CONFIG2, &reg, 1);
    if (ret != FAN_E_OK)
        return ret;
    
    if (ramp_control)
        reg |= FAN_RAMP_RATE_CONTROL_EN;
    else
        reg &= ~FAN_RAMP_RATE_CONTROL_EN;
    
    ret = fan_i2c_write(fan_dev.dev_id, FAN_CONFIG2, &reg, 1);
    return ret;
}

/* The Fan Configuration 2 registers control the tachometer measurement and advanced features of the RPMbased Fan Speed Control algorithm.
 * RegAddress 0x33h
 * bit5 = GHENx: Glitch Filter Enable. Enables the noise filter that removes high-frequency noise injected on the TACHx pin, where ?x? is 1 to 5, depending on device features.
 * = Glitch filter enabled.
 * 0 = Glitch filter disabled.
 */
int8_t fan_enable_glitch(fan_cfg_t *fan_cfg, fan_glitch_filter_t glitch )
{
    uint8_t reg = 0;
    int8_t ret = FAN_E_OK;
    
    ret = fan_i2c_read(fan_dev.dev_id, FAN_CONFIG2, &reg, 1);
    if (ret != FAN_E_OK)
        return ret;
    
    if (glitch)
        reg |= FAN_GLITCH_FILTER_EN;
    else
        reg &= ~FAN_GLITCH_FILTER_EN;
    
    ret = fan_i2c_write(fan_dev.dev_id, FAN_CONFIG2, &reg, 1);
    return ret;
}

/* The Fan Configuration 2 registers control the tachometer measurement and advanced features of the RPMbased Fan Speed Control algorithm.
 * RegAddress 0x33h
 * bit4-3: DPTx[1:0]: Derivative Options. Determines what form of derivative will be used in the Fan Speed Setting calculation
 * 11 = BOTH
 * 10 = STEP
 * 01 = BASIC
 * 00 = NONE
 */
int8_t fan_set_derivative(fan_cfg_t *fan_cfg, fan_derivative_t derivative )
{
    uint8_t reg = 0;
    int8_t ret = FAN_E_OK;
    
    ret = fan_i2c_read(fan_dev.dev_id, FAN_CONFIG2, &reg, 1);
    if (ret != FAN_E_OK)
        return ret;
    
    reg &= ~FAN_DERIVATIVE_BOTH;
    reg |= derivative;
    
    ret = fan_i2c_write(fan_dev.dev_id, FAN_CONFIG2, &reg, 1);
    return ret;
}

/* The Fan Configuration 2 registers control the tachometer measurement and advanced features of the RPMbased Fan Speed Control algorithm.
 * RegAddress 0x33h
 * bit2-1: ERGx[1:0]: Error Window. Determines the range of the error window.
 * 11 = 200 RPM
 * 10 = 100 RPM
 * 01 = 50 RPM
 * 00 = 0 RPM
 */
int8_t fan_set_error_range(fan_cfg_t *fan_cfg, fan_error_range_t error )
{
    uint8_t reg = 0;
    int8_t ret = FAN_E_OK;
    
    ret = fan_i2c_read(fan_dev.dev_id, FAN_CONFIG2, &reg, 1);
    if (ret != FAN_E_OK)
        return ret;
    
    reg &= ~FAN_ERROR_200RPM;
    reg |= error;
    
    ret = fan_i2c_write(fan_dev.dev_id, FAN_CONFIG2, &reg, 1);
    return ret;
}

/* PID Gain Register
 * RegAddress 0x35h 
 * bit5-4: GDEx[1:0]: Derivative Gain, where ?x? is 1 to 5, depending on device features. The effective gain applied to the KD in the PID equation.
 * bit3-2: GINx[1:0]: Integral Gain, where ?x? is 1 to 5, depending on device features. The effective gain applied to the KI in the PID equation.
 * bit1-0: GPRx[1:0]: Proportional Gain, where ?x? is 1 to 5, depending on device features. The effective gain applied to the KP in the PID equation.
 */
int8_t fan_set_gain(fan_cfg_t *fan_cfg, fan_gain_t gaind, fan_gain_t gaini, fan_gain_t gainp )
{
    uint8_t reg = 0;
    int8_t ret = FAN_E_OK;
    
    if (gaind > FAN_GAIN_8X || gaini > FAN_GAIN_8X || gainp > FAN_GAIN_8X)
        return FAN_E_INVALID;

    reg = gainp | (gaini << 2) | (gaind << 4);
    ret = fan_i2c_write(fan_dev.dev_id, FAN_GAIN, &reg, 1);
    return ret;
}

/* The Fan Spin Up Configuration registers control the settings of the Spin Up Routine 
 * bit7-6: DFCx[1:0]: Drive Fail Count, where ?x? is 1 to 5, depending on device features. Determines when an aging fan has been detected.
 * 11 = 64 update periods
 * 10 = 32 update periods
 * 01 = 16 update periods
 * 00 = Disabled
 */
int8_t fan_set_drivefail(fan_cfg_t *fan_cfg, fan_drive_count_t count )
{
    uint8_t reg = 0;
    int8_t ret = FAN_E_OK;
    
    /* The Configuration Register is software locked. */
    if (fan_cfg->fan_setup_cfg.lock_cfg)
        return FAN_E_INVALID;
    
    if (count > FAN_DRIVE_CNT_64)
        return FAN_E_INVALID;
    
    ret = fan_i2c_read(fan_dev.dev_id, FAN_SPINUP, &reg, 1);
    if (ret != FAN_E_OK)
        return ret;
    
    reg &= 0x3F;
    reg |= count << 6;
    
    ret = fan_i2c_write(fan_dev.dev_id, FAN_SPINUP, &reg, 1);
    return ret;
}

/* The Fan Spin Up Configuration registers control the settings of the Spin Up Routine 
 * bit5: NKCKx: No Kick, where ?x? is 1 to 5, depending on device features. Kick refers to the 100% drive for 1/4 of the spin-up time.
 * 1 = Spin-Up will not drive to 100% PWM
 * 0 = Spin-Up will drive to 100% PWM
 */
int8_t fan_enable_no_kick(fan_cfg_t *fan_cfg, uint8_t no_kick )
{
    uint8_t reg = 0;
    int8_t ret = FAN_E_OK;
    
    /* The Configuration Register is software locked. */
    if (fan_cfg->fan_setup_cfg.lock_cfg)
        return FAN_E_INVALID;
    
    ret = fan_i2c_read(fan_dev.dev_id, FAN_SPINUP, &reg, 1);
    if (ret != FAN_E_OK)
        return ret;
    
    if (no_kick)
        reg |= (1 << 5); 
    else
        reg &= ~(1 << 5);
    
    ret = fan_i2c_write(fan_dev.dev_id, FAN_SPINUP, &reg, 1);
    return ret;
}

/* The Fan Spin Up Configuration registers control the settings of the Spin Up Routine 
 * bit4-2: SPLVx[2:0]: Spin-Up Level, where ?x? is 1 to 5, depending on device features. This is the level the spin-up routine will use.
 * 111 = 65%
 * 110 = 60%
 * 101 = 55%
 * 100 = 50%
 * 011 = 45%
 * 010 = 40%
 * 001 = 35%
 * 000 = 30%
 */
int8_t fan_set_spin_level(fan_cfg_t *fan_cfg, fan_spinup_level_t spin )
{
    uint8_t reg = 0;
    int8_t ret = FAN_E_OK;

    /* The Configuration Register is software locked. */
    if (fan_cfg->fan_setup_cfg.lock_cfg)
        return FAN_E_INVALID;
    
    if (spin > FAN_SPINUP_65PERCENT)
        return FAN_E_INVALID;
    
    ret = fan_i2c_read(fan_dev.dev_id, FAN_SPINUP, &reg, 1);
    reg &= 0xE3;
    reg |= (spin << 2);
    ret = fan_i2c_write(fan_dev.dev_id, FAN_SPINUP, &reg, 1);
    return ret;
}

/* The Fan Spin Up Configuration registers control the settings of the Spin Up Routine 
 * bit1-0: SPTx[1:0]: Spin-Up Time, where ?x? is 1 to 5, depending on device features. This determines how long the spin-up routine will run before releasing the drive.
 * 11 = 2s
 * 10 = 1s
 * 01 = 500 ms
 * 00 = 250 ms
 */
int8_t fan_set_spin_time(fan_cfg_t *fan_cfg, fan_spinup_time_t time )
{
    uint8_t reg = 0;
    int8_t ret = FAN_E_OK;

    /* The Configuration Register is software locked. */
    if (fan_cfg->fan_setup_cfg.lock_cfg)
        return FAN_E_INVALID;
    
    if (time > FAN_SPINUP_2S)
        return FAN_E_INVALID;
    
    ret = fan_i2c_read(fan_dev.dev_id, FAN_SPINUP, &reg, 1);
    reg &= 0xEC;
    reg |= time;
    ret = fan_i2c_write(fan_dev.dev_id, FAN_SPINUP, &reg, 1);
    return ret;
}

/* This register determines the maximum step size for the ramp rate control 
 * RegAddress 0x37h */
int8_t fan_set_max_step(fan_cfg_t *fan_cfg, uint8_t step )
{
    int8_t ret = FAN_E_OK;
    
    /* The Configuration Register is software locked. */
    if (fan_cfg->fan_setup_cfg.lock_cfg)
        return FAN_E_INVALID;
   
    ret = fan_i2c_write(fan_dev.dev_id, FAN_MAX_STEP, &step, 1);
    return ret;
}

/* The Fan Minimum Drive Register (see Section 4.3.3.4 ?Minimum Drive Setting?) stores the minimum drive
setting for each RPM-based Fan Speed Control algorithm.
 * RegAddress 0x38h */
int8_t fan_set_min_drive(fan_cfg_t *fan_cfg, double  min_drive  )
{
    uint8_t set_data;
    int8_t ret = FAN_E_OK;
    
    set_data = min_drive / 100 * 255;
   
    ret = fan_i2c_write(fan_dev.dev_id, FAN_MIN_DRIVE, &set_data, 1);
    return ret;
}


/* Software Lock Register
 * RegAddress 0xEFh 
 * bit0: LOCK: Enables the SWL function
 * 1 = All SWL registers are locked and read-only. Unlock occurs on power cycle.
 * 0 = All SWL registers are writable.
 */
int8_t fan_set_sw_lock (fan_cfg_t *fan_cfg, uint8_t sw_lock_en)
{
    uint8_t reg = 0;
    int8_t ret = FAN_E_OK;
    
    fan_cfg->fan_setup_cfg.lock_cfg = (sw_lock_en != 0) ? 1 : 0;
    /* LOCK: Enables the SWL function
     * 1 = All SWL registers are locked and read-only. Unlock occurs on power cycle.
     * 0 = All SWL registers are writable. */
    reg = fan_cfg->fan_setup_cfg.lock_cfg;
    
    ret = fan_i2c_write(fan_dev.dev_id, FAN_SOFTWARE_LOCK, &reg, 1);
	return ret;
}


int8_t fan_set_valid_tach (fan_cfg_t *fan_cfg, uint16_t tach)
{
    double tmp;
    uint8_t set_tach;
    int8_t ret = FAN_E_OK;

    /* The Configuration Register is software locked. */
    if (fan_cfg->fan_setup_cfg.lock_cfg)
        return FAN_E_INVALID;
    
//     if ( tach > 16000 || tach < 480 )
     if ( tach > 16000 || tach < 32 )
         return FAN_E_INVALID;
    
    tmp = (float)( 1 / fan_cfg->fan_setup_cfg.poles_cfg ); /* configured when called fan_set_edges() */
    tmp *= fan_cfg->fan_setup_cfg.edges_cfg - 1; /* configured when called fan_set_edges() */
    tmp /= (float)( tach  / fan_cfg->fan_setup_cfg.mul_cfg); /* configured when called fan_set_range() */
    tach = tmp * fan_cfg->fan_setup_cfg.frequency_cfg * SEC_MIN;
    set_tach = tach >> 5;

    ret = fan_i2c_write(fan_dev.dev_id, FAN_VALID_TACH, &set_tach, 1);
    return ret;
}

int8_t fan_set_target_tach (fan_cfg_t *fan_cfg, uint16_t tach)
{
    double tmp;
    uint8_t write_data[2];
    int8_t ret = FAN_E_OK;

    if ( tach > 16000 || tach < 480 )
         return FAN_E_INVALID;
    
    tmp = (float)( 1 / fan_cfg->fan_setup_cfg.poles_cfg ); /* configured when called fan_set_edges() */
    tmp *= fan_cfg->fan_setup_cfg.edges_cfg - 1; /* configured when called fan_set_edges() */
    tmp /= (float)( tach  / fan_cfg->fan_setup_cfg.mul_cfg); /* configured when called fan_set_range() */
    tach = tmp * fan_cfg->fan_setup_cfg.frequency_cfg * SEC_MIN;
    write_data[ 0 ] = (uint8_t)( tach & 0xFF ) << 3;
    write_data[ 1 ] = (uint8_t)( tach >> 5 );

    ret = fan_i2c_write(fan_dev.dev_id, FAN_TACH_TARGET_LOW, &write_data[0], 1);
    ret = fan_i2c_write(fan_dev.dev_id, FAN_TACH_TARGET_HIGH, &write_data[1], 1);
    return ret;
}

int8_t fan_get_tach (fan_cfg_t *fan_cfg, uint16_t *data_out)
{
    uint8_t read_data[ 2 ] = { 0 };
    int8_t ret = FAN_E_OK;

    ret = fan_i2c_read(fan_dev.dev_id, FAN_TACH_READING_HIGH, &read_data[0], 1);
    if (ret != FAN_E_OK)
        return ret;
    
    ret = fan_i2c_read(fan_dev.dev_id, FAN_TACH_READING_LOW, &read_data[1], 1);
    if (ret != FAN_E_OK)
        return ret;
    
    *data_out = read_data[0];
    *data_out <<= 8;
    *data_out |= read_data[1];
    *data_out >>= 3;

    if ( *data_out != 0 )
    {
        *data_out = ( 3932160 * 2 ) / *data_out;
    }
    
    return ret;
}

int8_t fan_get_valid_tach (fan_cfg_t *fan_cfg, uint16_t *data_out)
{
    uint8_t reg = 0;
    int8_t ret = FAN_E_OK;

    ret = fan_i2c_read(fan_dev.dev_id, FAN_VALID_TACH, &reg , 1);
    if (ret != FAN_E_OK)
        return ret;
    
    *data_out = reg;
    *data_out <<= 8;
    
    return ret;
}

int8_t fan_get_driveband_fail (fan_cfg_t *fan_cfg, uint16_t *data_out)
{
    uint8_t data_l = 0;
    uint8_t data_h = 0;
    int8_t ret = FAN_E_OK;
    
    ret = fan_i2c_read(fan_dev.dev_id, FAN_FAIL_LOW, &data_l , 1);
    if (ret != FAN_E_OK)
        return ret;
    ret = fan_i2c_read(fan_dev.dev_id, FAN_FAIL_HIGH, &data_h , 1);
    if (ret != FAN_E_OK)
        return ret;
    
    *data_out = data_h;
    *data_out <<= 8;
    *data_out |= data_l;
    *data_out >>= 3;

    return ret;
}


int8_t fan_set_frequency (fan_cfg_t *fan_cfg, double freq)
{
    fan_cfg->fan_setup_cfg.frequency_cfg = freq;
    
    return FAN_E_OK;
}

fan_cfg_t* fan_get_config(void)
{
    return &fan_cfg;
}
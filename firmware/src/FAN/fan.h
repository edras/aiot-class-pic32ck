/******************************************************************************/
/** Fan Click MIKROE-2004                                                     */
/*******************************************************************************
 Fan Click MIKROE-2004

  Company:
    Microchip Technology Inc.

  File Name:
    fan.h

  Summary:
    Fan Click MIKROE-2004 board support - header file

  Description:
    Fan Click is based on the EMS2301, a single/multiple RPM-based PWM 
    fan controller from Microchip. 
 ******************************************************************************/

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

#ifndef _FAN_H    /* Guard against multiple inclusion */
#define _FAN_H

#include "hal.h"

/* Registers */
#define FAN_CONFIGURATION         0x20
    #define FAN_ALERT_PIN_EN      0x80
    #define FAN_DIS_TIMEOUT_FCT   0x40
    #define FAN_WDG_EN            0x20
    #define FAN_INT_TACH_CLK_EN   0x02
    #define FAN_USER_CLK_EN       0x01

#define FAN_STATUS                0x24
#define FAN_STALL_STATUS          0x25
#define FAN_SPIN_STATUS           0x26
#define FAN_DRIVE_FAIL_STATUS     0x27
#define FAN_INTERRUPT_ENABLE      0x29
#define FAN_PWM_POLARITY          0x2A
#define FAN_PWM_OUTPUT_CONFIG     0x2B
#define FAN_PWM_BASE45_FREQ       0x2C

#define FAN_PWM_BASE123_FREQ      0x2D
    typedef enum
    {
        FAN_PWM_BASE_26_00KHZ =      0x00,
        FAN_PWM_BASE_19_53KHZ =      0x01,
        FAN_PWM_BASE_4_882KHZ =      0x02,
        FAN_PWM_BASE_2_441KHZ =      0x03,
    } fan_pwm_base_t;

#define FAN_SETTING               0x30
    typedef enum
    {
        FAN_DIRECT_MODE =         0x80,
        FAN_FSC_MODE =            0x00,
    } fan_mode_t;

#define FAN_DIVIDE                0x31
#define FAN_CONFIG1               0x32
    /* bit7: ENAG (Enable Closed Loop algorithm) */
    typedef enum
    {
        FAN_CL_ALG_EN =           0x80,
        FAN_CL_ALG_DIS =          0x00, /* device is placed in Direct Setting mode */
    } fan_config_cl_alt_t;
    /* bit6-5: RNG (Range. Sets the minimum fan speed measured and reported) */
    typedef enum
    {
        FAN_4000_RPM =            0x60,
        FAN_2000_RPM =            0x40,
        FAN_1000_RPM =            0x20,
        FAN_500_RPM =             0x00,
    } fan_range_rpm_t;
    /* bit4-3: EDG (Edges. Sets the number of edges to sample when calculating RPM) */
    typedef enum
    {
        FAN_9_EDGES =             0x18,
        FAN_7_EDGES =             0x10,
        FAN_5_EDGES =             0x08,
        FAN_3_EDGES =             0x00,
    } fan_edges_t;
    /* bit2-0: UDT (Update Time. Sets the PID update rate for closed loop control)
     The Update Time, along with the Fan Step register, is used to control
     the ramp rate of the drive response to provide a cleaner transition of the actual fan operation
     as the desired fan speed changes. */
    typedef enum
    {
        FAN_UPDATE_1600_MS =      0x07,
        FAN_UPDATE_1200_MS =      0x06,
        FAN_UPDATE_800_MS =       0x05,
        FAN_UPDATE_500_MS =       0x04,
        FAN_UPDATE_400_MS =       0x03,
        FAN_UPDATE_300_MS =       0x02,
        FAN_UPDATE_200_MS =       0x01,
        FAN_UPDATE_100_MS =       0x00,
    } fan_update_interval_t;

#define FAN_CONFIG2               0x33
    /* bit6: ENRC (Enable Ramp Rate Control. This is available only when ENAGx = 0.) */
    typedef enum
    {
        FAN_RAMP_RATE_CONTROL_EN =  0x40,
        FAN_RAMP_RATE_CONTROL_DIS = 0x00,
    } fan_ramp_rate_t;
    /* bit5: GHEN (Glitch Filter Enable. Enables the noise filter that removes high-frequency noise injected on the TACHx pin) */
    typedef enum
    {
        FAN_GLITCH_FILTER_EN =      0x20,
        FAN_GLITCH_FILTER_DIS =     0x00,
    } fan_glitch_filter_t;
    /* bit4-3: DPT (Derivative Options. Determines what form of derivative will be used in the Fan Speed Setting calculation) */
    typedef enum
    {
        FAN_DERIVATIVE_BOTH =       0x18,
        FAN_DERIVATIVE_STEP =       0x10,
        FAN_DERIVATIVE_BASIC =      0x01,
        FAN_DERIVATIVE_NONE =       0x00,
    } fan_derivative_t;
    /* bit2-1: ERGx[1:0]: Error Window. Determines the range of the error window. */
    typedef enum
    {
        FAN_ERROR_200RPM =          0x6,
        FAN_ERROR_100RPM =          0x4,
        FAN_ERROR_50RPM =           0x2,
        FAN_ERROR_0RPM =            0x0,
    } fan_error_range_t;
    
    
#define FAN_GAIN                  0x35
    typedef enum
    {
        FAN_GAIN_1X =             0x00,
        FAN_GAIN_2X =             0x01,
        FAN_GAIN_4X =             0x02,
        FAN_GAIN_8X =             0x03,                
    } fan_gain_t;

#define FAN_SPINUP                0x36
    typedef enum
    {
        FAN_DRIVE_CNT_00 =        0x0,
        FAN_DRIVE_CNT_16 =        0x1,
        FAN_DRIVE_CNT_32 =        0x2,
        FAN_DRIVE_CNT_64 =        0x3,
    } fan_drive_count_t;

    typedef enum
    {
        FAN_SPINUP_30PERCENT =      0x00,
        FAN_SPINUP_35PERCENT =      0x01,
        FAN_SPINUP_40PERCENT =      0x02,
        FAN_SPINUP_45PERCENT =      0x03,
        FAN_SPINUP_50PERCENT =      0x04,
        FAN_SPINUP_55PERCENT =      0x05,
        FAN_SPINUP_60PERCENT =      0x06,
        FAN_SPINUP_65PERCENT =      0x07,
    } fan_spinup_level_t;
            
    typedef enum
    {
        FAN_SPINUP_250MS =          0x00,
        FAN_SPINUP_500MS =          0x01,
        FAN_SPINUP_1S =             0x02,
        FAN_SPINUP_2S =             0x03,
    } fan_spinup_time_t;
            
#define FAN_MAX_STEP              0x37
#define FAN_MIN_DRIVE             0x38
#define FAN_VALID_TACH            0x39
#define FAN_FAIL_LOW              0x3A
#define FAN_FAIL_HIGH             0x3B
#define FAN_TACH_TARGET_LOW       0x3C
#define FAN_TACH_TARGET_HIGH      0x3D
#define FAN_TACH_READING_HIGH     0x3E
#define FAN_TACH_READING_LOW      0x3F

/* Fan 2 Settings 0x40 - 0x4F */
/* Fan 3 Settings 0x50 - 0x5F */
/* Fan 4 Settings 0x60 - 0x6F */
/* Fan 5 Settings 0x70 - 0x7F */

#define FAN_SOFTWARE_LOCK         0xEF
#define FAN_PRODUCT_ID            0xFD /* 37h POR Value */
#define FAN_MANUFACTURER_ID       0xFE
#define FAN_REVISION              0xFF


#define FAN_DEFAULT_CFG           0x80
/* MBus address is set at 0101_111r/w (EMC2301 only). */
#define FAN_DEVICE_ADDR           0x2F
#define FAN_PRODUCT_ID_VAL        0x37
#define FAN_MANUFACTURER_ID_VAL   0x5D

#define FAN_MAX_TX_BUF_SIZE    256
        
/**\name API success/error codes */
#define FAN_E_OK                0
#define FAN_E_NULL_PTR         -1
#define FAN_E_DEV_NOT_FOUND    -2
#define FAN_E_COMM_FAIL        -3
#define FAN_E_INVALID          -4

#define SEC_MIN 60

/**
 * @brief Fan setup configuration.
 */
typedef struct
{
  uint8_t  poles_cfg;
  uint8_t  edges_cfg;
  uint8_t  range_cfg;
  uint8_t  mul_cfg;
  double   frequency_cfg;
  uint8_t  mode_cfg;
  uint8_t  lock_cfg;

} fan_setup_cfg_t;


/**
 * @brief Click configuration structure definition.
 */
typedef struct
{
    uint32_t         i2c_speed;        /**< Module speed. >*/
    uint8_t          i2c_address;      /**< Device slave address. >*/
    fan_setup_cfg_t  fan_setup_cfg;    /**< Fan configuration setup. >*/
} fan_cfg_t;

int8_t fan_init(void);
bool   fan_available(void);
int8_t fan_device_cfg(fan_cfg_t *fan_cfg, uint8_t config_reg);
int8_t fan_get_status (fan_cfg_t *fan_cfg, uint8_t *status);
int8_t fan_get_stall_status (fan_cfg_t *fan_cfg, uint8_t *status);
int8_t fan_get_spin_status (fan_cfg_t *fan_cfg, uint8_t *status);
int8_t fan_get_drive_fail_status (fan_cfg_t *fan_cfg, uint8_t *status);
int8_t fan_int_en (fan_cfg_t *fan_cfg, uint8_t enable);
int8_t fan_set_pwm_polarity (fan_cfg_t *fan_cfg, uint8_t polarity);
int8_t fan_set_pwm_drive_output (fan_cfg_t *fan_cfg, uint8_t mode);
int8_t fan_set_pwm_base(fan_cfg_t *fan_cfg, fan_pwm_base_t pwm_base, uint8_t pwm_id);
int8_t fan_set_duty_cycle_percentage (fan_cfg_t *fan_cfg, double percentage);
int8_t fan_set_mode (fan_cfg_t *fan_cfg, fan_mode_t fan_mode);
int8_t fan_set_range (fan_cfg_t *fan_cfg, fan_range_rpm_t fan_range);
int8_t fan_set_edges (fan_cfg_t *fan_cfg, fan_edges_t fan_edge);
int8_t fan_set_update (fan_cfg_t *fan_cfg, fan_update_interval_t fan_update);
int8_t fan_enable_ramp(fan_cfg_t *fan_cfg, fan_ramp_rate_t ramp_control );
int8_t fan_enable_glitch(fan_cfg_t *fan_cfg, fan_glitch_filter_t glitch );
int8_t fan_set_derivative(fan_cfg_t *fan_cfg, fan_derivative_t derivative );
int8_t fan_set_error_range(fan_cfg_t *fan_cfg, fan_error_range_t error );
int8_t fan_set_gain(fan_cfg_t *fan_cfg, fan_gain_t gaind, fan_gain_t gaini, fan_gain_t gainp );
int8_t fan_set_drivefail(fan_cfg_t *fan_cfg, fan_drive_count_t count );
int8_t fan_enable_no_kick(fan_cfg_t *fan_cfg, uint8_t no_kick );
int8_t fan_set_spin_level(fan_cfg_t *fan_cfg, fan_spinup_level_t spin );
int8_t fan_set_spin_time(fan_cfg_t *fan_cfg, fan_spinup_time_t time );
int8_t fan_set_max_step(fan_cfg_t *fan_cfg, uint8_t step );
int8_t fan_set_min_drive(fan_cfg_t *fan_cfg, double  min_drive  );
int8_t fan_set_sw_lock (fan_cfg_t *fan_cfg, uint8_t sw_lock_en);
int8_t fan_set_default_setting (fan_cfg_t *fan_cfg);


int8_t fan_set_valid_tach (fan_cfg_t *fan_cfg, uint16_t tach);
int8_t fan_set_target_tach (fan_cfg_t *fan_cfg, uint16_t tach);
int8_t fan_get_valid_tach (fan_cfg_t *fan_cfg, uint16_t *tach);
int8_t fan_get_driveband_fail (fan_cfg_t *fan_cfg, uint16_t *data_out);
int8_t fan_get_tach (fan_cfg_t *fan_cfg, uint16_t *data_out);
int8_t fan_set_frequency (fan_cfg_t *fan_cfg, double freq);

fan_cfg_t* fan_get_config(void);

/*!
 * @brief This API reads the data from the given register address of the sensor.
 */
int8_t fan_get_reg(uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
        
#endif /* _FAN_H */

/* *****************************************************************************
 End of File
 */

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
#ifndef APC1_H_
#define APC1_H_

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

#include "hal.h"

#define APC1_DEV_ADDR                 0x12
    
#define APC1_HEADER_0                 0x42
#define APC1_HEADER_1                 0x4D
#define APC1_ADDRESS_CMD              0x40
#define APC1_ADDRESS_RESPONSE         0x47
#define APC1_CMD_SET_MEAS_MODE        0xE1
#define APC1_CMD_REQ_MEAS             0xE2
#define APC1_CMD_SET_MODE             0xE4
#define APC1_CMD_GET_INFO             0xE9
#define APC1_INFO_LEN                 23
#define APC1_MEAS_LEN                 64
#define APC1_ANSWER_TIME              200
#define APC1_RESET_TIME               2000

#define APC1_MODE_IDLE                0x00
#define APC1_MODE_MEASUREMENT         0x01
#define APC1_MODE_RESET               0x0F

/* ----- PUBLIC API Sensor structures -----*/
typedef struct
{
  uint8_t  name[5];
  uint8_t  type;
  uint8_t  serial_number[8];
  uint16_t firmware;
} apc1_data;

typedef struct
{
    uint16_t pm1_0;             /* PM1.0 mass concentration. */
    uint16_t pm2_5;             /* PM2.5 mass concentration. */
    uint16_t pm10;              /* PM10 mass concentration. */
    uint16_t pm1_0_air;         /* PM1.0 mass concentration in atmospheric environment. */
    uint16_t pm2_5_air;         /* PM2.5 mass concentration in atmospheric environment. */
    uint16_t pm10_air;          /* PM10 mass concentration in atmospheric environment. */
    uint16_t part_over_0_3um;   /* Number of particles with diameter > 0.3�m in 0.1L of air. */
    uint16_t part_over_0_5um;   /* Number of particles with diameter > 0.5�m in 0.1L of air. */
    uint16_t part_over_1_0um;   /* Number of particles with diameter > 1.0�m in 0.1L of air. */
    uint16_t part_over_2_5um;   /* Number of particles with diameter > 2.5�m in 0.1L of air. */
    uint16_t part_over_5_0um;   /* Number of particles with diameter > 5.0�m in 0.1L of air. */
    uint16_t part_over_10um;    /* Number of particles with diameter > 10�m in 0.1L of air. */
    uint16_t tvoc;              /* TVOC output. */
    uint16_t eco2;              /* Output in ppm CO2 equivalents. */
    float t_comp;               /* Compensated temperature. */
    float rh_comp;              /* Compensated humidity. */
    float t_raw;                /* Uncompensated temperature. */
    float rh_raw;               /* Uncompensated humidity. */
    uint32_t rs0;               /* Gas sensor raw resistance value RSi. */
    uint32_t rs1;               /* Gas sensor raw resistance value RSi. */
    uint32_t rs2;               /* Gas sensor raw resistance value RSi. */
    uint32_t rs3;               /* Gas sensor raw resistance value RSi. */
    uint8_t aqi;                /* Air Quality Index according to UBA Classification of TVOC value. */
    uint8_t firmware;           /* Firmware version. */
    uint8_t error_code;         /* Error code. */
    uint16_t checksum;  
} apc1_measurement;

/* ----- PUBLIC API Return Codes -----*/
// APC1_OK should be the last to have value 0, errors should be negative
typedef enum  {
    APC1_E_NOK = -5,
    APC1_E_NULL_PTR,
    APC1_E_DEV_NOT_FOUND,
    APC1_E_COMM_FAIL,
    APC1_E_INVALID,
    APC1_OK
} APC1_RC;

/* ----- PUBLIC API Functions -----*/
 /*!
 * @brief Initialize the APC1 sensor
 *
 * @return Result of API execution status
 */
APC1_RC APC1_init();

/* ----- PUBLIC API Functions -----*/
 /*!
 * @brief Check if APC1 sensor is available
 *
 * @return true if APC1 is available, false otherwise
 */
bool APC1_available();

 /*!
 * @brief Reads the sensor information and stores it in the buffer provided by the user
 *
 * @param[out] data - data buffer for sensor information
 *
 * @return Result of API execution status
 */
APC1_RC APC1_get_info(apc1_data *data);

 /*!
 * @brief Sets DEEP SLEEP mode for APC1 sensor based on state parameter
 *
 * @param[in] state - sets or disables the APC1 DEEP SLEEP sensor mode
 *
 * @return Result of API execution status
 */
APC1_RC APC1_deep_sleep(bool state);

/*!
 * @brief Sets APC1 sensor in IDLE mode
 *
 * @return Result of API execution status
 */
APC1_RC APC1_set_idle_mode();

/*!
 * @brief Sets APC1 sensor in MEASUREMENT mode
 *
 * @return Result of API execution status
 */
APC1_RC APC1_set_meas_mode();

 /*!
 * @brief Reads the sensor values and stores them in the internal buffer
 *
 * @return Result of API execution status
 */
APC1_RC APC1_get_measurement(void);

 /*!
 * @brief Return the last measured values from the internal buffer
 *
 * @return APC1 data structure
 */
apc1_measurement* APC1_get_data(void);

/*!
 * @brief Sends Software Reset command to APC1 sensor
 *
 * @return Result of API execution status
 */
APC1_RC APC1_soft_reset();

/*!
 * @brief Hardware resets the APC1 sensor via RST pin
 *
 * @return Result of API execution status
 */
APC1_RC APC1_hard_reset();

 /*!
 * @brief Used to enable to disable debug messages
 *
 * @param[in] state - sets or disables the debug messages
 *
 * @return Result of API execution status
 */
void APC1_set_debug(bool state);

/*!
 * @brief Print sensor data in CSV format
 *
 * @return None
 */
void APC1_print_csv_data();

/*!
 * @brief Print sensor data
 *
 * @return None
 */
void APC1_print_data();

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* APC1_H_ */
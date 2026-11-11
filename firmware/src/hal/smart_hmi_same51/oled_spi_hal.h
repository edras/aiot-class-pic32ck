/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _OLED_SPI_HAL_H    /* Guard against multiple inclusion */
#define _OLED_SPI_HAL_H

#include "hal.h"

#ifdef MIKROE_OLED_C

#define OLED_rw_low         AN2_Clear
#define OLED_enable_high    EN2_Set
#define OLED_reset_low      RST2_Clear
#define OLED_reset_high     RST2_Set
#define OLED_dc_low         PWM2_Clear
#define OLED_dc_high        PWM2_Set
#define OLED_cs_low         CS2_Clear
#define OLED_cs_high        CS2_Set
#define OLED_delay_ms       TIMER_DelayMs

#else

#define OLED_rw_low(...)       do{}while(false)
#define OLED_enable_high(...)  do{}while(false)
#define OLED_reset_low(...)    do{}while(false)
#define OLED_reset_high(...)   do{}while(false)
#define OLED_dc_low(...)       do{}while(false)
#define OLED_dc_high(...)      do{}while(false)
#define OLED_cs_low(...)       do{}while(false)
#define OLED_cs_high(...)      do{}while(false)
#define OLED_delay_ms(...)     do{}while(false)

#endif

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

void OLED_comm_open();

/*!
 *  @brief This API sends a byte of data through SPI
 *
 *  @param[in] byte : Data to be sent
 */
void OLED_spi_byteWrite(uint8_t byte);

/*!
 *  @brief This API sends a data through SPI
 *
 *  @param[in] byte : Data to be sent
 *  @param[in] len : Length of the data
 */
void OLED_spi_Write(uint8_t *data, size_t len);

/*!
 *  @brief This API checks if data is still sent through SPI
 */
bool OLED_spi_is_transmitter_busy();

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _OLED_SPI_HAL_H */

/* *****************************************************************************
 End of File
 */

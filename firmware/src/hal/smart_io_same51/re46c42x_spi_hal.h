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

#ifndef _RE46C42X_SPI_HAL_H    /* Guard against multiple inclusion */
#define _RE46C42X_SPI_HAL_H

#include "hal.h"

#ifdef MIKROE_RE46C42X

#define RE46C42X_ADC_CHANNEL    ADC_POSINPUT_AIN0   //this is configured on AN3 pin
#define RE46C42X_enable_high    PWM3_Set
#define RE46C42X_enable_low     PWM3_Clear
#define RE46C42X_cs_low         CS3_Clear
#define RE46C42X_cs_high        CS3_Set
#define RE46C42X_delay_ms       TIMER_DelayMs
#define RE46C42X_delay_us       TIMER_DelayUs

#else
#define RE46C42X_ADC_CHANNEL(...)       do{}while(false)
#define RE46C42X_enable_high(...)       do{}while(false)
#define RE46C42X_enable_low(...)        do{}while(false)
#define RE46C42X_cs_low(...)            do{}while(false)
#define RE46C42X_cs_high(...)           do{}while(false)
#define RE46C42X_delay_ms(...)          do{}while(false)
#define RE46C42X_delay_us(...)          do{}while(false)
#endif

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif
/*!
 *  @brief This API sends a byte of data through SPI
 *
 *  @param[in] byte : Data to be sent
 */
void RE46C42X_spi_byteWrite(uint8_t byte);

/*!
 *  @brief This API sends data through SPI
 *
 *  @param[in] byte : Data to be sent
 *  @param[in] len : Length of the data
 */
void RE46C42X_spi_Write(uint8_t *data, size_t len);

/*!
 *  @brief This API reads data through SPI
 *
 *  @param[in] reg : Register to be read
 *  @param[out] data : Buffer to store data
 */
void RE46C42X_spi_Read(uint8_t *reg, uint8_t *data);

/*!
 *  @brief This API checks if data is still sent through SPI
 */
bool RE46C42X_spi_is_transmitter_busy();

/*!
 * @brief This API configures the ADC channel and reads the value on ADC
 *
 * @return ADC value
 */
uint16_t RE46C42X_adc_read();

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _RE46C42X_SPI_HAL_H */

/* *****************************************************************************
 End of File
 */
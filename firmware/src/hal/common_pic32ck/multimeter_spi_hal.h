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

#ifndef _MULTIMETER_SPI_HAL_H    /* Guard against multiple inclusion */
#define _MULTIMETER_SPI_HAL_H

#include "hal.h"

#ifdef MIKROE_MULTIMETER

#define MULTIMETER_range_sel_0_low         AN2_Clear
#define MULTIMETER_range_sel_0_high        AN2_Set
#define MULTIMETER_range_sel_1_low         PWM2_Clear
#define MULTIMETER_range_sel_1_high        PWM2_Set
#define MULTIMETER_range_sel_2_low         EN2_Clear
#define MULTIMETER_range_sel_2_high        EN2_Set
#define MULTIMETER_cs_low                  CS2_Clear
#define MULTIMETER_cs_high                 CS2_Set
#define MULTIMETER_delay_ms                TIMER_DelayMs

#else

#define MULTIMETER_range_sel_0_low(...)         do{}while(false)
#define MULTIMETER_range_sel_0_high(...)        do{}while(false)
#define MULTIMETER_range_sel_1_low(...)         do{}while(false)
#define MULTIMETER_range_sel_1_high(...)        do{}while(false)
#define MULTIMETER_range_sel_2_low(...)         do{}while(false)
#define MULTIMETER_range_sel_2_high(...)        do{}while(false)
#define MULTIMETER_cs_low(...)                  do{}while(false)
#define MULTIMETER_cs_high(...)                 do{}while(false)
#define MULTIMETER_delay_ms(...)                do{}while(false)

#endif

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief This API writes and reads data to and from SPI
 *
 *  @param[in]  tx_data : Data to be sent
 *  @param[in]  tx_len  : Length of the data to be sent
 *  @param[out] rx_data : Buffer to store data
 *  @param[in]  rx_len  : Length of the data to be read
 * 
 *  @return True if data was read correctly, false otherwise.
 */
bool MULTIMETER_spi_write_read(uint8_t *tx_data, size_t tx_len, uint8_t *rx_data, size_t rx_len);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _MULTIMETER_SPI_HAL_H */

/* *****************************************************************************
 End of File
 */

#include "multimeter_spi_hal.h"
#include "bus_hal.h"

/*
 * INSTRUCTIONS
 * ============
 *
 * Implement all functions where they are marked as IMPLEMENT.
 * Follow the function specification in the comments.
 */

#ifndef MIKROE_MULTIMETER
bool MULTIMETER_init(void) { return false; }
bool MULTIMETER_available(void) { return false; }
void MULTIMETER_calibrate(void) {}
float MULTIMETER_measure_I(void) { return 0.0f; }
float MULTIMETER_measure_U(void) { return 0.0f; }
float MULTIMETER_measure_R(void) { return 0.0f; }
float MULTIMETER_measure_C(void) { return 0.0f; }
#endif

bool MULTIMETER_spi_write_read(uint8_t *tx_data, size_t tx_len, uint8_t *rx_data, size_t rx_len)
{
#ifdef MIKROE_MULTIMETER
    return HAL_SPI_WriteRead(tx_data, tx_len, rx_data, rx_len);
#else
    (void)tx_data; (void)tx_len; (void)rx_data; (void)rx_len;
    return false;
#endif
}

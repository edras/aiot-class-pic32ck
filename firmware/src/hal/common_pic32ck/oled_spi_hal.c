#include "oled_spi_hal.h"
#include "bus_hal.h"

/*
 * INSTRUCTIONS
 * ============
 *
 * Implement all functions where they are marked as IMPLEMENT.
 * Follow the function specification in the comments.
 */

/**
 * Select the current spi bus.
 *
 *
 */

void OLED_comm_open(void)
{
    
}

void OLED_spi_byteWrite(uint8_t byte)
{
    HAL_SPI_Write(&byte, 1);
}

void OLED_spi_Write(uint8_t *data, size_t len)
{
    HAL_SPI_Write(data, len);
}

bool OLED_spi_is_transmitter_busy()
{
    return HAL_SPI_IsTransmitterBusy();
}
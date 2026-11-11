#include "oled_spi_hal.h"

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

#ifndef MIKROE_OLED_C
void OLEDC_initialize(void) {}
uint8_t oledc_is_any_message(void) { return true; }
void oledc_draw_string_on_bg(uint8_t x, uint8_t y, uint8_t sx, uint8_t sy, char* string, uint16_t color, uint16_t bg_color) {}
#endif

void OLED_comm_open(void)
{
    
}

void OLED_spi_byteWrite(uint8_t byte)
{
//    SERCOM1_SPI_Write(&byte, 1);
}

void OLED_spi_Write(uint8_t *data, size_t len)
{
//    SERCOM1_SPI_Write(data, len);
}

bool OLED_spi_is_transmitter_busy()
{
    return false; //SERCOM1_SPI_IsTransmitterBusy();
}
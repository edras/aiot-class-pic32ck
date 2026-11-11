#include "re46c42x_spi_hal.h"

/*
 * INSTRUCTIONS
 * ============
 *
 * Implement all functions where they are marked as IMPLEMENT.
 * Follow the function specification in the comments.
 */
void RE46C42X_spi_byteWrite(uint8_t byte)
{
    SERCOM1_SPI_Write(&byte, 1);
}

void RE46C42X_spi_Write(uint8_t *data, size_t len)
{
    SERCOM1_SPI_Write(data, len);
}

void RE46C42X_spi_Read(uint8_t *reg, uint8_t *data)
{
    SERCOM1_SPI_WriteRead(reg, 1, data, 2);
}

bool RE46C42X_spi_is_transmitter_busy()
{
    return SERCOM1_SPI_IsTransmitterBusy();
}

uint16_t RE46C42X_adc_read()
{
#ifdef MIKROE_RE46C42X
    /* Configure channel input */
    ADC1_ChannelSelect(RE46C42X_ADC_CHANNEL, ADC_NEGINPUT_GND);

    /* Start ADC conversion */
    ADC1_ConversionStart();

    /* Wait till ADC conversion result is available */
    while(!ADC1_ConversionStatusGet()) {};
    
    return ADC1_ConversionResultGet();
#endif
    return 0;
}
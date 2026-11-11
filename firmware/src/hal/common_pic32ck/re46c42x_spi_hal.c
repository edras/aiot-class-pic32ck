#include "re46c42x_spi_hal.h"
#include "bus_hal.h"

/*
 * INSTRUCTIONS
 * ============
 *
 * Implement all functions where they are marked as IMPLEMENT.
 * Follow the function specification in the comments.
 */
void RE46C42X_spi_byteWrite(uint8_t byte)
{
    HAL_SPI_Write(&byte, 1);
}

void RE46C42X_spi_Write(uint8_t *data, size_t len)
{
    HAL_SPI_Write(data, len);
}

void RE46C42X_spi_Read(uint8_t *reg, uint8_t *data)
{
    HAL_SPI_WriteRead(reg, 1, data, 2);
}

bool RE46C42X_spi_is_transmitter_busy()
{
    return HAL_SPI_IsTransmitterBusy();
}

uint16_t RE46C42X_adc_read()
{
#ifdef MIKROE_RE46C42X
    /* Enable software-controlled conversion on the target channel */
    ADC_SoftwareControlledConversionEnable(RE46C42X_ADC_CHANNEL);

    /* Start ADC conversion */
    ADC_ChannelConversionStart();

    /* Wait till ADC conversion result is available */
    while (!ADC_ChannelResultIsReady(RE46C42X_ADC_CHANNEL)) {}

    return ADC_ResultGet(RE46C42X_ADC_CHANNEL);
#endif
    return 0;
}
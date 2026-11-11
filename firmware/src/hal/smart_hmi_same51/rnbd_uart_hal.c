#include "rnbd_uart_hal.h"

size_t RNBD_transmit_ready()
{
    return SERCOM0_USART_WriteCountGet();
}

size_t RNBD_available_data()
{
    return SERCOM0_USART_ReadCountGet();
}

size_t RNBD451_write(uint8_t *txData, size_t len)
{
    return SERCOM0_USART_Write(txData, len);
}

size_t RNBD451_read(uint8_t* pRdBuffer, const size_t size)
{
    return SERCOM0_USART_Read(pRdBuffer, size);
}
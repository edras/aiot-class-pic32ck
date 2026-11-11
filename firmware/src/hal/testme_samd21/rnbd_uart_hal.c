#include "rnbd_uart_hal.h"

#ifdef MIKROE_RNDB

size_t RNBD_transmit_ready()
{
    return SERCOM2_USART_WriteCountGet();
}

size_t RNBD_available_data()
{
    return SERCOM2_USART_ReadCountGet();
}

size_t RNBD451_write(uint8_t *txData, size_t len)
{
    return SERCOM2_USART_Write(txData, len);
}

size_t RNBD451_read(uint8_t* pRdBuffer, const size_t size)
{
    return SERCOM2_USART_Read(pRdBuffer, size);
}

#else

bool RNBD_init(void) { return 1; }
bool RNBD_set_async_message_handler(char* pBuffer, uint8_t len) { return false; }
bool RNBD_enter_cmd_mode(void) { return false; }
bool RNBD_set_name(const char *name, uint8_t nameLen){ return false; }
bool RNBD_enter_data_mode(void){ return false; }
void RNBD_set_debug(bool debug_state){}
bool RNBD_data_ready(void){ return false; }
uint8_t RNBD_read(void){ return '%'; } 
bool RNBD_is_connected(void){ return false; }

size_t RNBD_transmit_ready()
{
    return 0;
}

size_t RNBD_available_data()
{
    return 0;
}

size_t RNBD451_write(uint8_t *txData, size_t len)
{
    return 0;
}

size_t RNBD451_read(uint8_t* pRdBuffer, const size_t size)
{
    return 0;
}
#endif
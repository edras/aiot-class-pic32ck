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

#ifndef _RNBD_UART_HAL_H    /* Guard against multiple inclusion */
#define _RNBD_UART_HAL_H

#include "hal.h"

/* ***************   ATTENTION *********************************
 * For this project, pins CS, SCK, MISO, and MOSI should be removed
 * from MIKROE board, so it will not interfere with oder SPI members 
 * present on the bus
 */

#define RNBD_reset_low          RST2_Clear
#define RNBD_reset_high         RST2_Set
#define RNBD_delay_ms           TIMER_DelayMs
#define RNBD_available          1

#define RNDB_TIMEOUT            SYSTICK_TIMEOUT
#define RNDB_StartTimeOut       SYSTICK_StartTimeOut
#define RNDB_IsTimeoutReached   SYSTICK_IsTimeoutReached

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/* Arms continuous reception on the click UART. smart_hmi_same51 needs no
   equivalent: its SERCOM0 ring buffer plib receives from the moment it is
   initialised. See the comment in common_pic32ck/rnbd_uart_hal.c. */
void RNBD_uart_hal_init(void);

size_t RNBD_transmit_ready();
size_t RNBD_available_data();
size_t RNBD451_write(uint8_t *txData, size_t len);
size_t RNBD451_read(uint8_t* pRdBuffer, const size_t size);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _RNBD_UART_HAL_H */

/* *****************************************************************************
 End of File
 */

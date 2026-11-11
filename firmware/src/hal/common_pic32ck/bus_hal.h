#ifndef BUS_HAL_H
#define BUS_HAL_H

#include "definitions.h"

#define HAL_I2C_Write            SERCOM0_I2C_Write
#define HAL_I2C_WriteRead        SERCOM0_I2C_WriteRead
#define HAL_I2C_IsBusy           SERCOM0_I2C_IsBusy
#define HAL_I2C_Read             SERCOM0_I2C_Read
#define HAL_I2C_ErrorGet         SERCOM0_I2C_ErrorGet
#define HAL_I2C_CallbackRegister SERCOM0_I2C_CallbackRegister

#define HAL_SPI_Write             SERCOM4_SPI_Write
#define HAL_SPI_WriteRead         SERCOM4_SPI_WriteRead
#define HAL_SPI_IsTransmitterBusy SERCOM4_SPI_IsTransmitterBusy

/* SERCOM1 is generated as the plain non-blocking USART plib, not the ring
 * buffer variant demo_same51 uses on SERCOM0. rnbd_uart_hal.c supplies the
 * buffering the RNBD451 driver expects on top of these four primitives - see
 * the comment there. SERCOM1_USART_WriteCountGet/ReadCountGet are deliberately
 * NOT exposed: on this plib they report progress within the current transfer,
 * not buffer occupancy, so forwarding them would silently misbehave. */
#define HAL_CLICK_UART_Write                SERCOM1_USART_Write
#define HAL_CLICK_UART_WriteIsBusy          SERCOM1_USART_WriteIsBusy
#define HAL_CLICK_UART_Read                 SERCOM1_USART_Read
#define HAL_CLICK_UART_ReadCallbackRegister SERCOM1_USART_ReadCallbackRegister

/* Sensirion debug terminal UART (used only when MIKROE_SENSIRION is defined
 * by the board-specific hal.h) */
#define HAL_TERMINAL_UART_Write SERCOM5_USART_Write
#define HAL_TERMINAL_UART_Done  SERCOM5_USART_TransmitComplete

#endif

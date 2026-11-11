#include <string.h>

#include "rnbd_uart_hal.h"
#include "bus_hal.h"

/* ---------------------------------------------------------------------------
 * RNBD451 UART buffering
 *
 * demo_same51 talks to the RNBD451 over SERCOM0, generated as a Harmony USART
 * *ring buffer* plib. rnbd451.c is written against those semantics:
 *
 *   SERCOM0_USART_ReadCountGet()  - bytes already buffered and ready to take
 *   SERCOM0_USART_Read(buf, n)    - copies up to n buffered bytes, returns count
 *   SERCOM0_USART_WriteCountGet() - bytes still pending in the TX buffer
 *
 * Reception is continuous there: the plib's interrupt handler buffers every
 * byte as it arrives, whether or not the application has asked for one.
 *
 * MCC generated the plain non-blocking plib for PIC32CK's SERCOM1 instead, and
 * its identically named functions mean something different. Read() *starts* a
 * request for exactly n bytes and returns bool; ReadCountGet() reports progress
 * within the current request; and while no request is outstanding, arriving
 * bytes are simply discarded. Forwarding rnbd451.c's calls straight onto that
 * plib compiles cleanly but cannot work - RNBD_available_data() would stay 0
 * forever, uart_ble_read() would return the buffer before its byte had arrived,
 * and RNBD_transmit_ready() would latch "busy" permanently because
 * WriteCountGet() keeps returning the completed transfer's length.
 *
 * The buffering is therefore provided here rather than by switching SERCOM1 to
 * ring-buffer mode in MCC. This keeps the difference inside the HAL, where the
 * rest of the PIC32CK port already puts target-specific behaviour, and it
 * survives code regeneration. NOTE: SERCOM1 must stay in the plain
 * non-blocking mode for this file to compile - the ring buffer plib has no
 * WriteIsBusy(). If SERCOM1 is ever regenerated in ring-buffer mode, delete
 * this buffering and forward directly, as demo_same51 does.
 *
 * Re-arming the one-byte read from inside the read callback is safe with this
 * plib: SERCOM1_USART_ISR_RX_Handler() clears rxBusyStatus and rxSize before
 * invoking the callback, and SERCOM1_USART_Read()'s ErrorClear() only drains
 * the RX FIFO when an error flag is actually set, so no buffered byte is lost.
 * ------------------------------------------------------------------------- */

#define RNBD_RX_RING_SIZE   256U    /* must be a power of two */
#define RNBD_TX_BUF_SIZE    64U     /* rnbd451.c's cmdBuf is 64 bytes */

static volatile uint8_t  rx_ring[RNBD_RX_RING_SIZE];
static volatile uint16_t rx_head;       /* advanced by the ISR only  */
static volatile uint16_t rx_tail;       /* advanced by the task only */
static volatile uint8_t  rx_byte;       /* landing slot for the outstanding read */
static uint8_t           tx_buf[RNBD_TX_BUF_SIZE];
static bool              rx_running;

static void rnbd_rx_callback(uintptr_t context)
{
    uint16_t next = (uint16_t)((rx_head + 1U) & (RNBD_RX_RING_SIZE - 1U));

    (void)context;

    /* Drop the byte when the ring is full, which is what a full ring buffer
       plib does as well. */
    if (next != rx_tail)
    {
        rx_ring[rx_head] = rx_byte;
        rx_head = next;
    }

    (void)HAL_CLICK_UART_Read((void *)&rx_byte, 1);
}

/* Start continuous reception. Called from HardwareLayer_init() so that no byte
   the module sends can be missed, and defensively from the accessors below so
   that a missing init cannot silently kill the BLE path. */
void RNBD_uart_hal_init(void)
{
    if (!rx_running)
    {
        rx_running = true;
        rx_head = 0U;
        rx_tail = 0U;
        HAL_CLICK_UART_ReadCallbackRegister(rnbd_rx_callback, 0);
        (void)HAL_CLICK_UART_Read((void *)&rx_byte, 1);
    }
}

size_t RNBD_transmit_ready()
{
    /* rnbd451.c treats a non-zero return as "not ready" (uart_ble_data_ready:
       "return RNBD_transmit_ready()? false:true"), matching the ring buffer
       plib's pending-byte count. */
    return HAL_CLICK_UART_WriteIsBusy() ? 1U : 0U;
}

size_t RNBD_available_data()
{
    RNBD_uart_hal_init();

    return (size_t)((rx_head - rx_tail) & (RNBD_RX_RING_SIZE - 1U));
}

size_t RNBD451_write(uint8_t *txData, size_t len)
{
    if ((txData == NULL) || (len == 0U))
    {
        return 0U;
    }

    if (len > RNBD_TX_BUF_SIZE)
    {
        len = RNBD_TX_BUF_SIZE;
    }

    /* The plain plib transmits straight out of the caller's buffer and refuses
       a second request while one is in flight, so copy into storage that stays
       valid and let the previous command drain first. The wait is bounded by
       that command's transmission time, not by an arbitrary delay. */
    while (HAL_CLICK_UART_WriteIsBusy())
    {
        /* wait for the previous command to finish */
    }

    (void)memcpy(tx_buf, txData, len);

    return HAL_CLICK_UART_Write(tx_buf, len) ? len : 0U;
}

size_t RNBD451_read(uint8_t* pRdBuffer, const size_t size)
{
    size_t count = 0U;

    RNBD_uart_hal_init();

    if (pRdBuffer == NULL)
    {
        return 0U;
    }

    while ((count < size) && (rx_tail != rx_head))
    {
        pRdBuffer[count] = rx_ring[rx_tail];
        rx_tail = (uint16_t)((rx_tail + 1U) & (RNBD_RX_RING_SIZE - 1U));
        count++;
    }

    return count;
}

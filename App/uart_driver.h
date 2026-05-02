/**
 * @file    uart_driver.h
 * @brief   UART0 driver API for debug/status output.
 *
 * @details Provides a minimal blocking UART0 transmit interface.
 *          Reception is not used by this project.
 *
 *          Baud rate  : 115 200
 *          Data bits  : 8
 *          Parity     : None
 *          Stop bits  : 1
 *          System clock: 16 MHz
 *
 * @note    Output is available on the LaunchPad's virtual COM port
 *          (PA1 = TX, PA0 = RX).
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

/* =========================================================================
 *  Public API
 * ========================================================================= */

/**
 * @brief  Initialise UART0 for 115200-8N1 operation.
 *
 * @details Enables the UART0 and Port A peripheral clocks, configures PA0/PA1
 *          for the UART alternate function, and sets the baud rate.
 *          Must be called once before any UART_SendXxx call.
 */
void UART_Init(void);

/**
 * @brief  Transmit a single character (blocking).
 *
 * @param[in]  c  The character to transmit.
 */
void UART_SendChar(char c);

/**
 * @brief  Transmit a null-terminated string (blocking).
 *
 * @param[in]  str  Pointer to the null-terminated string to transmit.
 */
void UART_SendString(const char *str);

#endif /* UART_DRIVER_H */

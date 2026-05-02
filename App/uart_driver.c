/**
 * @file    uart_driver.c
 * @brief   UART0 driver implementation.
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#include "uart_driver.h"
#include "gate_config.h"
#include "tm4c123gh6pm.h"

/* =========================================================================
 *  Private Macros
 * ========================================================================= */

/** @brief UART0 TX FIFO full flag bit in UART0_FR_R. */
#define UART_FR_TXFF    (1UL << 5U)

/** @brief UART enable bit in UART0_CTL_R. */
#define UART_CTL_UARTEN (1UL << 0U)

/** @brief UART TX enable bit in UART0_CTL_R. */
#define UART_CTL_TXE    (1UL << 8U)

/** @brief UART RX enable bit in UART0_CTL_R. */
#define UART_CTL_RXE    (1UL << 9U)

/** @brief 8-bit word-length field in UART0_LCRH_R (WLEN = 0b11). */
#define UART_LCRH_WLEN_8 (0x3UL << 5U)

/* =========================================================================
 *  Public Function Implementations
 * ========================================================================= */

void UART_Init(void)
{
    /* Enable UART0 and Port A peripheral clocks */
    SYSCTL_RCGCUART_R |= (1UL << 0U);
    SYSCTL_RCGCGPIO_R |= CFG_RCGC_PORTA;

    /* Wait until both peripherals are ready */
    while (!(SYSCTL_PRUART_R & (1UL << 0U))) { /* spin */ }
    while (!(SYSCTL_PRGPIO_R & CFG_RCGC_PORTA)) { /* spin */ }

    /* Configure PA0 (RX) and PA1 (TX) for UART alternate function */
    GPIO_PORTA_AFSEL_R |=  0x03UL;
    GPIO_PORTA_PCTL_R   = (GPIO_PORTA_PCTL_R & ~0x000000FFUL) | 0x00000011UL;
    GPIO_PORTA_DEN_R   |=  0x03UL;
    GPIO_PORTA_AMSEL_R &= ~0x03UL;

    /* Configure UART0:
     *   System clock = 16 MHz, desired baud = 115 200
     *   BRD  = 16 000 000 / (16 * 115 200) = 8.6805...
     *   IBRD = 8
     *   FBRD = round(0.6805 * 64) = 44
     */
    UART0_CTL_R  &= ~UART_CTL_UARTEN;           /* disable during config    */
    UART0_IBRD_R  = CFG_UART_BAUD_IBRD;
    UART0_FBRD_R  = CFG_UART_BAUD_FBRD;
    UART0_LCRH_R  = UART_LCRH_WLEN_8;           /* 8-bit, no parity, 1 stop */
    UART0_CTL_R  |= UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;
}

/* -------------------------------------------------------------------------- */

void UART_SendChar(char c)
{
    /* Block until the TX FIFO has room */
    while (UART0_FR_R & UART_FR_TXFF)
    {
        /* spin */
    }
    UART0_DR_R = (uint32_t)c;
}

/* -------------------------------------------------------------------------- */

void UART_SendString(const char *str)
{
    if (str == (const char *)0)
    {
        return;
    }

    while (*str != '\0')
    {
        UART_SendChar(*str);
        str++;
    }
}

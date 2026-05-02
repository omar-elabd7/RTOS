/**
 * @file    gpio_driver.h
 * @brief   GPIO initialisation and LED control API.
 *
 * @details Provides two public functions:
 *            - GPIO_Init()     – configure all project GPIO pins once at startup.
 *            - LED_Write()     – set the Red / Green LED output state.
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include <stdint.h>

/* =========================================================================
 *  Public API
 * ========================================================================= */

/**
 * @brief  Initialise all GPIO ports used by the Smart Gate System.
 *
 * @details Enables clocks for Ports B, D, E, F (and A for UART, handled by
 *          uart_driver).  Configures:
 *            - PF1, PF3 as digital outputs (LEDs)
 *            - PF4 as digital input with pull-up (obstacle / SW1)
 *            - PE0, PE1 as digital inputs with pull-down (driver panel)
 *            - PB0, PB1 as digital inputs with pull-down (security panel)
 *            - PD0, PD1 as digital inputs with pull-down (limit switches)
 *
 * @note   Must be called once before the FreeRTOS scheduler starts.
 */
void GPIO_Init(void);

/**
 * @brief  Write the desired LED output state.
 *
 * @details Only the bits corresponding to CFG_LED_MASK (PF1 and PF3) are
 *          affected; all other Port F bits are preserved.
 *
 * @param[in]  mask  Bitmask built from CFG_LED_RED and/or CFG_LED_GREEN.
 *                   Pass 0 to turn both LEDs off.
 */
void LED_Write(uint32_t mask);

#endif /* GPIO_DRIVER_H */

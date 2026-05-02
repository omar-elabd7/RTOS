/**
 * @file    gate_config.h
 * @brief   Hardware pin definitions and system-wide configuration constants.
 *
 * @details This is the single point of truth for all hardware mappings and
 *          tunable parameters.  Changing a pin assignment or timing value
 *          here propagates automatically to every module that includes this
 *          header.
 *
 *  Hardware Map (TM4C123GH6PM LaunchPad)
 *  ──────────────────────────────────────────────────────────────────────
 *  Pin   │ Role                      │ Pull    │ Active
 *  ──────┼───────────────────────────┼─────────┼────────
 *  PF1   │ Red  LED  (CLOSING)       │ –       │ HIGH
 *  PF3   │ Green LED (OPENING/REV)   │ –       │ HIGH
 *  PF4   │ Obstacle button (SW1)     │ Pull-up │ LOW
 *  PE0   │ Driver   OPEN  button     │ Pull-up │ LOW
 *  PE1   │ Driver   CLOSE button     │ Pull-up │ LOW
 *  PB0   │ Security OPEN  button     │ Pull-up │ LOW
 *  PB1   │ Security CLOSE button     │ Pull-up │ LOW
 *  PD0   │ Open  Limit button        │ Pull-up │ LOW
 *  PD1   │ Close Limit button        │ Pull-up │ LOW
 *  PA0   │ UART0 RX                  │ –       │ –
 *  PA1   │ UART0 TX                  │ –       │ –
 *  ──────────────────────────────────────────────────────────────────────
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#ifndef GATE_CONFIG_H
#define GATE_CONFIG_H

#include <stdint.h>

/* =========================================================================
 *  GPIO Clock-Enable Masks (SYSCTL_RCGCGPIO_R)
 * ========================================================================= */
#define CFG_RCGC_PORTA     (1UL << 0U)
#define CFG_RCGC_PORTB     (1UL << 1U)
#define CFG_RCGC_PORTD     (1UL << 3U)
#define CFG_RCGC_PORTE     (1UL << 4U)
#define CFG_RCGC_PORTF     (1UL << 5U)

/* =========================================================================
 *  Port F – LEDs and Obstacle Button
 * ========================================================================= */
#define CFG_LED_RED        (1UL << 1U)   /**< PF1 – Red  LED  (gate CLOSING)  */
#define CFG_LED_GREEN      (1UL << 3U)   /**< PF3 – Green LED (gate OPENING)  */
#define CFG_LED_MASK       (CFG_LED_RED | CFG_LED_GREEN)

#define CFG_BTN_OBSTACLE   (1UL << 4U)   /**< PF4 – Obstacle (pull-up, act-LOW) */

/* =========================================================================
 *  Port E – Driver Panel Buttons  (pull-up, active-LOW)
 * ========================================================================= */
#define CFG_BTN_DRV_OPEN   (1UL << 0U)   /**< PE0 – Driver OPEN  */
#define CFG_BTN_DRV_CLOSE  (1UL << 1U)   /**< PE1 – Driver CLOSE */

/* =========================================================================
 *  Port B – Security Panel Buttons  (pull-up, active-LOW)
 * ========================================================================= */
#define CFG_BTN_SEC_OPEN   (1UL << 0U)   /**< PB0 – Security OPEN  */
#define CFG_BTN_SEC_CLOSE  (1UL << 1U)   /**< PB1 – Security CLOSE */

/* =========================================================================
 *  Port D – Limit Buttons  (pull-up, active-LOW)
 * ========================================================================= */
#define CFG_BTN_OPEN_LIM   (1UL << 0U)   /**< PD0 – Open  Limit */
#define CFG_BTN_CLOSE_LIM  (1UL << 1U)   /**< PD1 – Close Limit */

/* =========================================================================
 *  UART0 Configuration  (PA0 = RX, PA1 = TX)
 * ========================================================================= */
#define CFG_UART_BAUD_IBRD   8U    /**< Integer   BRD for 115200 @ 16 MHz    */
#define CFG_UART_BAUD_FBRD  44U    /**< Fractional BRD for 115200 @ 16 MHz   */

/* =========================================================================
 *  Timing Parameters
 * ========================================================================= */

/** @brief Input debounce interval in milliseconds. */
#define CFG_DEBOUNCE_MS         20U

/** @brief Safety task polling interval in milliseconds. */
#define CFG_SAFETY_POLL_MS       5U

/** @brief LED / LED-control task polling interval in milliseconds. */
#define CFG_LED_POLL_MS         10U

/** @brief Status task polling interval in milliseconds. */
#define CFG_STATUS_POLL_MS     200U

/**
 * @brief  Manual-vs-Auto mode threshold in milliseconds.
 *
 * @details If a command button is released within this time it is treated as a
 *          one-touch "auto" press (gate runs to limit).  If held longer it is
 *          treated as a "manual" press (gate stops on release).
 */
#define CFG_AUTO_THRESHOLD_MS  500U

/** @brief Duration (ms) of the obstacle-triggered reverse movement. */
#define CFG_REVERSE_DURATION_MS 500U

/* =========================================================================
 *  FreeRTOS Task Priorities
 *  (higher number = higher priority)
 * ========================================================================= */
#define CFG_PRIO_SAFETY_TASK      4U   /**< Highest – safety-critical         */
#define CFG_PRIO_INPUT_TASK       3U   /**< High    – button capture          */
#define CFG_PRIO_GATE_CTRL_TASK   2U   /**< Medium  – FSM                     */
#define CFG_PRIO_LED_TASK         2U   /**< Medium  – LED output              */
#define CFG_PRIO_STATUS_TASK      1U   /**< Low     – UART display            */

/* =========================================================================
 *  FreeRTOS Task Stack Depths  (words)
 * ========================================================================= */
#define CFG_STACK_SAFETY_TASK    256U
#define CFG_STACK_INPUT_TASK     256U
#define CFG_STACK_GATE_CTRL_TASK 512U
#define CFG_STACK_LED_TASK       128U
#define CFG_STACK_STATUS_TASK    256U

/* =========================================================================
 *  Button Queue Depth
 * ========================================================================= */
#define CFG_BTN_QUEUE_DEPTH      20U

#endif /* GATE_CONFIG_H */

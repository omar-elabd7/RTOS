/**
 * @file    led_task.h
 * @brief   LED Control Task – reflects the gate state on the physical LEDs.
 *
 * @details Monitors gGateState and updates the Red / Green LEDs:
 *            OPENING / REVERSING  -> Green LED ON,  Red LED OFF
 *            CLOSING              -> Red   LED ON,  Green LED OFF
 *            All other states     -> Both  LEDs OFF
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#ifndef LED_TASK_H
#define LED_TASK_H

/* =========================================================================
 *  Public API
 * ========================================================================= */

/**
 * @brief  FreeRTOS task function for the LED Control Task.
 *
 * @details Create this task via xTaskCreate() in main.c.
 *          Recommended priority : CFG_PRIO_LED_TASK (Medium).
 *          Recommended stack    : CFG_STACK_LED_TASK words.
 *
 * @param[in]  pvParameters  Unused (pass NULL).
 */
void vLEDControlTask(void *pvParameters);

#endif /* LED_TASK_H */

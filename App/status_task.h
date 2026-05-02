/**
 * @file    status_task.h
 * @brief   Status Task – UART state-change reporter.
 *
 * @details The lowest-priority task; prints the gate state name over UART0
 *          (115200-8N1) whenever the state changes.  Output is visible on
 *          the LaunchPad virtual COM port.
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#ifndef STATUS_TASK_H
#define STATUS_TASK_H

/* =========================================================================
 *  Public API
 * ========================================================================= */

/**
 * @brief  FreeRTOS task function for the Status Task.
 *
 * @details Create this task via xTaskCreate() in main.c.
 *          Recommended priority : CFG_PRIO_STATUS_TASK (Low).
 *          Recommended stack    : CFG_STACK_STATUS_TASK words.
 *
 * @param[in]  pvParameters  Unused (pass NULL).
 */
void vStatusTask(void *pvParameters);

#endif /* STATUS_TASK_H */

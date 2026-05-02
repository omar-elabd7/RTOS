/**
 * @file    input_task.h
 * @brief   Input Task – button polling, debouncing, and event dispatch.
 *
 * @details The Input Task samples all nine push buttons at a fixed 20 ms
 *          interval, detects rising and falling edges, and posts the
 *          corresponding ButtonEvent_t items to xButtonQueue.
 *
 *          Rising edges on the limit-switch pins additionally give the
 *          respective binary semaphores (xOpenLimitSem / xCloseLimitSem)
 *          for tasks that need direct synchronisation.
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#ifndef INPUT_TASK_H
#define INPUT_TASK_H

/* =========================================================================
 *  Public API
 * ========================================================================= */

/**
 * @brief  FreeRTOS task function for the Input Task.
 *
 * @details Create this task via xTaskCreate() in main.c.
 *          Recommended priority : CFG_PRIO_INPUT_TASK (High).
 *          Recommended stack    : CFG_STACK_INPUT_TASK words.
 *
 * @param[in]  pvParameters  Unused (pass NULL).
 */
void vInputTask(void *pvParameters);

#endif /* INPUT_TASK_H */

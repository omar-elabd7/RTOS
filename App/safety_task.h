/**
 * @file    safety_task.h
 * @brief   Safety Task – highest-priority real-time safety monitor.
 *
 * @details Enforces three safety behaviours at hardware-polling speed
 *          (every CFG_SAFETY_POLL_MS milliseconds):
 *
 *            1. Obstacle during CLOSING:
 *                  a) Immediately switch to GATE_REVERSING (Green LED ON).
 *                  b) Wait CFG_REVERSE_DURATION_MS.
 *                  c) Transition to GATE_STOPPED_MIDWAY.
 *
 *            2. Same-panel conflict (OPEN + CLOSE held simultaneously):
 *                  -> Immediately force GATE_STOPPED_MIDWAY.
 *
 *            3. Cross-panel security priority:
 *                  Security CLOSE + Driver OPEN  active -> force GATE_CLOSING.
 *                  Security OPEN  + Driver CLOSE active -> force GATE_OPENING.
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#ifndef SAFETY_TASK_H
#define SAFETY_TASK_H

/* =========================================================================
 *  Public API
 * ========================================================================= */

/**
 * @brief  FreeRTOS task function for the Safety Task.
 *
 * @details Create this task via xTaskCreate() in main.c.
 *          Recommended priority : CFG_PRIO_SAFETY_TASK (Highest).
 *          Recommended stack    : CFG_STACK_SAFETY_TASK words.
 *
 * @param[in]  pvParameters  Unused (pass NULL).
 */
void vSafetyTask(void *pvParameters);

#endif /* SAFETY_TASK_H */

/**
 * @file    gate_control_task.h
 * @brief   Gate Control Task – finite state machine (FSM) driver.
 *
 * @details Receives ButtonEvent_t items from xButtonQueue and implements the
 *          full gate state machine including:
 *            - Manual mode  (hold button -> gate stops on release)
 *            - Auto mode    (tap button  -> gate runs to limit)
 *            - Security-panel priority over driver panel
 *            - Same-panel conflict detection (OPEN + CLOSE -> safe stop)
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#ifndef GATE_CONTROL_TASK_H
#define GATE_CONTROL_TASK_H

/* =========================================================================
 *  Public API
 * ========================================================================= */

/**
 * @brief  FreeRTOS task function for the Gate Control Task.
 *
 * @details Create this task via xTaskCreate() in main.c.
 *          Recommended priority : CFG_PRIO_GATE_CTRL_TASK (Medium).
 *          Recommended stack    : CFG_STACK_GATE_CTRL_TASK words.
 *
 * @param[in]  pvParameters  Unused (pass NULL).
 */
void vGateControlTask(void *pvParameters);

#endif /* GATE_CONTROL_TASK_H */

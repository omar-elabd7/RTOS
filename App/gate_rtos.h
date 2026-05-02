/**
 * @file    gate_rtos.h
 * @brief   Shared FreeRTOS object handles and thread-safe gate state API.
 *
 * @details Declares the application-wide RTOS handles (queue, semaphores,
 *          mutex) and the two accessor functions that every task uses to
 *          read or write the shared gate state variable.
 *
 *          All handles are allocated in gate_rtos.c and are extern here so
 *          that every .c file that needs them only needs to include this
 *          single header.
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#ifndef GATE_RTOS_H
#define GATE_RTOS_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "gate_types.h"

/* =========================================================================
 *  Shared RTOS Object Handles  (defined in gate_rtos.c)
 * ========================================================================= */

/**
 * @brief  Button-event queue.
 *
 * @details The Input Task posts ButtonEvent_t values into this queue.
 *          The Gate Control Task receives from it.
 *          Depth is defined by CFG_BTN_QUEUE_DEPTH.
 */
extern QueueHandle_t xButtonQueue;

/**
 * @brief  Binary semaphore – signalled when the Open Limit button fires.
 *
 * @details Given by the Input Task on a rising edge of PD0.
 *          Can also be taken by any task that needs to synchronise on the
 *          gate reaching the fully-open position.
 */
extern SemaphoreHandle_t xOpenLimitSem;

/**
 * @brief  Binary semaphore – signalled when the Close Limit button fires.
 *
 * @details Given by the Input Task on a rising edge of PD1.
 */
extern SemaphoreHandle_t xCloseLimitSem;

/**
 * @brief  Mutex protecting the shared gGateState variable.
 *
 * @details All reads and writes to gGateState MUST go through
 *          GetGateState() / SetGateState() which take this mutex internally.
 */
extern SemaphoreHandle_t xGateStateMutex;

/* =========================================================================
 *  RTOS Object Initialisation
 * ========================================================================= */

/**
 * @brief  Create all shared FreeRTOS objects.
 *
 * @details Must be called once from main() BEFORE creating any tasks.
 *          Uses configASSERT() to halt on allocation failure.
 */
void GateRTOS_Init(void);

/* =========================================================================
 *  Thread-Safe Gate State Accessors
 * ========================================================================= */

/**
 * @brief  Read the current gate state under mutex protection.
 *
 * @return The current GateState_t value.
 */
GateState_t GetGateState(void);

/**
 * @brief  Write a new gate state under mutex protection.
 *
 * @param[in]  newState  The state to transition to.
 */
void SetGateState(GateState_t newState);

#endif /* GATE_RTOS_H */

/**
 * @file    gate_rtos.c
 * @brief   Shared FreeRTOS object definitions and thread-safe state accessors.
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#include "gate_rtos.h"
#include "gate_config.h"
#include "gate_types.h"

/* =========================================================================
 *  Shared RTOS Object Handles  (extern-declared in gate_rtos.h)
 * ========================================================================= */

QueueHandle_t     xButtonQueue    = NULL;
SemaphoreHandle_t xOpenLimitSem   = NULL;
SemaphoreHandle_t xCloseLimitSem  = NULL;
SemaphoreHandle_t xGateStateMutex = NULL;

/* =========================================================================
 *  Private: Shared Gate State Variable
 *
 *  Declared static so no external code can access it directly.
 *  All access goes through GetGateState() / SetGateState().
 * ========================================================================= */

static volatile GateState_t gGateState = GATE_IDLE_CLOSED;

/* =========================================================================
 *  Public Function Implementations
 * ========================================================================= */

void GateRTOS_Init(void)
{
    xButtonQueue    = xQueueCreate(CFG_BTN_QUEUE_DEPTH, sizeof(ButtonEvent_t));
    xOpenLimitSem   = xSemaphoreCreateBinary();
    xCloseLimitSem  = xSemaphoreCreateBinary();
    xGateStateMutex = xSemaphoreCreateMutex();

    /* Halt in debug builds if any allocation fails */
    configASSERT(xButtonQueue    != NULL);
    configASSERT(xOpenLimitSem   != NULL);
    configASSERT(xCloseLimitSem  != NULL);
    configASSERT(xGateStateMutex != NULL);
}

/* -------------------------------------------------------------------------- */

GateState_t GetGateState(void)
{
    GateState_t state;

    xSemaphoreTake(xGateStateMutex, portMAX_DELAY);
    state = gGateState;
    xSemaphoreGive(xGateStateMutex);

    return state;
}

/* -------------------------------------------------------------------------- */

void SetGateState(GateState_t newState)
{
    xSemaphoreTake(xGateStateMutex, portMAX_DELAY);
    gGateState = newState;
    xSemaphoreGive(xGateStateMutex);
}

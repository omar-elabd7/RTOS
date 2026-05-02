/**
 * @file    safety_task.c
 * @brief   Safety Task implementation.
 *
 * @details Runs at the highest application priority to ensure that safety
 *          conditions are enforced faster than any other task can react.
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#include "safety_task.h"
#include "gate_config.h"
#include "gate_rtos.h"
#include "gate_types.h"
#include "tm4c123gh6pm.h"

#include "FreeRTOS.h"
#include "task.h"

/* =========================================================================
 *  Private Helpers
 * ========================================================================= */

/**
 * @brief  Read the obstacle button (PF4, active-LOW, pull-up).
 *
 * @return 1 if obstacle is detected, 0 otherwise.
 */
static inline uint8_t ObstacleDetected(void)
{
    /* PF4 pull-up, active-LOW: pin LOW means obstacle present */
    return (GPIO_PORTF_DATA_R & CFG_BTN_OBSTACLE) ? 0U : 1U;
}

/**
 * @brief  Handle the obstacle-detection safety sequence.
 *
 * @details  1. Reverse (GATE_REVERSING) for CFG_REVERSE_DURATION_MS.
 *           2. Stop completely (GATE_STOPPED_MIDWAY).
 */
static void HandleObstacle(void)
{
    SetGateState(GATE_REVERSING);
    vTaskDelay(pdMS_TO_TICKS(CFG_REVERSE_DURATION_MS));
    SetGateState(GATE_STOPPED_MIDWAY);
}

/**
 * @brief  Read an active-LOW button pin as a logical 1/0 (1 = pressed).
 *         All buttons use pull-up resistors; pressed = pin driven LOW.
 */
static inline uint8_t BtnPressed(uint32_t reg, uint32_t mask)
{
    return (reg & mask) ? 0U : 1U;
}

/**
 * @brief  Enforce same-panel conflict rule.
 *
 * @details If OPEN and CLOSE are pressed simultaneously on the driver panel
 *          OR on the security panel, stop the gate immediately.
 */
static void EnforceConflictStop(GateState_t state)
{
    uint8_t drvOpen  = BtnPressed(GPIO_PORTE_DATA_R, CFG_BTN_DRV_OPEN);
    uint8_t drvClose = BtnPressed(GPIO_PORTE_DATA_R, CFG_BTN_DRV_CLOSE);
    uint8_t secOpen  = BtnPressed(GPIO_PORTB_DATA_R, CFG_BTN_SEC_OPEN);
    uint8_t secClose = BtnPressed(GPIO_PORTB_DATA_R, CFG_BTN_SEC_CLOSE);

    if ((drvOpen && drvClose) || (secOpen && secClose))
    {
        if (state == GATE_OPENING || state == GATE_CLOSING)
        {
            SetGateState(GATE_STOPPED_MIDWAY);
        }
    }
}

/**
 * @brief  Enforce cross-panel security priority.
 *
 * @details Security panel always wins when both panels are active at the
 *          same time.
 */
static void EnforceSecurityPriority(GateState_t state)
{
    uint8_t drvOpen  = BtnPressed(GPIO_PORTE_DATA_R, CFG_BTN_DRV_OPEN);
    uint8_t drvClose = BtnPressed(GPIO_PORTE_DATA_R, CFG_BTN_DRV_CLOSE);
    uint8_t secOpen  = BtnPressed(GPIO_PORTB_DATA_R, CFG_BTN_SEC_OPEN);
    uint8_t secClose = BtnPressed(GPIO_PORTB_DATA_R, CFG_BTN_SEC_CLOSE);

    /* Security CLOSE overrides Driver OPEN */
    if (secClose && drvOpen && (state == GATE_OPENING))
    {
        SetGateState(GATE_CLOSING);
    }

    /* Security OPEN overrides Driver CLOSE */
    if (secOpen && drvClose && (state == GATE_CLOSING))
    {
        SetGateState(GATE_OPENING);
    }
}

/* =========================================================================
 *  Task Implementation
 * ========================================================================= */

void vSafetyTask(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        GateState_t state = GetGateState();

        /* ---------------------------------------------------------------- */
        /* 1. Obstacle protection  (only active while gate is CLOSING)      */
        /* ---------------------------------------------------------------- */
        if (ObstacleDetected() && (state == GATE_CLOSING))
        {
            HandleObstacle();
            /* After HandleObstacle() the state is STOPPED_MIDWAY;
             * skip the remaining checks this cycle.                         */
            vTaskDelay(pdMS_TO_TICKS(CFG_SAFETY_POLL_MS));
            continue;
        }

        /* ---------------------------------------------------------------- */
        /* 2. Same-panel conflict detection                                  */
        /* ---------------------------------------------------------------- */
        EnforceConflictStop(state);

        /* ---------------------------------------------------------------- */
        /* 3. Cross-panel security priority                                  */
        /* ---------------------------------------------------------------- */
        state = GetGateState();  /* re-read in case step 2 changed it       */
        EnforceSecurityPriority(state);

        vTaskDelay(pdMS_TO_TICKS(CFG_SAFETY_POLL_MS));
    }
}

/**
 * @file    gate_control_task.c
 * @brief   Gate Control Task – finite state machine (FSM) implementation.
 *
 * @details Blocks on xButtonQueue and processes each ButtonEvent_t through
 *          the gate FSM.
 *
 *  Manual vs Auto Mode
 *  ───────────────────
 *  A "press" event starts a timer.  When the corresponding "release" event
 *  arrives the elapsed time is compared against CFG_AUTO_THRESHOLD_MS:
 *    < threshold : one-touch auto mode  – gate continues to the limit.
 *    >= threshold: manual hold mode     – gate stops on release.
 *
 *  Security Priority
 *  ─────────────────
 *  Security-panel button events override driver-panel events.  If a
 *  security CLOSE is currently held, driver OPEN events are silently
 *  discarded, and vice-versa.  The Safety Task also enforces this at the
 *  hardware level for added redundancy.
 *
 *  Conflict Handling
 *  ─────────────────
 *  If OPEN and CLOSE buttons on the SAME panel are detected simultaneously
 *  (checked at the end of every event cycle) the gate is stopped.
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#include "gate_control_task.h"
#include "gate_config.h"
#include "gate_rtos.h"
#include "gate_types.h"
#include "tm4c123gh6pm.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* =========================================================================
 *  Private Types
 * ========================================================================= */

/**
 * @brief  Internal context block for the Gate Control Task.
 *
 * @details All mutable state is grouped here to keep the task function clean.
 */
typedef struct
{
    TickType_t pressStartTick;   /**< Tick count when current button was pressed */
    uint8_t    openPending;      /**< 1 = waiting to assess an OPEN release      */
    uint8_t    closePending;     /**< 1 = waiting to assess a CLOSE release      */
    uint8_t    autoModeOpen;     /**< 1 = gate runs autonomously to open  limit  */
    uint8_t    autoModeClose;    /**< 1 = gate runs autonomously to close limit  */
    uint8_t    secOpenHeld;      /**< 1 = security OPEN  button currently held   */
    uint8_t    secCloseHeld;     /**< 1 = security CLOSE button currently held   */
} GateCtrlCtx_t;

/* =========================================================================
 *  Private Helpers
 * ========================================================================= */

/**
 * @brief  Calculate how long (ms) a button has been held since pressStartTick.
 */
static uint32_t HeldTime_ms(TickType_t pressStart)
{
    return (uint32_t)((xTaskGetTickCount() - pressStart) * portTICK_PERIOD_MS);
}

/**
 * @brief  Start an OPEN movement if the gate is not already opening.
 */
static void StartOpening(GateCtrlCtx_t *ctx)
{
    if (GetGateState() != GATE_OPENING)
    {
        SetGateState(GATE_OPENING);
    }
    ctx->pressStartTick = xTaskGetTickCount();
    ctx->openPending    = 1U;
    ctx->closePending   = 0U;
    ctx->autoModeOpen   = 0U;
    ctx->autoModeClose  = 0U;
}

/**
 * @brief  Start a CLOSE movement if the gate is not already closing.
 */
static void StartClosing(GateCtrlCtx_t *ctx)
{
    if (GetGateState() != GATE_CLOSING)
    {
        SetGateState(GATE_CLOSING);
    }
    ctx->pressStartTick = xTaskGetTickCount();
    ctx->closePending   = 1U;
    ctx->openPending    = 0U;
    ctx->autoModeClose  = 0U;
    ctx->autoModeOpen   = 0U;
}

/**
 * @brief  Evaluate OPEN-button release: decide auto vs manual.
 */
static void EvaluateOpenRelease(GateCtrlCtx_t *ctx)
{
    if (!ctx->openPending)
    {
        return;
    }

    if (HeldTime_ms(ctx->pressStartTick) < CFG_AUTO_THRESHOLD_MS)
    {
        ctx->autoModeOpen = 1U;  /* continue to open limit */
    }
    else
    {
        ctx->autoModeOpen = 0U;
        if (GetGateState() == GATE_OPENING)
        {
            SetGateState(GATE_STOPPED_MIDWAY);
        }
    }
    ctx->openPending = 0U;
}

/**
 * @brief  Evaluate CLOSE-button release: decide auto vs manual.
 */
static void EvaluateCloseRelease(GateCtrlCtx_t *ctx)
{
    if (!ctx->closePending)
    {
        return;
    }

    if (HeldTime_ms(ctx->pressStartTick) < CFG_AUTO_THRESHOLD_MS)
    {
        ctx->autoModeClose = 1U;  /* continue to close limit */
    }
    else
    {
        ctx->autoModeClose = 0U;
        if (GetGateState() == GATE_CLOSING)
        {
            SetGateState(GATE_STOPPED_MIDWAY);
        }
    }
    ctx->closePending = 0U;
}

/**
 * @brief  Check for same-panel conflict (OPEN + CLOSE simultaneously).
 *
 * @details Called after every event to guard against simultaneous button
 *          presses on the same panel.  All buttons are pull-up, active-LOW.
 */
static void CheckConflict(GateCtrlCtx_t *ctx)
{
    /* Pull-up active-LOW: pin LOW (0) = pressed → invert the test */
    uint8_t drvOpen  = (GPIO_PORTE_DATA_R & CFG_BTN_DRV_OPEN)  ? 0U : 1U;
    uint8_t drvClose = (GPIO_PORTE_DATA_R & CFG_BTN_DRV_CLOSE) ? 0U : 1U;
    uint8_t secOpen  = (GPIO_PORTB_DATA_R & CFG_BTN_SEC_OPEN)  ? 0U : 1U;
    uint8_t secClose = (GPIO_PORTB_DATA_R & CFG_BTN_SEC_CLOSE) ? 0U : 1U;

    if ((drvOpen && drvClose) || (secOpen && secClose))
    {
        GateState_t state = GetGateState();
        if (state == GATE_OPENING || state == GATE_CLOSING)
        {
            SetGateState(GATE_STOPPED_MIDWAY);
            ctx->autoModeOpen  = 0U;
            ctx->autoModeClose = 0U;
            ctx->openPending   = 0U;
            ctx->closePending  = 0U;
        }
    }
}

/* =========================================================================
 *  Task Implementation
 * ========================================================================= */

void vGateControlTask(void *pvParameters)
{
    (void)pvParameters;

    GateCtrlCtx_t ctx =
    {
        .pressStartTick = 0U,
        .openPending    = 0U,
        .closePending   = 0U,
        .autoModeOpen   = 0U,
        .autoModeClose  = 0U,
        .secOpenHeld    = 0U,
        .secCloseHeld   = 0U
    };

    ButtonEvent_t evt;

    for (;;)
    {
        /* Block until an event is available */
        if (xQueueReceive(xButtonQueue, &evt, portMAX_DELAY) != pdTRUE)
        {
            continue;
        }

        switch (evt)
        {
            /* ============================================================ */
            /* OPEN commands                                                  */
            /* ============================================================ */

            case EVT_SEC_OPEN_PRESSED:
                ctx.secOpenHeld = 1U;
                StartOpening(&ctx);
                break;

            case EVT_DRV_OPEN_PRESSED:
                /* Ignored if security CLOSE is currently held */
                if (!ctx.secCloseHeld)
                {
                    StartOpening(&ctx);
                }
                break;

            case EVT_SEC_OPEN_RELEASED:
                ctx.secOpenHeld = 0U;
                EvaluateOpenRelease(&ctx);
                break;

            case EVT_DRV_OPEN_RELEASED:
                /* Ignored if security CLOSE is currently held */
                if (!ctx.secCloseHeld)
                {
                    EvaluateOpenRelease(&ctx);
                }
                break;

            /* ============================================================ */
            /* CLOSE commands                                                 */
            /* ============================================================ */

            case EVT_SEC_CLOSE_PRESSED:
                ctx.secCloseHeld = 1U;
                StartClosing(&ctx);
                break;

            case EVT_DRV_CLOSE_PRESSED:
                /* Ignored if security OPEN is currently held */
                if (!ctx.secOpenHeld)
                {
                    StartClosing(&ctx);
                }
                break;

            case EVT_SEC_CLOSE_RELEASED:
                ctx.secCloseHeld = 0U;
                EvaluateCloseRelease(&ctx);
                break;

            case EVT_DRV_CLOSE_RELEASED:
                /* Ignored if security OPEN is currently held */
                if (!ctx.secOpenHeld)
                {
                    EvaluateCloseRelease(&ctx);
                }
                break;

            /* ============================================================ */
            /* Limit switches                                                 */
            /* ============================================================ */

            case EVT_OPEN_LIMIT:
            {
                GateState_t state = GetGateState();
                if (state == GATE_OPENING || state == GATE_REVERSING)
                {
                    SetGateState(GATE_IDLE_OPEN);
                    ctx.autoModeOpen  = 0U;
                    ctx.autoModeClose = 0U;
                    ctx.openPending   = 0U;
                }
                break;
            }

            case EVT_CLOSE_LIMIT:
            {
                GateState_t state = GetGateState();
                if (state == GATE_CLOSING)
                {
                    SetGateState(GATE_IDLE_CLOSED);
                    ctx.autoModeOpen  = 0U;
                    ctx.autoModeClose = 0U;
                    ctx.closePending  = 0U;
                }
                break;
            }

            /* ============================================================ */
            /* Obstacle                                                       */
            /* Safety Task manages the REVERSING sequence; here we only      */
            /* cancel any pending auto-close so the gate does not resume.    */
            /* ============================================================ */

            case EVT_OBSTACLE:
                ctx.autoModeClose = 0U;
                ctx.closePending  = 0U;
                break;

            default:
                /* Unknown event – silently discard */
                break;
        }

        /* Check for same-panel conflict after every event */
        CheckConflict(&ctx);

        /* Suppress unused-variable warnings for tracking flags */
        (void)ctx.autoModeOpen;
        (void)ctx.autoModeClose;
    }
}

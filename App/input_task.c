/**
 * @file    input_task.c
 * @brief   Input Task implementation.
 *
 * @details Polls all nine push buttons every CFG_DEBOUNCE_MS milliseconds.
 *          Detects rising / falling edges and posts ButtonEvent_t items to
 *          xButtonQueue.  Also gives the limit semaphores on rising edges
 *          of the physical limit-switch pins.
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#include "input_task.h"
#include "gate_config.h"
#include "gate_rtos.h"
#include "gate_types.h"
#include "tm4c123gh6pm.h"

#include "FreeRTOS.h"
#include "task.h"

/* =========================================================================
 *  Private Types
 * ========================================================================= */

/**
 * @brief  Compact state tracker for a single button.
 */
typedef struct
{
    uint8_t prevState;   /**< Debounced state from the previous cycle (0 or 1) */
} BtnState_t;

/* =========================================================================
 *  Private Helper: read a single pin as 0 or 1
 * ========================================================================= */

/** @brief Read an active-HIGH pin from a 32-bit GPIO data register. */
static inline uint8_t ReadActiveHigh(uint32_t reg, uint32_t mask)
{
    return (reg & mask) ? 1U : 0U;
}

/** @brief Read an active-LOW pin from a 32-bit GPIO data register. */
static inline uint8_t ReadActiveLow(uint32_t reg, uint32_t mask)
{
    return (reg & mask) ? 0U : 1U;
}

/* =========================================================================
 *  Private Helper: post an event to the queue (non-blocking)
 * ========================================================================= */
static inline void PostEvent(ButtonEvent_t evt)
{
    /* Use timeout = 0: if the queue is full we drop the event rather than
     * blocking a high-priority task.  The queue depth (CFG_BTN_QUEUE_DEPTH)
     * should be large enough to avoid this under normal operation.           */
    (void)xQueueSend(xButtonQueue, &evt, 0U);
}

/* =========================================================================
 *  Task Implementation
 * ========================================================================= */

void vInputTask(void *pvParameters)
{
    (void)pvParameters;

    /* Per-button previous state (all start as released = 0) */
    BtnState_t drvOpen   = {0U};
    BtnState_t drvClose  = {0U};
    BtnState_t secOpen   = {0U};
    BtnState_t secClose  = {0U};
    BtnState_t openLim   = {0U};
    BtnState_t closeLim  = {0U};
    BtnState_t obstacle  = {0U};

    for (;;)
    {
        /* ---------------------------------------------------------------- */
        /* Sample all raw pin states  (all buttons: pull-up, active-LOW)    */
        /* ---------------------------------------------------------------- */
        uint8_t curDrvOpen  = ReadActiveLow(GPIO_PORTE_DATA_R, CFG_BTN_DRV_OPEN);
        uint8_t curDrvClose = ReadActiveLow(GPIO_PORTE_DATA_R, CFG_BTN_DRV_CLOSE);
        uint8_t curSecOpen  = ReadActiveLow(GPIO_PORTB_DATA_R, CFG_BTN_SEC_OPEN);
        uint8_t curSecClose = ReadActiveLow(GPIO_PORTB_DATA_R, CFG_BTN_SEC_CLOSE);
        uint8_t curOpenLim  = ReadActiveLow(GPIO_PORTD_DATA_R, CFG_BTN_OPEN_LIM);
        uint8_t curCloseLim = ReadActiveLow(GPIO_PORTD_DATA_R, CFG_BTN_CLOSE_LIM);
        uint8_t curObstacle = ReadActiveLow(GPIO_PORTF_DATA_R, CFG_BTN_OBSTACLE);

        /* ---------------------------------------------------------------- */
        /* Edge detection and event dispatch                                 */
        /* ---------------------------------------------------------------- */

        /* -- Driver OPEN -- */
        if (curDrvOpen && !drvOpen.prevState)
            PostEvent(EVT_DRV_OPEN_PRESSED);
        else if (!curDrvOpen && drvOpen.prevState)
            PostEvent(EVT_DRV_OPEN_RELEASED);

        /* -- Driver CLOSE -- */
        if (curDrvClose && !drvClose.prevState)
            PostEvent(EVT_DRV_CLOSE_PRESSED);
        else if (!curDrvClose && drvClose.prevState)
            PostEvent(EVT_DRV_CLOSE_RELEASED);

        /* -- Security OPEN -- */
        if (curSecOpen && !secOpen.prevState)
            PostEvent(EVT_SEC_OPEN_PRESSED);
        else if (!curSecOpen && secOpen.prevState)
            PostEvent(EVT_SEC_OPEN_RELEASED);

        /* -- Security CLOSE -- */
        if (curSecClose && !secClose.prevState)
            PostEvent(EVT_SEC_CLOSE_PRESSED);
        else if (!curSecClose && secClose.prevState)
            PostEvent(EVT_SEC_CLOSE_RELEASED);

        /* -- Open Limit (rising edge only) -- */
        if (curOpenLim && !openLim.prevState)
        {
            PostEvent(EVT_OPEN_LIMIT);
            xSemaphoreGive(xOpenLimitSem);   /* direct-to-task signal */
        }

        /* -- Close Limit (rising edge only) -- */
        if (curCloseLim && !closeLim.prevState)
        {
            PostEvent(EVT_CLOSE_LIMIT);
            xSemaphoreGive(xCloseLimitSem);  /* direct-to-task signal */
        }

        /* -- Obstacle (rising edge only) -- */
        if (curObstacle && !obstacle.prevState)
            PostEvent(EVT_OBSTACLE);

        /* ---------------------------------------------------------------- */
        /* Save current states for the next cycle                           */
        /* ---------------------------------------------------------------- */
        drvOpen.prevState  = curDrvOpen;
        drvClose.prevState = curDrvClose;
        secOpen.prevState  = curSecOpen;
        secClose.prevState = curSecClose;
        openLim.prevState  = curOpenLim;
        closeLim.prevState = curCloseLim;
        obstacle.prevState = curObstacle;

        vTaskDelay(pdMS_TO_TICKS(CFG_DEBOUNCE_MS));
    }
}

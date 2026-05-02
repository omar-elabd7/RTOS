/**
 * @file    status_task.c
 * @brief   Status Task implementation.
 *
 * @details Initialises UART0 then polls gGateState every
 *          CFG_STATUS_POLL_MS milliseconds.  Transmits the new state name
 *          to the serial terminal only when the state has changed.
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#include "status_task.h"
#include "gate_config.h"
#include "gate_rtos.h"
#include "gate_types.h"
#include "uart_driver.h"

#include "FreeRTOS.h"
#include "task.h"

/* =========================================================================
 *  Private Constants
 * ========================================================================= */

/**
 * @brief  Human-readable names for each GateState_t value.
 *
 * @note   Index must match the numeric value of the corresponding
 *         GateState_t enumerator (see gate_types.h).
 */
static const char * const STATE_NAMES[] =
{
    /* GATE_IDLE_CLOSED    = 0 */ "IDLE_CLOSED\r\n",
    /* GATE_IDLE_OPEN      = 1 */ "IDLE_OPEN\r\n",
    /* GATE_OPENING        = 2 */ "OPENING\r\n",
    /* GATE_CLOSING        = 3 */ "CLOSING\r\n",
    /* GATE_STOPPED_MIDWAY = 4 */ "STOPPED_MIDWAY\r\n",
    /* GATE_REVERSING      = 5 */ "REVERSING\r\n"
};

#define STATE_NAMES_COUNT  (sizeof(STATE_NAMES) / sizeof(STATE_NAMES[0]))

/* =========================================================================
 *  Task Implementation
 * ========================================================================= */

void vStatusTask(void *pvParameters)
{
    (void)pvParameters;

    /* Use a sentinel that cannot match a valid state to force the first
     * print immediately on task startup.                                     */
    GateState_t prevState = (GateState_t)0xFFU;
    GateState_t curState;

    UART_Init();
    UART_SendString("\r\n=== Smart Parking Gate System Started ===\r\n");
    UART_SendString("Gate State: IDLE_CLOSED\r\n");

    for (;;)
    {
        curState = GetGateState();

        if (curState != prevState)
        {
            UART_SendString("Gate State: ");

            if ((uint32_t)curState < (uint32_t)STATE_NAMES_COUNT)
            {
                UART_SendString(STATE_NAMES[(uint32_t)curState]);
            }
            else
            {
                UART_SendString("UNKNOWN\r\n");
            }

            prevState = curState;
        }

        vTaskDelay(pdMS_TO_TICKS(CFG_STATUS_POLL_MS));
    }
}

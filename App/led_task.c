/**
 * @file    led_task.c
 * @brief   LED Control Task implementation.
 *
 * @details Polls the gate state every CFG_LED_POLL_MS milliseconds.
 *          Updates the physical LEDs only when the state has changed to
 *          avoid redundant GPIO writes on every cycle.
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#include "led_task.h"
#include "gate_config.h"
#include "gate_rtos.h"
#include "gate_types.h"
#include "gpio_driver.h"

#include "FreeRTOS.h"
#include "task.h"

/* =========================================================================
 *  Task Implementation
 * ========================================================================= */

void vLEDControlTask(void *pvParameters)
{
    (void)pvParameters;

    /* Initialise prevState to a sentinel that does not match any valid state
     * so that the LED output is written on the very first iteration.        */
    GateState_t prevState = (GateState_t)0xFFU;
    GateState_t curState;

    for (;;)
    {
        curState = GetGateState();

        /* Only update hardware if the state has actually changed */
        if (curState != prevState)
        {
            switch (curState)
            {
                case GATE_OPENING:
                case GATE_REVERSING:
                    /* Gate moving up (or reversing obstacle): Green LED ON */
                    LED_Write(CFG_LED_GREEN);
                    break;

                case GATE_CLOSING:
                    /* Gate moving down: Red LED ON */
                    LED_Write(CFG_LED_RED);
                    break;

                case GATE_IDLE_OPEN:
                case GATE_IDLE_CLOSED:
                case GATE_STOPPED_MIDWAY:
                default:
                    /* Stationary: both LEDs OFF */
                    LED_Write(0U);
                    break;
            }

            prevState = curState;
        }

        vTaskDelay(pdMS_TO_TICKS(CFG_LED_POLL_MS));
    }
}

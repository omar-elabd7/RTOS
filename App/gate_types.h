/**
 * @file    gate_types.h
 * @brief   Shared application-level type definitions for the Smart Gate System.
 *
 * @details Contains all enumerations and typedef aliases that are consumed by
 *          multiple modules.  This file has NO dependencies on any hardware
 *          header so it can be included anywhere in the project.
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#ifndef GATE_TYPES_H
#define GATE_TYPES_H

/* =========================================================================
 *  Gate State-Machine States
 * ========================================================================= */

/**
 * @brief  Enumeration of all possible gate states.
 *
 * @note   The numeric values are used as indices into the state-name string
 *         table inside status_task.c – do NOT reorder or add gaps.
 */
typedef enum
{
    GATE_IDLE_CLOSED    = 0,  /**< Gate fully closed;  both LEDs OFF          */
    GATE_IDLE_OPEN      = 1,  /**< Gate fully open;    both LEDs OFF          */
    GATE_OPENING        = 2,  /**< Gate moving up;     Green LED ON           */
    GATE_CLOSING        = 3,  /**< Gate moving down;   Red   LED ON           */
    GATE_STOPPED_MIDWAY = 4,  /**< Stopped mid-travel; both LEDs OFF          */
    GATE_REVERSING      = 5   /**< Obstacle reverse (0.5 s); Green LED ON     */
} GateState_t;

/* =========================================================================
 *  Button / Input Event Types  (sent via the button queue)
 * ========================================================================= */

/**
 * @brief  Enumeration of all button edge events posted by the Input Task.
 */
typedef enum
{
    EVT_DRV_OPEN_PRESSED   = 0,  /**< Driver   OPEN  button pressed   */
    EVT_DRV_OPEN_RELEASED  = 1,  /**< Driver   OPEN  button released  */
    EVT_DRV_CLOSE_PRESSED  = 2,  /**< Driver   CLOSE button pressed   */
    EVT_DRV_CLOSE_RELEASED = 3,  /**< Driver   CLOSE button released  */
    EVT_SEC_OPEN_PRESSED   = 4,  /**< Security OPEN  button pressed   */
    EVT_SEC_OPEN_RELEASED  = 5,  /**< Security OPEN  button released  */
    EVT_SEC_CLOSE_PRESSED  = 6,  /**< Security CLOSE button pressed   */
    EVT_SEC_CLOSE_RELEASED = 7,  /**< Security CLOSE button released  */
    EVT_OPEN_LIMIT         = 8,  /**< Open  limit switch triggered    */
    EVT_CLOSE_LIMIT        = 9,  /**< Close limit switch triggered    */
    EVT_OBSTACLE           = 10  /**< Obstacle sensor triggered       */
} ButtonEvent_t;

#endif /* GATE_TYPES_H */

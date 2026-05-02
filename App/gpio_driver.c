/**
 * @file    gpio_driver.c
 * @brief   GPIO initialisation and LED control implementation.
 *
 * @author  Embedded Systems Team
 * @date    2026
 */

#include "gpio_driver.h"
#include "gate_config.h"
#include "tm4c123gh6pm.h"

/* =========================================================================
 *  Public Function Implementations
 * ========================================================================= */

void GPIO_Init(void)
{
    /* ------------------------------------------------------------------ */
    /* 1. Enable peripheral clocks for Ports B, D, E, F                   */
    /* ------------------------------------------------------------------ */
    const uint32_t clkMask = CFG_RCGC_PORTB |
                             CFG_RCGC_PORTD |
                             CFG_RCGC_PORTE |
                             CFG_RCGC_PORTF;

    SYSCTL_RCGCGPIO_R |= clkMask;

    /* Wait until all requested ports are ready (PRGPIO ready flags) */
    while ((SYSCTL_PRGPIO_R & clkMask) != clkMask)
    {
        /* spin */
    }

    /* ------------------------------------------------------------------ */
    /* 2. Port F – LEDs (PF1, PF3) outputs; PF4 input with pull-up        */
    /* ------------------------------------------------------------------ */

    /* Unlock PF4 (committed-write-protect for the on-board SW1 pin)      */
    GPIO_PORTF_LOCK_R  = 0x4C4F434BUL;
    GPIO_PORTF_CR_R   |= (CFG_LED_MASK | CFG_BTN_OBSTACLE);

    GPIO_PORTF_AMSEL_R &= ~(CFG_LED_MASK | CFG_BTN_OBSTACLE); /* no analogue */
    GPIO_PORTF_PCTL_R  &= ~0x000F0FF0UL;                      /* GPIO fn     */
    GPIO_PORTF_AFSEL_R &= ~(CFG_LED_MASK | CFG_BTN_OBSTACLE); /* no alt fn   */

    GPIO_PORTF_DIR_R   |=  CFG_LED_MASK;        /* PF1, PF3: outputs       */
    GPIO_PORTF_DIR_R   &= ~CFG_BTN_OBSTACLE;    /* PF4:      input         */

    GPIO_PORTF_PUR_R   |=  CFG_BTN_OBSTACLE;    /* pull-up on PF4          */
    GPIO_PORTF_DEN_R   |=  CFG_LED_MASK | CFG_BTN_OBSTACLE;

    GPIO_PORTF_DATA_R  &= ~CFG_LED_MASK;        /* LEDs off initially      */

    /* ------------------------------------------------------------------ */
    /* 3. Port E – Driver panel: PE0 (OPEN), PE1 (CLOSE) – pull-up, active-LOW */
    /* ------------------------------------------------------------------ */
    const uint32_t portE_mask = CFG_BTN_DRV_OPEN | CFG_BTN_DRV_CLOSE;

    GPIO_PORTE_AMSEL_R &= ~portE_mask;
    GPIO_PORTE_PCTL_R  &= ~0x000000FFUL;
    GPIO_PORTE_AFSEL_R &= ~portE_mask;
    GPIO_PORTE_DIR_R   &= ~portE_mask;          /* inputs                  */
    GPIO_PORTE_PUR_R   |=  portE_mask;          /* pull-up resistors       */
    GPIO_PORTE_DEN_R   |=  portE_mask;

    /* ------------------------------------------------------------------ */
    /* 4. Port B – Security panel: PB0 (OPEN), PB1 (CLOSE) – pull-up, active-LOW */
    /* ------------------------------------------------------------------ */
    const uint32_t portB_mask = CFG_BTN_SEC_OPEN | CFG_BTN_SEC_CLOSE;

    GPIO_PORTB_AMSEL_R &= ~portB_mask;
    GPIO_PORTB_PCTL_R  &= ~0x000000FFUL;
    GPIO_PORTB_AFSEL_R &= ~portB_mask;
    GPIO_PORTB_DIR_R   &= ~portB_mask;
    GPIO_PORTB_PUR_R   |=  portB_mask;
    GPIO_PORTB_DEN_R   |=  portB_mask;

    /* ------------------------------------------------------------------ */
    /* 5. Port D – Limit switches: PD0 (Open Limit), PD1 (Close Limit) – pull-up, active-LOW */
    /* ------------------------------------------------------------------ */
    const uint32_t portD_mask = CFG_BTN_OPEN_LIM | CFG_BTN_CLOSE_LIM;

    GPIO_PORTD_AMSEL_R &= ~portD_mask;
    GPIO_PORTD_PCTL_R  &= ~0x000000FFUL;
    GPIO_PORTD_AFSEL_R &= ~portD_mask;
    GPIO_PORTD_DIR_R   &= ~portD_mask;
    GPIO_PORTD_PUR_R   |=  portD_mask;
    GPIO_PORTD_DEN_R   |=  portD_mask;
}

/* -------------------------------------------------------------------------- */

void LED_Write(uint32_t mask)
{
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~CFG_LED_MASK)
                        | (mask & CFG_LED_MASK);
}

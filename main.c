#include "FreeRTOS.h"
#include "task.h"

#include "App/gate_config.h"
#include "App/gate_rtos.h"
#include "App/gpio_driver.h"
#include "App/safety_task.h"
#include "App/input_task.h"
#include "App/gate_control_task.h"
#include "App/led_task.h"
#include "App/status_task.h"

int main(void)
{

    GPIO_Init();

    GateRTOS_Init();

    xTaskCreate(vSafetyTask,
                "Safety",
                CFG_STACK_SAFETY_TASK,
                NULL,
                CFG_PRIO_SAFETY_TASK,
                NULL);

    xTaskCreate(vInputTask,
                "Input",
                CFG_STACK_INPUT_TASK,
                NULL,
                CFG_PRIO_INPUT_TASK,
                NULL);

    xTaskCreate(vGateControlTask,
                "GateCtrl",
                CFG_STACK_GATE_CTRL_TASK,
                NULL,
                CFG_PRIO_GATE_CTRL_TASK,
                NULL);

    xTaskCreate(vLEDControlTask,
                "LED",
                CFG_STACK_LED_TASK,
                NULL,
                CFG_PRIO_LED_TASK,
                NULL);

    xTaskCreate(vStatusTask,
                "Status",
                CFG_STACK_STATUS_TASK,
                NULL,
                CFG_PRIO_STATUS_TASK,
                NULL);

    vTaskStartScheduler();

    for (;;) { }
}

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configENABLE_MPU 0                  // Cortex-M0+ have not MPU.
#define configSYSTEM_CALL_STACK_SIZE 0      // Cortex-M0+ not support supervisor mode.

#define configUSE_PREEMPTION                    1
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCPU_CLOCK_HZ                      (125000000UL)
#define configTICK_RATE_HZ                      ((TickType_t)1000)
#define configMAX_PRIORITIES                    5
#define configMINIMAL_STACK_SIZE                ((unsigned short)128)
#define configTOTAL_HEAP_SIZE                   ((size_t)(32 * 1024))
#define configMAX_TASK_NAME_LEN                 16
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1

/* Hook function related definitions. */
#define configUSE_MALLOC_FAILED_HOOK            1

/* Use the system provided time functions */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0

/* Enable the software timers. */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               2
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            256

/* Cortex M0 specifics */
#define configPRIO_BITS                         2        // Only 2 bits for RP2040 NVIC

/* Include/exclude API functions */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xTaskGetSchedulerState          1


#endif /* FREERTOSCONFIG_H */

//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f7xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"

void vApplicationTickHook(void);
void vApplicationIdleHook(void);
void vApplicationMallocFailedHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, signed char *pcTaskName);

uint64_t u64IdleTicksCnt = 0; // Counts when the OS has no task to execute.
uint64_t tickTime = 0;        // Counts OS ticks (default = 1000Hz).

void vApplicationTickHook(void) {
	++tickTime;

}

// This FreeRTOS call-back function gets when no other task is ready to execute.
// On a completely unloaded system this is getting called at over 2.5MHz!
// ----------------------------------------------------------------------------
void vApplicationIdleHook(void) {

	++u64IdleTicksCnt;
}

// A required FreeRTOS function.
// ----------------------------------------------------------------------------
void vApplicationMallocFailedHook(void) {
	//configASSERT( 0 );  // Latch on any failure / error.
	assert_param("Malloc failed!\n");

}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, signed char *pcTaskName) {
	(void) pcTaskName;
	(void) pxTask;

	/* Run time stack overflow checking is performed if
	 configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	 function is called if a stack overflow is detected. */
	//taskDISABLE_INTERRUPTS();
	//for( ;; );
	assert_param("Holy FUCK! STACK OVERFLOW !!\n");

}

void test_task(char* pvp)
{

	while(1) {

		HAL_GPIO_TogglePin(GPIOI,GPIO_PIN_1);
		vTaskDelay(1000);

	}



}


int
main(int argc, char* argv[])
{
  // At this stage the system clock should have already been configured
  // at high speed.


	xTaskCreate(test_task,"test",configMINIMAL_STACK_SIZE,NULL,1,NULL);

	vTaskStartScheduler();

	//should not get here

  // Infinite loop
  while (1)
    {
       // Add your code here.
    }
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------

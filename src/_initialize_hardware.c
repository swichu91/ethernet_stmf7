//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_cortex.h"

// ----------------------------------------------------------------------------

// The external clock frequency is specified as a preprocessor definition
// passed to the compiler via a command line option (see the 'C/C++ General' ->
// 'Paths and Symbols' -> the 'Symbols' tab, if you want to change it).
// The value selected during project creation was HSE_VALUE=25000000.
//
// The code to set the clock is at the end.
//
// Note1: The default clock settings assume that the HSE_VALUE is a multiple
// of 1MHz, and try to reach the maximum speed available for the
// board. It does NOT guarantee that the required USB clock of 48MHz is
// available. If you need this, please update the settings of PLL_M, PLL_N,
// PLL_P, PLL_Q to match your needs.
//
// Note2: The external memory controllers are not enabled. If needed, you
// have to define DATA_IN_ExtSRAM or DATA_IN_ExtSDRAM and to configure
// the memory banks in system/src/cmsis/system_stm32f7xx.c to match your needs.

// ----------------------------------------------------------------------------

// Forward declarations.

void
__initialize_hardware (void);

void
__initialize_hardware_early (void);

void
SystemClock_Config (void);

// ----------------------------------------------------------------------------

#if 0

// These are the very first initialisations, performed immediately after
// reset.
//
// Redefine this function to enable external RAM before data & bss init.

void
__initialize_hardware_early (void)
{
}

#endif

static void SYSLED_Init(void)
{
	GPIO_InitTypeDef  gpio_init_structure;

    /* Enable the GPIO_LED clock */
	__HAL_RCC_GPIOI_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    gpio_init_structure.Pin = GPIO_PIN_1;
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(GPIOI, &gpio_init_structure);

    HAL_GPIO_WritePin(GPIOI,GPIO_PIN_1,GPIO_PIN_RESET);

}

static uint8_t __attribute__((section( ".sdram" ))) test_mem[100];

static void SDRAM_Test(void)
{
	uint8_t errcnt=0;
	uint8_t i;

	for(i=0;i<100;i++) {

		test_mem[i] = i;
	}

	for(i=0;i<100;i++) {

		if(test_mem[i] != i) {
			errcnt++;
		}
	}

	if(errcnt > 0) {

		while(1) {

			/* LED1 blinks */
			HAL_GPIO_TogglePin(GPIOI,GPIO_PIN_1);
			HAL_Delay(100);
		}
	}


}




// This is the application hardware initialisation routine,
// redefined to add more inits.
//
// Called from _start(), right after data & bss init, before
// constructors.
//
// After reset the Cortex-M processor is in Thread mode,
// priority is Privileged, and the Stack is set to Main.
//
// Warning: The HAL requires the system timer, running at 1000 Hz
// and calling HAL_IncTick().

void
__initialize_hardware (void)
{
  // Enable instruction & data cache.
  SCB_EnableICache ();
  SCB_EnableDCache ();

  // Initialise the HAL Library; it must be the first function
  // to be executed before the call of any HAL function.
  // More HAL initialisations can be defined in stm32f7xx_hal_msp.c
  HAL_Init ();

  SYSLED_Init();
  SDRAM_Test();

  // Enable HSE Oscillator and activate PLL with HSE as source
  SystemClock_Config ();

  // Call the CSMSIS system clock routine to store the clock frequency
  // in the SystemCoreClock global RAM location.
  SystemCoreClockUpdate ();
}

// Disable when using RTOSes, since they have their own handler.


// This is a sample SysTick handler, use it if you need HAL timings.
void __attribute__ ((section(".after_vectors")))
SysTick_Handler(void)
  {
#if defined(USE_HAL_DRIVER)
    HAL_IncTick();
#endif
  }


// ----------------------------------------------------------------------------

/**
 * @brief  System Clock Configuration
 * @param  None
 * @retval None
 */

/**
 * Set HCLK to 200MHz,
 * APB1 peripheral to 50MHz,
 * APB1 timer to 100 MHz,
 * APB2 peripheral to 100 Mhz,
 * APB2 timer to 200 MHz
 * FCLK cortex to 200 Mhz
 */
void
__attribute__((weak))
SystemClock_Config (void)
{
  // Enable Power Control clock
  __PWR_CLK_ENABLE()
  ;

  // The voltage scaling allows optimizing the power consumption when the
  // device is clocked below the maximum system frequency, to update the
  // voltage scaling value regarding system frequency refer to product
  // datasheet.
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitTypeDef RCC_OscInitStruct;

#if defined(HSE_VALUE) && (HSE_VALUE != 0)
  // Enable HSE Oscillator and activate PLL with HSE as source.
  // This is tuned for STM32F7-DISCOVERY; update it for your board.
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  // This assumes the HSE_VALUE is a multiple of 1 MHz. If this is not
  // your case, you have to recompute these PLL constants.
  RCC_OscInitStruct.PLL.PLLM = 25;
#else
  // Use HSI and activate PLL with HSI as source.
  // This is generic; update it for your board.
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  // 16 is the average calibration value, adjust for your own board.
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  // This assumes the HSI_VALUE is a multiple of 1 MHz. If this is not
  // your case, you have to recompute these PLL constants.
  RCC_OscInitStruct.PLL.PLLM = (HSI_VALUE / 1000000u);
#endif

  RCC_OscInitStruct.PLL.PLLN = 400; /* 200 MHz */
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8; /* To make USB work. */
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  HAL_RCC_OscConfig (&RCC_OscInitStruct);

  // Activate the OverDrive to reach the 200 MHz Frequency
  HAL_PWREx_EnableOverDrive ();

  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
  // clocks dividers
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
      | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  // This is expected to work for most large cores.
  // Check and update it for your own configuration.
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_6);

  HAL_SYSTICK_Config (HAL_RCC_GetHCLKFreq () / 1000);

  HAL_SYSTICK_CLKSourceConfig (SYSTICK_CLKSOURCE_HCLK);
}

// ----------------------------------------------------------------------------

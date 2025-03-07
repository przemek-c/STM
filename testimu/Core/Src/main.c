/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "gpio.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "icm20948.h"
#include <string.h>
#include <stdbool.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LED1_PIN GPIO_PIN_11
#define LED2_PIN GPIO_PIN_12
#define LED3_PIN GPIO_PIN_13
#define LED_PORT GPIOB

#define UART_RX_BUFFER_SIZE 32
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
axises my_gyro;
axises my_accel;
axises my_mag;

uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
uint8_t uartRxData;
bool commandReceived = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void ProcessCommand(uint8_t* command);
void LED_Control(uint8_t led_num, bool state);
void LED_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
    /* USER CODE BEGIN 2 */
  icm20948_init();
  ak09916_init();
  
  LED_Init();
  
  // Start UART reception in interrupt mode
  HAL_UART_Receive_IT(&huart1, &uartRxData, 1);
  
  // Send initial message
  char welcomeMsg[] = "STM32 LED Control Ready\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)welcomeMsg, strlen(welcomeMsg), 100);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  icm20948_gyro_read_dps(&my_gyro);
	  icm20948_accel_read_g(&my_accel);
	  ak09916_mag_read_uT(&my_mag);
    
    // Process command if received
    if (commandReceived) {
      ProcessCommand(uartRxBuffer);
      memset(uartRxBuffer, 0, UART_RX_BUFFER_SIZE);
      commandReceived = false;
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Initialize LEDs
  * @retval None
  */
void LED_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  // Enable the GPIO clock
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  // Configure the LED pins as outputs
  GPIO_InitStruct.Pin = LED1_PIN | LED2_PIN | LED3_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);
  
  // Turn off all LEDs initially
  HAL_GPIO_WritePin(LED_PORT, LED1_PIN | LED2_PIN | LED3_PIN, GPIO_PIN_RESET);
}

/**
  * @brief  Control LED state
  * @param  led_num: LED number (1-3)
  * @param  state: LED state (true = ON, false = OFF)
  * @retval None
  */
void LED_Control(uint8_t led_num, bool state) {
  GPIO_PinState pinState = state ? GPIO_PIN_SET : GPIO_PIN_RESET;
  
  switch (led_num) {
    case 1:
      HAL_GPIO_WritePin(LED_PORT, LED1_PIN, pinState);
      break;
    case 2:
      HAL_GPIO_WritePin(LED_PORT, LED2_PIN, pinState);
      break;
    case 3:
      HAL_GPIO_WritePin(LED_PORT, LED3_PIN, pinState);
      break;
    default:
      break;
  }
}

/**
  * @brief  Process received command
  * @param  command: Command string to process
  * @retval None
  */
void ProcessCommand(uint8_t* command) {
  char response[32] = {0};
  
  // Format: "LEDx ON" or "LEDx OFF" where x is 1-3
  if (strncmp((char*)command, "LED", 3) == 0 && command[3] >= '1' && command[3] <= '3') {
    uint8_t led_num = command[3] - '0';
    
    if (strncmp((char*)command + 4, " ON", 3) == 0) {
      LED_Control(led_num, true);
      sprintf(response, "LED%d turned ON\r\n", led_num);
    } 
    else if (strncmp((char*)command + 4, " OFF", 4) == 0) {
      LED_Control(led_num, false);
      sprintf(response, "LED%d turned OFF\r\n", led_num);
    }
    else {
      sprintf(response, "Invalid command format\r\n");
    }
  } 
  else {
    sprintf(response, "Invalid command\r\n");
  }
  
  // Send response
  HAL_UART_Transmit(&huart1, (uint8_t*)response, strlen(response), 100);
}

/**
  * @brief  UART Reception Callback
  * @param  huart: UART handle
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  static uint8_t rxIndex = 0;
  
  if (huart->Instance == USART1) {
    if (uartRxData == '\n' || uartRxData == '\r') {
      // End of command
      if (rxIndex > 0) {
        uartRxBuffer[rxIndex] = 0; // Null-terminate
        commandReceived = true;
        rxIndex = 0;
      }
    }
    else if (rxIndex < UART_RX_BUFFER_SIZE - 1) {
      uartRxBuffer[rxIndex++] = uartRxData;
    }
    
    // Restart reception
    HAL_UART_Receive_IT(&huart1, &uartRxData, 1);
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

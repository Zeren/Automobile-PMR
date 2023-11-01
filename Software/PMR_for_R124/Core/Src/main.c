/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DISPLAY 0X74

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_tx;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */
uint8_t uart_txdata[48] = "AT+DMOSETGROUP=0,000.0000,000.0000,0000,1,0000\r\n";
uint8_t i2ctxBuff[8];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void InitDisplay(uint16_t address);
void WriteDisplay(uint16_t address, uint8_t number, uint8_t channel,
		uint8_t tone);
uint8_t NumToDig(uint8_t number);
void channel2freq(uint8_t ch, uint8_t *buff);
void SetGroup(uint8_t channel, uint8_t tone);
void SetVolume(uint8_t volume);
void SetFilter(uint8_t emph, uint8_t highpass, uint8_t lowpass);
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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	uint8_t channel = 4; // Default value
	uint8_t tone = 25; // Default value
	uint16_t show = 0;
	uint8_t debounce = 0;

	InitDisplay(DISPLAY);
	HAL_Delay(1000);
	uint8_t txdata[16] = "AT+DMOCONNECT\r\n";
	HAL_UART_Transmit(&huart2, txdata, 15, 1000);

	HAL_Delay(1000);
	SetGroup(channel, tone);

	HAL_Delay(500);
	uint8_t txdata3[19] = "AT+DMOSETVOLUME=8\r\n";
	HAL_UART_Transmit(&huart2, txdata3, 19, 1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		// Debounce button
		if (HAL_GPIO_ReadPin(channel_bt_GPIO_Port, channel_bt_Pin)
				& HAL_GPIO_ReadPin(tone_bt_GPIO_Port, tone_bt_Pin)
				& (debounce != 0))
			debounce -= 1;
		// Display write
		if (show == 0) {
			WriteDisplay(DISPLAY, channel, 1, 0);
		}
		if (show == 100){
			WriteDisplay(DISPLAY, tone, 0, 1);
		}
		show += 1;
		if (show > 200) {
			show = 0;
		}
		// Zaznam z tlacitek
		if (debounce == 0) {
			if (HAL_GPIO_ReadPin(channel_bt_GPIO_Port, channel_bt_Pin) == 0) {
				channel += 1;
				show = 0;
				if (channel > 16)
					channel = 1;
				debounce = 10;
			}
			if (HAL_GPIO_ReadPin(tone_bt_GPIO_Port, tone_bt_Pin) == 0) {
				tone += 1;
				show = 100;
				if (tone > 64)
					tone = 0;
				debounce = 10;
			}
		}
		// write settings to SA818
		if (debounce == 1)
			SetGroup(channel, tone);
		HAL_Delay(10);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00602173;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_OC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 10000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pins : channel_bt_Pin tone_bt_Pin */
  GPIO_InitStruct.Pin = channel_bt_Pin|tone_bt_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
uint8_t NumToDig(uint8_t number) {
	uint8_t symbol = 0;
	switch (number) {
	case 0:
		symbol = 0b00111111;
		break;
	case 1:
		symbol = 0b00000110;
		break;
	case 2:
		symbol = 0b01011011;
		break;
	case 3:
		symbol = 0b01001111;
		break;
	case 4:
		symbol = 0b01100110;
		break;
	case 5:
		symbol = 0b01101101;
		break;
	case 6:
		symbol = 0b01111101;
		break;
	case 7:
		symbol = 0b00000111;
		break;
	case 8:
		symbol = 0b01111111;
		break;
	case 9:
		symbol = 0b01101111;
		break;
	case 10:
		symbol = 0b01110111; //A
		break;
	case 11:
		symbol = 0b01111100; //B
		break;
	case 12:
		symbol = 0b00111001; //C
		break;
	case 13:
		symbol = 0b01011110; //D
		break;
	case 14:
		symbol = 0b01111001; //E
		break;
	case 15:
		symbol = 0b01110001; //F
		break;
	}
	return symbol;
}

void channel2freq(uint8_t ch, uint8_t *buff) {
	static char freq[8];
	switch (ch) {
	case 1:
		memcpy(freq, "446.0062", 8);
		break;
	case 2:
		memcpy(freq, "446.0187", 8);
		break;
	case 3:
		memcpy(freq, "446.0312", 8);
		break;
	case 4:
		memcpy(freq, "446.0437", 8);
		break;
	case 5:
		memcpy(freq, "446.0562", 8);
		break;
	case 6:
		memcpy(freq, "446.0687", 8);
		break;
	case 7:
		memcpy(freq, "446.0812", 8);
		break;
	case 8:
		memcpy(freq, "446.0937", 8);
		break;
	case 9:
		memcpy(freq, "446.1062", 8);
		break;
	case 10:
		memcpy(freq, "446.1187", 8);
		break;
	case 11:
		memcpy(freq, "446.1312", 8);
		break;
	case 12:
		memcpy(freq, "446.1437", 8);
		break;
	case 13:
		memcpy(freq, "446.1562", 8);
		break;
	case 14:
		memcpy(freq, "446.1687", 8);
		break;
	case 15:
		memcpy(freq, "446.1812", 8);
		break;
	case 16:
		memcpy(freq, "446.1937", 8);
		break;
	}
	memmove(buff, freq, sizeof(freq));
}

void InitDisplay(uint16_t address) {
//	uint8_t i2ctxBuff[8];
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
		HAL_Delay(1);
	i2ctxBuff[0] = 0x07;
	i2ctxBuff[1] = 0x00;

//	HAL_I2C_Master_Transmit(&hi2c1, address << 1, i2ctxBuff, 2, 100);
	HAL_I2C_Master_Transmit_DMA(&hi2c1, address << 1, i2ctxBuff, 2);
	HAL_Delay(1);
	i2ctxBuff[0] = 0x06;
	i2ctxBuff[1] = 0x00;
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
		HAL_Delay(1);
//	HAL_I2C_Master_Transmit(&hi2c1, address << 1, i2ctxBuff, 2, 100);
	HAL_I2C_Master_Transmit_DMA(&hi2c1, address << 1, i2ctxBuff, 2);
	HAL_Delay(1);
}

void WriteDisplay(uint16_t address, uint8_t number, uint8_t channel,
		uint8_t tone) {
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
		HAL_Delay(1);
	if (number > 99)
		number = 99;
	uint8_t digit1 = number % 10;
	uint8_t digit2 = number / 10;
	uint8_t symbol = NumToDig(digit1);

	symbol = symbol | tone << 7;
	symbol = ~symbol;



	i2ctxBuff[0] = 0x03;
	i2ctxBuff[1] = symbol;
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
		HAL_Delay(1);
//	HAL_I2C_Master_Transmit(&hi2c1, address << 1, i2ctxBuff, 2, 100);
	HAL_I2C_Master_Transmit_DMA(&hi2c1, address << 1, i2ctxBuff, 2);
	HAL_Delay(1);

	symbol = NumToDig(digit2);

	symbol = symbol | channel << 7;
	symbol = ~symbol;

	i2ctxBuff[0] = 0x02;
	i2ctxBuff[1] = symbol;
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
		HAL_Delay(1);
//	HAL_I2C_Master_Transmit(&hi2c1, address << 1, i2ctxBuff, 2, 100);
	HAL_I2C_Master_Transmit_DMA(&hi2c1, address << 1, i2ctxBuff, 2);
	HAL_Delay(1);
}

void SetGroup(uint8_t channel, uint8_t tone) {
	if (tone > 38)
		tone = 38;
	uint8_t tone_d1 = tone % 10 + '0';
	uint8_t tone_d2 = tone / 10 + '0';

	uart_txdata[44] = tone_d2;
	uart_txdata[45] = tone_d1;
	uart_txdata[37] = tone_d2;
	uart_txdata[38] = tone_d1;

	channel2freq(channel, &uart_txdata[17]);
	channel2freq(channel, &uart_txdata[26]);

	while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY)
		HAL_Delay(1);
//	HAL_UART_Transmit(&huart2, txdata2, 48, 1000);

	HAL_UART_Transmit_DMA(&huart2, uart_txdata, 48);
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
	while (1) {
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

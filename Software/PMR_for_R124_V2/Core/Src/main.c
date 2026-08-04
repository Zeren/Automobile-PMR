/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rda1846.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_tx;
DMA_HandleTypeDef hdma_i2c1_rx;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */
volatile uint8_t current_channel = 4; // Index 3 odpovídá PMR kanálu 4 (446.043750 MHz)
volatile uint8_t current_ctcss = 5;  // Index 25 odpovídá CTCSS tónu 156.7 Hz
volatile uint8_t is_tx = 0;           // Stavový příznak transceiveru (0 = RX, 1 = TX)

RDA1846_Reg30_t rda_settings = {0};
RDA1846_ctcss ctcss_settings = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void Debug_Print(const char *msg);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Debug_Print(const char *msg) {
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);
}
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  char debug_msg[128];
  Debug_Print("\r\n=======================================\r\n");
  Debug_Print("--- PMR446 Transceiver Booting ---\r\n");
  Debug_Print("=======================================\r\n");

  rda_settings.bits.pdn_reg = 1;
  rda_settings.bits.channel_mode = 0;
  rda_settings.bits.sq_on = 1;
  rda_settings.bits.chip_cal_en = 1;    // Povolení kalibrace VCO
  rda_settings.bits.others = 1;         // Nastaví bit 14 na 1 (Režim 26 MHz)

  ctcss_settings.tx_CTCSS = RDA1846_CTCSS_INNER_CTCSS_EN;
  ctcss_settings.rx_CTCSS = RDA1846_CTCSS_INNER_CTCSS_EN;
  ctcss_settings.tone = CTCSS_Tones[current_ctcss];

  HAL_StatusTypeDef i2c_stat = RDA1846_WriteRegister(0x30, rda_settings.value);
  if (i2c_stat != HAL_OK) {
      Debug_Print("[ERROR] I2C komunikace s RDA1846 selhala!\r\n");
  } else {
      Debug_Print("[OK] I2C komunikace navazana.\r\n");
  }

  Debug_Print("[INFO] Nahravam tovarni inicializacni sekvenci (DSP/AGC/PLL)...\r\n");
  RDA1846_Init();

  RDA1846_SetReferenceClock(25000);

  uint32_t target_freq = PMR446_Frequencies[current_channel];
  snprintf(debug_msg, sizeof(debug_msg), "[INFO] Nastavuji frekvenci: %lu Hz (Kanal %d)\r\n", target_freq, current_channel + 1);
  Debug_Print(debug_msg);
  RDA1846_SetFrequency(target_freq);

  // Nastavení Squelch hystereze (Otevře při -113 dBm, Zavře při -116 dBm)
    RDA1846_SetSquelchThresholds(-113, -116);

  HAL_GPIO_WritePin(PA_BIAS_ON_GPIO_Port, PA_BIAS_ON_Pin, GPIO_PIN_RESET);
  RDA1846_SetRxMode(&rda_settings, &ctcss_settings);
  is_tx = 0;
  Debug_Print("[INFO] Inicializace dokoncena. System prechazi do RX rezimu.\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  static uint32_t last_debounce_time = 0;
  static uint32_t last_telemetry_time = 0;
  static GPIO_PinState last_ptt_state = GPIO_PIN_SET;
  static GPIO_PinState debounced_ptt_state = GPIO_PIN_SET;

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // --- 1. Vyhodnoceni PTT (Debounce) ---
    GPIO_PinState current_ptt_state = HAL_GPIO_ReadPin(PTT_GPIO_Port, PTT_Pin);

    if (current_ptt_state != last_ptt_state) {
        last_debounce_time = HAL_GetTick();
    }

    if ((HAL_GetTick() - last_debounce_time) > 20) {
        if (current_ptt_state != debounced_ptt_state) {
            debounced_ptt_state = current_ptt_state;

            if (debounced_ptt_state == GPIO_PIN_RESET && !is_tx) {
                Debug_Print("[ACTION] PTT stisknuto. Prepinam do TX...\r\n");
                HAL_GPIO_WritePin(PA_BIAS_ON_GPIO_Port, PA_BIAS_ON_Pin, GPIO_PIN_SET);
                HAL_Delay(10);
                RDA1846_SetTxMode(&rda_settings, &ctcss_settings);
                is_tx = 1;
                Debug_Print("[INFO] TX rezim aktivni.\r\n");
            }
            else if (debounced_ptt_state == GPIO_PIN_SET && is_tx) {
                Debug_Print("[ACTION] PTT uvolneno. Prepinam do RX...\r\n");
                RDA1846_SetRxMode(&rda_settings, &ctcss_settings);
                HAL_Delay(10);
                HAL_GPIO_WritePin(PA_BIAS_ON_GPIO_Port, PA_BIAS_ON_Pin, GPIO_PIN_RESET);
                is_tx = 0;
                Debug_Print("[INFO] RX rezim aktivni.\r\n");
            }
        }
    }
    last_ptt_state = current_ptt_state;

    // --- 2. Kontinualni telemetrie (RSSI a SQ stav) ---
    if (!is_tx && (HAL_GetTick() - last_telemetry_time) > 500) {
        last_telemetry_time = HAL_GetTick();

        uint16_t raw_rssi = RDA1846_ReadRSSI();
        uint16_t flags = RDA1846_ReadFlags();

        // Konverze surové hodnoty z registru 0x5F (bity 9:0) na dBm dle datasheetu
        int16_t rssi_dbm = (raw_rssi / 8) - 135;

        snprintf(debug_msg, sizeof(debug_msg), "[TELEMETRY] RSSI: %d dBm (raw: %u), Flags: 0x%04X\r\n", rssi_dbm, raw_rssi, flags);
        Debug_Print(debug_msg);

        if (flags & FLAG_SQUELCH_OPEN) {
            Debug_Print(" >>> SQUELCH OTEVREN (Nosna detekovana) <<<\r\n");
        }
        if (flags & FLAG_CTCSS_DETECTED) {
            Debug_Print(" >>> CTCSS TON DETEKOVÁN <<<\r\n");
        }
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
  hi2c1.Init.Timing = 0x00C12166;
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
  huart2.Init.BaudRate = 115200;
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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PA_BIAS_ON_GPIO_Port, PA_BIAS_ON_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA_BIAS_ON_Pin */
  GPIO_InitStruct.Pin = PA_BIAS_ON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PA_BIAS_ON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PTT_Pin */
  GPIO_InitStruct.Pin = PTT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PTT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RADIO_INT_Pin */
  GPIO_InitStruct.Pin = RADIO_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RADIO_INT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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

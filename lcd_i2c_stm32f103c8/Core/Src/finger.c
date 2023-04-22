#include "finger.h"
#include "stm32f1xx_hal.h"
extern UART_HandleTypeDef huart1;
void UART1_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  UART_HandleTypeDef huart1 = {0};

  /* Peripheral clock enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_USART1_CLK_ENABLE();

  /* UART1 GPIO Configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* UART1 initialization */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    /* Error_Handler() is not defined here, so you may want to add your own implementation */
    while(1);
  }
}

uint8_t Finger_GetImage(void) {
  uint8_t tx_data[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, GET_IMAGE_CMD, 0x00, 0x03, 0x01, 0x05};
  uint8_t rx_data[12];

  /* Send the command to the module */
  HAL_UART_Transmit(&huart1, tx_data, sizeof(tx_data), HAL_MAX_DELAY);

  /* Wait for the acknowledgement from the module */
  HAL_UART_Receive(&huart1, rx_data, sizeof(rx_data), HAL_MAX_DELAY);

  /* Check the confirmation code in the acknowledgement */
  if (rx_data[9] == GET_IMAGE_SUCCESS) {
    /* Finger image collected successfully */
    return GET_IMAGE_SUCCESS;
  } else if (rx_data[9] == GET_IMAGE_ERROR_NO_FINGER) {
    /* No finger detected */
    return GET_IMAGE_ERROR_NO_FINGER;
  } else {
    /* Other errors occurred */
    return GET_IMAGE_ERROR_FAIL;
  }
}
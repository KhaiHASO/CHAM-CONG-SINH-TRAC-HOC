#include "finger.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_rcc_ex.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_exti.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#include "stm32f1xx_hal_uart.h"
#include <stdbool.h> 
#include "i2c-lcd.h"

#define FINGER_BUFFER_LENGTH 64
#define RECEIVE_TIMEOUT 500// receive timeout in milliseconds
#define UART_COMMAND_LENGTH 12
extern UART_HandleTypeDef huart1;
uint8_t Finger_Buffer[FINGER_BUFFER_LENGTH];
uint8_t fingerID;

// Receives a message from the fingerprint sensor
uint8_t receive_finger(UART_HandleTypeDef *huart, uint8_t len) {
  uint8_t data[FINGER_BUFFER_LENGTH];
  uint8_t p;
  int i = 0;
  while (i < len) {
    HAL_UART_Receive(huart, &data[i], 1, RECEIVE_TIMEOUT); // receive 1 byte within the timeout
    i++;
  }
  p = data[len - 3];
  return p;
}

// Receives a message for fingerprint matching
uint8_t receive_finger_match(UART_HandleTypeDef *huart, uint8_t len) {
  uint8_t data[FINGER_BUFFER_LENGTH];
  uint8_t p;
  int i = 0;
  while (i < len) {
    HAL_UART_Receive(huart, &data[i], 1, RECEIVE_TIMEOUT); // receive 1 byte within the timeout
    i++;
  }
  p = data[len - 5];
  return p;
}

// Receives a message for fingerprint search
uint8_t receive_finger_search(UART_HandleTypeDef *huart, uint8_t len) {
  uint8_t data[FINGER_BUFFER_LENGTH];
  uint8_t p;
  int i = 0;
  while (i < len) {
    HAL_UART_Receive(huart, &data[i], 1, RECEIVE_TIMEOUT); // receive 1 byte within the timeout
    i++;
  }
  p = data[len - 7];
  fingerID = data[11];
  return p;
}

// Collects fingerprint data
int collect_finger(UART_HandleTypeDef *huart) {
  uint8_t buffer[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x01, 0x00, 0x05};
  HAL_UART_Transmit(huart, buffer, sizeof(buffer), RECEIVE_TIMEOUT); // transmit data within the timeout
  return receive_finger(huart, 12);
}

// Converts fingerprint image to template
int img2tz(uint8_t local) {
  int sum = 0x00;
  sum = local + 0x07;
  uint8_t buffer[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x04, 0x02, local, 0x00, sum};
  HAL_UART_Transmit(&huart1, buffer, sizeof(buffer), RECEIVE_TIMEOUT);
  return receive_finger(&huart1, 12);
}

int match(void)
{
  uint8_t data[14] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x03, 0x00, 0x07};
  HAL_UART_Transmit(&huart1, data, 12, 100);
  return receive_finger_match(&huart1,14);
}

int regmodel(void)
{
  uint8_t data[12] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x05, 0x00, 0x09};
  HAL_UART_Transmit(&huart1, data, 12, 100);
  return receive_finger(&huart1,12);
}

int store(uint8_t ID)
{
  uint8_t sum1 = 0x0E + ID;
  uint8_t data[15] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x06, 0x06, 0x01,0x00 ,ID, 0x00, sum1};
  HAL_UART_Transmit(&huart1, data, 14, 100);
  return receive_finger(&huart1,12);
}

int search(void)
{
  uint8_t data[17] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x08, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x01};
  HAL_UART_Transmit(&huart1, data, 17, 100);
  return receive_finger_search(&huart1,16);
}

int search1(void)
{
  uint8_t data[17] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x08, 0x04, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0F};
  HAL_UART_Transmit(&huart1, data, 17, 100);
  return receive_finger_search(&huart1,16);
}

int empty(void)
{
  uint8_t data[12] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x0D, 0x00, 0x11};
  HAL_UART_Transmit(&huart1, data, 12, 100);
  return receive_finger(&huart1,12);
}

//**********************CAC HAM SU DUNG THUC TE*********************************
//**********************CAC HAM SU DUNG THUC TE*********************************
//**********************CAC HAM SU DUNG THUC TE*********************************
//**********************CAC HAM SU DUNG THUC TE*********************************
//**********************CAC HAM SU DUNG THUC TE*********************************
//**********************CAC HAM SU DUNG THUC TE*********************************
//**********************CAC HAM SU DUNG THUC TE*********************************
void add_fingerprint(UART_HandleTypeDef *huart, uint8_t ID) {
  // Step 1: Collect fingerprint data
  int result = collect_finger(huart);
  if (result != 0x00) {
    // Error occurred while collecting fingerprint data
    sendlcd("Collecting failed");
    return;
  }
	sendlcd("Collecting OK");

  // Step 2: Convert fingerprint image to template
  result = img2tz(0x01);
  if (result != 0x00) {
    // Error occurred while converting fingerprint image to template
    sendlcd("image failed");
    return;
  }
	sendlcd("image OK");

  // Step 3: Store the fingerprint template in the specified ID
  result = store(ID);
  if (result != 0x00) {
    // Error occurred while storing the fingerprint template
    sendlcd("Storing failed");
    return;
  }
	sendlcd("Storing OK");

  // Successfully added fingerprint
  sendlcd("Successfully");
}

int verify_fingerprint(void) {
  int result = search();
  if (result == 0) {
    sendlcd("Not found");
    HAL_Delay(1000);
    return 0;
  }
  result = match();
  if (result == 0) {
    sendlcd("Incorrect");
    HAL_Delay(1000);
    return 0;
  }
  sendlcd("Complete");
  HAL_Delay(1000);
  return 1;
}





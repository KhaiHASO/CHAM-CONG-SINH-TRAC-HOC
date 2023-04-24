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
extern UART_HandleTypeDef huart1;

uint8_t Finger_Buffer[FINGER_BUFFER_LENGTH];

#define RECEIVE_TIMEOUT 1000 // th?i gian ch? nh?n d? li?u

uint8_t pID;

uint8_t receive_finger(UART_HandleTypeDef *huart, uint8_t len) {
  uint8_t p, D[20];
  int i = 0;
  while (i < len) {
    HAL_UART_Receive(huart, &D[i], 1, 1000); // nh?n m?t byte trong 1 giây
    i++;
  }
  p = D[len - 3];
  return p;
}

uint8_t receive_finger_match(UART_HandleTypeDef *huart, uint8_t len) {
  uint8_t p, D[20];
  int i = 0;
  while (i < len) {
    HAL_UART_Receive(huart, &D[i], 1, 1000); // nh?n m?t byte trong 1 giây
    i++;
  }
  p = D[len - 5];
  return p;
}

uint8_t receive_finger_search(UART_HandleTypeDef *huart, uint8_t len) {
  uint8_t p, D[20];
  int i = 0;
  while (i < len) {
    HAL_UART_Receive(huart, &D[i], 1, 1000); // nh?n m?t byte trong 1 giây
    i++;
  }
  p = D[len - 7];
  pID = D[11];
  return p;
}

int collect_finger(UART_HandleTypeDef *huart) {
  uint8_t buffer[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x01, 0x00, 0x05};
  HAL_UART_Transmit(huart, buffer, sizeof(buffer), 1000); // truy?n d? li?u trong 1 giây
  return receive_finger(huart, 12);
}

int img2tz(uint8_t local) {
  int sum = 0x00;
  sum = local + 0x07;
  uint8_t buffer[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x04, 0x02, local, 0x00, sum};
  HAL_UART_Transmit(&huart1, buffer, sizeof(buffer), 1000);
  return receive_finger(&huart1, 12);
}

int match(void) {
  uint8_t buffer[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x03, 0x00, 0x07};
  HAL_UART_Transmit(&huart1, buffer, sizeof(buffer), 1000);
  return receive_finger_match(&huart1, 14);
}

int regmodel(void) {
  uint8_t buffer[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x05, 0x00, 0x09};
  HAL_UART_Transmit(&huart1, buffer, sizeof(buffer), 1000);
  return receive_finger(&huart1, 12);
}

int store(uint8_t ID) {
  uint8_t sum1 = 0x0E + ID;
  uint8_t buffer[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x06, 0x06, 0x01, 0x00, ID, 0x00, sum1};
  HAL_UART_Transmit(&huart1, buffer, sizeof(buffer), 1000);
  return receive_finger(&huart1, 12);
}

int search(void) {
  uint8_t buffer[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x08, 0x04, 0x01, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x0F};
  HAL_UART_Transmit(&huart1, buffer, sizeof(buffer), 1000);
  return receive_finger_search(&huart1, 16);
}

int search1(void) {
  uint8_t buffer[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x08, 0x04, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0F};
  HAL_UART_Transmit(&huart1, buffer, sizeof(buffer), 1000);
  return receive_finger_search(&huart1, 16);
}

int empty(void)
{		
   uint8_t buffer[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x0D, 0x00, 0x11};
   HAL_UART_Transmit(&huart1, buffer, sizeof(buffer), 1000); // truy?n d? li?u trong 1 giây
   return receive_finger(&huart1, 12);
}

void testfinger()
{
	bool FINGER_OK = false; 
	  // Ch? c?m bi?n vân tay k?t n?i
  while (collect_finger(&huart1) != FINGER_OK)
  {
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("Sensor not found");
    HAL_Delay(1000);
  }

  // Ch? ngu?i dùng d?t ngón tay lên c?m bi?n
  while (img2tz(1) != FINGER_OK)
  {
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("Place finger");
    HAL_Delay(1000);
  }

  // Luu c?m bi?n vân tay v?i ID = 1
  if (store(1) == FINGER_OK)
  {
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("Finger stored");
    HAL_Delay(1000);
  }
  else
  {
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("Error storing");
    HAL_Delay(1000);
  }

  // Ch? ngu?i dùng d?t ngón tay lên c?m bi?n d? ki?m tra kh?p
  while (img2tz(2) != FINGER_OK)
  {
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("Place finger");
    HAL_Delay(1000);
  }

  // Tìm ki?m c?m bi?n vân tay v?i ID = 1
  if (search() == FINGER_OK && pID == 1)
  {
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("Finger matched");
    HAL_Delay(1000);
  }
  else
  {
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("Finger not found");
    HAL_Delay(1000);
  }

  // Xóa c?m bi?n vân tay v?i ID = 1
  if (empty() == FINGER_OK)
  {
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("Finger removed");
    HAL_Delay(1000);
  }
  else
  {
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("Error removing");
    HAL_Delay(1000);
  }

}




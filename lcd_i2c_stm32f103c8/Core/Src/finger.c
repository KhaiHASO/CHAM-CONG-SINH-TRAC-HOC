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
#include "buzz.h"
uint8_t pID;
uint8_t receive_finger(uint8_t len)
{
  uint8_t p, D[20];
  int i = 0;
  while (i < len)
  {
    if (HAL_UART_Receive(&huart1, &D[i], 1, HAL_MAX_DELAY) == HAL_OK)
    {
      i++;
    }
  }
  p = D[len - 3];
  return p;
}

uint8_t receive_finger_match(uint8_t len)
{
  uint8_t p, D[20];
  int i = 0;
  while (i < len)
  {
    if (HAL_UART_Receive(&huart1, &D[i], 1, HAL_MAX_DELAY) == HAL_OK)
    {
      i++;
    }
  }
  p = D[len - 5];
  return p;
}

uint8_t receive_finger_search(uint8_t len)
{
  uint8_t p, D[20];
  int i = 0;
  while (i < len)
  {
    if (HAL_UART_Receive(&huart1, &D[i], 1, HAL_MAX_DELAY) == HAL_OK)
    {
      i++;
    }
  }
  p = D[len - 7];
  pID = D[11];

  return p;
}

int collect_finger(void)
{
  uint8_t tx_buf[12] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x01, 0x00, 0x05};
  if (HAL_UART_Transmit(&huart1, tx_buf, 12, HAL_MAX_DELAY) != HAL_OK)
  {
    return -1;
  }
  return receive_finger(12);
}

int img2tz(uint8_t local)
{
  int sum = 0x00;
  uint8_t tx_buf[12] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x04, 0x02, local, 0x00};
  for (int i = 0; i < 12; i++)
  {
    sum += tx_buf[i];
  }
  tx_buf[11] = sum & 0xFF;

  if (HAL_UART_Transmit(&huart1, tx_buf, 12, HAL_MAX_DELAY) != HAL_OK)
  {
    return -1;
  }
  return receive_finger(12);
}

int match(void)
{
  uint8_t tx_buf[14] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x03, 0x00, 0x07};
  if (HAL_UART_Transmit(&huart1, tx_buf, 14, HAL_MAX_DELAY) != HAL_OK)
  {
    return -1;
  }
  return receive_finger_match(14);
}

uint8_t regmodel(void)
{ // t?o mã vân tay chu?n t? 2 b? d?m
  uint8_t txData[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x05, 0x00, 0x09};
  HAL_UART_Transmit(&huart1, txData, sizeof(txData), HAL_MAX_DELAY);
  return receive_finger(12);
}

uint8_t store(uint8_t ID)
{  // luu mã vân tay chu?n vào flash
  uint8_t sum1 = 0x0E + ID;
  uint8_t txData[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x06, 0x06, 0x01, 0x00, ID, 0x00, sum1};
  HAL_UART_Transmit(&huart1, txData, sizeof(txData), HAL_MAX_DELAY);
  return receive_finger(12);
}

uint8_t search(void)
{ // l?y mã vân tay t? flash ra d? so sánh v?i vân tay v?a nh?n trên b? d?m
  uint8_t txData[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x08, 0x04, 0x01, 0x00, 0x00, 0x00, 0xff, 0x00, 0x0D};
  HAL_UART_Transmit(&huart1, txData, sizeof(txData), HAL_MAX_DELAY);
  return receive_finger_search(16);
}
int search1(void)
{
    uint8_t txData[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x08, 0x04, 0x01, 0x00, 0x00, 0x00, 0xff, 0x00, 0x0D};
    HAL_UART_Transmit(&huart1, txData, sizeof(txData), HAL_MAX_DELAY);
    return receive_finger_search(16);
}

int empty(void)
{
    uint8_t txData[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x0D, 0x00, 0x11};
    HAL_UART_Transmit(&huart1, txData, sizeof(txData), HAL_MAX_DELAY);
    return receive_finger(12);
}
///////hàm su dung thuc te ##################################################################################
#define ADD_FINGER 1
#define MATCH_TIMEOUT 5000 // Timeout cho vi?c ch? k?t qu? kh?p vân tay

// Hàm thêm vân tay

int add_fingerprint(uint8_t ID)
{
    int ret = 0;
    bool success = false;
    
    // Nh?p m?u vân tay
    while (!success)
    {
        ret = collect_finger();
        if (ret != 0x00) {
            return ret;
        }
        ret = img2tz(1);
        if (ret != 0x00) {
            return ret;
        }
        ret = regmodel();
        if (ret != 0x00) {
            return ret;
        }
        
        // Xác minh m?u vân tay l?n 1
        ret = search1();
        if (ret == 0x00)
        {
            // Tìm th?y ID trùng kh?p trong flash
            return -1;
        }
        else if (ret == 0x09)
        {
            // Không tìm th?y ID trong flash
            success = true;
        }
        else
        {
            return ret;
        }
    }
    
    success = false;
    // Nh?p l?i m?u vân tay
    while (!success)
    {
        ret = collect_finger();
        if (ret != 0x00) {
            return ret;
        }
        ret = img2tz(2);
        if (ret != 0x00) {
            return ret;
        }
        ret = regmodel();
        if (ret != 0x00) {
            return ret;
        }
        
        // Xác minh m?u vân tay l?n 2
        ret = search1();
        if (ret == 0x00)
        {
            // Tìm th?y ID trùng kh?p trong flash
            success = false;
        }
        else if (ret == 0x09)
        {
            // Không tìm th?y ID trong flash
            success = true;
        }
        else
        {
            return ret;
        }
    }
    
    // Luu ID vào flash
    ret = store(ID);
    if (ret != 0x00) {
        return ret;
    }
    
    return 0;
}

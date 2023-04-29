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
extern uint8_t pID;
uint8_t tmp = 0xff;
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
{ // ghi du lieu van tay vao bo nho dem local(local co the la: 0x01 vung 1, 0x02 vung 2)
    int sum = 0x00;
    sum = local + 0x07;
    uint8_t tx_buf[13] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x04, 0x02, local, 0x00, sum};
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return -1; // l?i khi g?i d? li?u
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
  uint8_t txData[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x08, 0x04, 0x01, 0x00, 0x00, 0x00, 0xff, 0x01, 0x0D};
  HAL_UART_Transmit(&huart1, txData, sizeof(txData), HAL_MAX_DELAY);
  return receive_finger_search(16);
}
int search1(void)
{
    uint8_t txData[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x08, 0x04, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0F};
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
// Hàm thêm vân tay
int collectFinger(void)
{
  collect:
  sendlcd("DAT TAY VAO");
	beep(50,1);

  while (collect_finger() != 0x00)
  {
    tmp = collect_finger();
  }
  sendlcd("COLLECT FINGER DONE");

  tmp = 0xff;
  while (tmp != 0x00)
  {
    tmp = img2tz(0x01);
  }
  sendlcd("IMG2TZ(0x01) DONE");

  tmp = 0xff;
  while (tmp != 0x00)
  {
    tmp = collect_finger();
  }
  sendlcd("COLLECT FINGER DONEee");

  tmp = 0xff;
  while (tmp != 0x00)
  {
    tmp = img2tz(0x02);
  }
  sendlcd("IMG2TZ(0x02) DONE");

  tmp = 0xff;
  while (tmp != 0x00)
  {
    tmp = match();
    HAL_Delay(100);
    if (tmp == 0x08 || (tmp == 0x01))
    {
      sendlcd("LOI,LAM LAI");
      HAL_Delay(1000);
      goto collect;
    }
    sendlcdint(tmp);
  }
  sendlcd("MATCH DONE");

  tmp = 0xff;
  while (tmp != 0x00)
  {
    tmp = regmodel();
    HAL_Delay(100);
    sendlcdint(tmp);
  }
  sendlcd("REGMODEL DONE");
	return 1;
}
void addFinger(int ID)
{
	collectFinger();
  tmp = 0xff;
  while (tmp != 0x00)
  {
    tmp = store(ID);
    HAL_Delay(100);
    sendlcdint(tmp);
  }
  sendlcd("STORE DONE");
	beep(50,2);
  HAL_Delay(1000);
  tmp = 0xff;
}

int verify_fingerprint()
	{

		collectFinger();
    // Search for fingerprint in database
    uint16_t finger_id = search();
    
    // Check if fingerprint is in database
    if (finger_id == 0x00) {
        // Fingerprint match found
			sendlcdint(pID);
			HAL_Delay(2000);
			beep(75,3);
        return pID;
			
    } else {
        // Fingerprint match not found
			sendlcd("not found");
			HAL_Delay(2000);
			beep(500,2);
        return -1;
    }
}
void deleteAllFinger(void)
{
	empty();
	beep(500,2);
	sendlcd("Xoa toan bo");
	
}





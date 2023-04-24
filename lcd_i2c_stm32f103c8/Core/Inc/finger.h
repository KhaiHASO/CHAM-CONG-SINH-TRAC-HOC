#ifndef FINGER_H
#define FINGER_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_rcc_ex.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_exti.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#include "stm32f1xx_hal_uart.h"

#define FINGER_ACK 0x00
#define FINGER_NACK_RECV_ERR 0x01
#define FINGER_NACK_NO_FINGER 0x02
#define FINGER_NACK_FAIL 0x03

extern UART_HandleTypeDef huart1;
int collect_finger(UART_HandleTypeDef *huart);
int img2tz(uint8_t local);
int match(void);
int regmodel(void);
int store(uint8_t ID);
int search(void);
int search1(void);
int empty(void);
void testfinger();


#endif /* FINGER_H */

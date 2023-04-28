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

extern UART_HandleTypeDef huart1;

uint8_t receive_finger(uint8_t len);
uint8_t receive_finger_match(uint8_t len);
uint8_t receive_finger_search(uint8_t len);
int collect_finger(void);
int img2tz(uint8_t local);
int match(void);
uint8_t regmodel(void);
uint8_t store(uint8_t ID);
uint8_t search(void);
int search1(void);
int empty(void);
void addFinger(int ID);
int verify_fingerprint(void) ;
#endif /* FINGER_H */

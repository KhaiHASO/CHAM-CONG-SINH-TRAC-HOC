#include "stm32f10x.h"
//#include "UART.h"
#include "stdint.h"
#include "stdio.h"
#include "finger.h"
#include "stm32f1xx_hal_uart.h"

extern uint8_t pID;

uint8_t receive_finger(uint8_t len)
{
    uint8_t p, D[20];
    int i = 0;
    while(i < len) {
        if (USART1_ACTION()) {
            D[i] = USART1_getc();
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
    while(i < len) {
        if (USART1_ACTION()) {
            D[i] = USART1_getc();
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
    while(i < len) {
        if (USART1_ACTION()) {
            D[i] = USART1_getc();
            i++;
        }
    }
    p = D[len - 7];
    pID = D[11];
    return p;
}

int collect_finger(void)
{
    USART1_SendByte(0xEF); USART1_SendByte(0x01);
    USART1_SendByte(0xFF); USART1_SendByte(0xFF);
    USART1_SendByte(0xFF); USART1_SendByte(0xFF);
    USART1_SendByte(0x01);
    USART1_SendByte(0x00); USART1_SendByte(0x03);
    USART1_SendByte(0x01);
    USART1_SendByte(0x00); USART1_SendByte(0x05);
    return receive_finger(12);
}

int img2tz(uint8_t local)
{
    int sum = 0x00;
    sum = local + 0x07;
    USART1_SendByte(0xEF); USART1_SendByte(0x01);
    USART1_SendByte(0xFF); USART1_SendByte(0xFF);
    USART1_SendByte(0xFF); USART1_SendByte(0xFF);
    USART1_SendByte(0x01);
    USART1_SendByte(0x00); USART1_SendByte(0x04);
    USART1_SendByte(0x02);
    USART1_SendByte(local);
    USART1_SendByte(0x00); USART1_SendByte(sum);
    return receive_finger(12);
}

int match(void) {
  USART1_SendByte(0xEF);
  USART1_SendByte(0x01);
  USART1_SendByte(0xFF);
  USART1_SendByte(0xFF);
  USART1_SendByte(0xFF);
  USART1_SendByte(0xFF);
  USART1_SendByte(0x01);
  USART1_SendByte(0x00);
  USART1_SendByte(0x03);
  USART1_SendByte(0x03);
  USART1_SendByte(0x00);
  USART1_SendByte(0x07);
  return receive_finger_match(14);
}

int regmodel(void)
{
    uint8_t sum1;
    sum1 = 0x0E + pID;
    USART1_SendByte(0xEF); USART1_SendByte(0x01);
    USART1_SendByte(0xFF); USART1_SendByte(0xFF); USART1_SendByte(0xFF); USART1_SendByte(0xFF);
    USART1_SendByte(0x01);
    USART1_SendByte(0x00); USART1_SendByte(0x03);
    USART1_SendByte(0x05);
    USART1_SendByte(0x00); USART1_SendByte(0x09);
    return receive_finger(12);
}

int store(uint8_t ID)
{
    uint8_t sum1;
    sum1 = 0x0E + ID;
    USART1_SendByte(0xEF);USART1_SendByte(0x01);
    USART1_SendByte(0xFF);USART1_SendByte(0xFF);USART1_SendByte(0xFF);USART1_SendByte(0xFF);
    USART1_SendByte(0x01);
    USART1_SendByte(0x00);USART1_SendByte(0x06);
    USART1_SendByte(0x06);
    USART1_SendByte(0x01);
    USART1_SendByte(0x00);USART1_SendByte(ID);
    USART1_SendByte(0x00);USART1_SendByte(sum1);
    return receive_finger(12);
}

int search(void)
{
    USART1_SendByte(0xEF);USART1_SendByte(0x01);
    USART1_SendByte(0xFF);USART1_SendByte(0xFF);USART1_SendByte(0xFF);USART1_SendByte(0xFF);

    // kiem tra check sum tu day
    USART1_SendByte(0x01);
    USART1_SendByte(0x00);USART1_SendByte(0x08);
    USART1_SendByte(0x04);
    USART1_SendByte(0x01);
    USART1_SendByte(0x00);USART1_SendByte(0x00);// dia chi bat dau
    USART1_SendByte(0x00);USART1_SendByte(0xff);// dia chi ket thuc
    USART1_SendByte(0x01);USART1_SendByte(0x0D);// ma check sum dc tinh

    return receive_finger_search(16);
}

int search1(void)
{
    USART1_SendByte(0xEF);USART1_SendByte(0x01);
    USART1_SendByte(0xFF);USART1_SendByte(0xFF);USART1_SendByte(0xFF);USART1_SendByte(0xFF);

    // kiem tra check sum tu day
    USART1_SendByte(0x01);
    USART1_SendByte(0x00);USART1_SendByte(0x08);
    USART1_SendByte(0x04);
    USART1_SendByte(0x01);
    USART1_SendByte(0x00);USART1_SendByte(0x00);// dia chi bat dau
    USART1_SendByte(0x00);USART1_SendByte(0x01);// dia chi ket thuc
    USART1_SendByte(0x00);USART1_SendByte(0x0F);// ma check sum dc tinh

    return receive_finger_search(16);
}

int empty(void)
{
    USART1_SendByte(0xEF);USART1_SendByte(0x01);
    USART1_SendByte(0xFF);USART1_SendByte(0xFF);USART1_SendByte(0xFF);USART1_SendByte(0xFF);
    USART1_SendByte(0x01);
    USART1_SendByte(0x00);USART1_SendByte(0x03);
    USART1_SendByte(0x0D);
    USART1_SendByte(0x00);USART1_SendByte(0x11);
    return receive_finger(12);
}

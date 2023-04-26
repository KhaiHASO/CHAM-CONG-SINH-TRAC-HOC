#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "stdio.h"
#include "finger.h"

extern UART_HandleTypeDef huart1;
extern uint8_t pID;

static uint8_t receive_finger(uint8_t len)
{
    uint8_t p, D[20];
    int i = 0;
    while (i < len)
    {
        if (HAL_UART_Receive(&huart1, &D[i], 1, 100) == HAL_OK)
        {
            i++;
        }
    }
    p = D[len - 3];
    return p;
}

// ... (rest of the functions updated with HAL)

int collect_finger(void)
{
    uint8_t cmd[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x01, 0x00, 0x05};
    HAL_UART_Transmit(&huart1, cmd, sizeof(cmd), 100);
    return receive_finger(12);
}

// ... (rest of the functions updated with HAL)
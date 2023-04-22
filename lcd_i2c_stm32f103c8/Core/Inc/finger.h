#ifndef __FINGER_H
#define __FINGER_H

void UART1_Init(void);
#include <stdint.h>

/* Instruction codes */
#define GET_IMAGE_CMD 0x01
#define GET_IMAGE_ACK 0x07

/* Confirmation codes */
#define GET_IMAGE_SUCCESS 0x00
#define GET_IMAGE_ERROR_RX 0x01
#define GET_IMAGE_ERROR_NO_FINGER 0x02
#define GET_IMAGE_ERROR_FAIL 0x03

uint8_t Finger_GetImage(void);

#endif /* __FINGER_H */


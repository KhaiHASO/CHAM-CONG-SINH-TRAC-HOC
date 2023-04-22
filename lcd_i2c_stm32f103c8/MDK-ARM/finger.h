// finger.h
#ifndef __FINGER_H__
#define __FINGER_H__

#include "stdint.h"

extern uint8_t pID;

uint8_t receive_finger(uint8_t len);
uint8_t receive_finger_match(uint8_t len);
uint8_t receive_finger_search(uint8_t len);
int collect_finger(void);
int img2tz(uint8_t local);
int match(void);
int regmodel(void);
int store(uint8_t ID);
int search(void);
int search1(void);
int empty(void);

#endif // __FINGER_H__

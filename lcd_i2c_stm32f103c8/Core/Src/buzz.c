#include "buzz.h"

extern TIM_HandleTypeDef htim2; 
void beep(int duration_ms, int beeps) {
  //HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // bật tín hiệu PWM cho buzzer
  
  for(int i = 0; i <= beeps; i++) {
    __HAL_TIM_SET_AUTORELOAD(&htim2, 30); // thiết lập giá trị tự động nạp lại của Timer
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 15); // thiết lập giá trị so sánh của Timer
    HAL_Delay(duration_ms); // chờ thời gian tương ứng với tham số duration_ms
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2); // ngắt tín hiệu PWM cho buzzer
    HAL_Delay(duration_ms); // chờ thời gian tương ứng với tham số duration_ms
    if (i != beeps) { // chỉ bật lại tín hiệu PWM nếu chưa phát hết số lần beep
      HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // bật lại tín hiệu PWM cho buzzer
    }
  }
  
  HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2); // ngắt tín hiệu PWM cho buzzer
}


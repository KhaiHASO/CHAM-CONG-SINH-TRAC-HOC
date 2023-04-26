#include "joystick.h"


uint32_t joystick_X, joystick_Y, value[2];
uint8_t button;

uint32_t MAP(uint32_t au32_IN, uint32_t au32_INmin, uint32_t au32_INmax, uint32_t au32_OUTmin, uint32_t au32_OUTmax)
{
    return ((((au32_IN - au32_INmin)*(au32_OUTmax - au32_OUTmin))/(au32_INmax - au32_INmin)) + au32_OUTmin);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    joystick_X = MAP(value[0], 0, 4096, 0, 100);
    joystick_Y = MAP(value[1], 0, 4096, 0, 100);
}
void joystick_init(ADC_HandleTypeDef* hadc)
{
    // Khởi tạo các thiết bị cần thiết
    HAL_ADC_Start_DMA(hadc, (uint32_t *)value, 2);
}

uint8_t get_joystick_position(void)
{

    button = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);
    if(button == 0) {
        return 1; // Nút được nhấn
    }
    else if(joystick_X < 10) {
        return 2; // Joystick ở vị trí bên trái
    }
    else if(joystick_X > 90) {
        return 3; // Joystick ở vị trí bên phải
    }
    else {
        return 0; // Joystick ở vị trí giữa
    }
}

char* get_joystick_position_string(void)
{
    static char result[50]; // chuỗi kết quả
    memset(result, 0, sizeof(result)); // xóa dữ liệu trong chuỗi
    
    // tính toán giá trị X và Y
    uint32_t joystick_X = MAP(value[0], 0, 4096, 0, 100);
    uint32_t joystick_Y = MAP(value[1], 0, 4096, 0, 100);
    
    // ghi kết quả vào chuỗi
    sprintf(result, "Joystick X: %lu, Y: %lu", (unsigned long)joystick_X, (unsigned long)joystick_Y);
    return result; // trả về chuỗi kết quả
}



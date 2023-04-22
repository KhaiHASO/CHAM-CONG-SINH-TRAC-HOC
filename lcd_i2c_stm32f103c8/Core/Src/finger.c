#include "finger.h"
#include "stm32f10x.h"

#define HEADER 0xEF01
#define GET_IMAGE 0x01

uint8_t get_image(void)
{
    uint8_t confirmation_code = 0xFF; // default value in case of communication error
    uint16_t package_len = 0x03;
    uint8_t instruction_code = GET_IMAGE;
    uint16_t checksum = HEADER + package_len + instruction_code;

    // Prepare the package to send to the module
    uint8_t package[] = {HEADER >> 8, HEADER & 0xFF, 0x00, 0x00, package_len, instruction_code, checksum >> 8, checksum & 0xFF};

    // Send the package to the module
    // ...

    // Receive and process the acknowledgement package
    uint8_t ack_package[8];
    // Receive the acknowledgement package and store it in the ack_package buffer
    // ...

    // Check the confirmation code in the acknowledgement package
    if (ack_package[4] == 0x00)
    {
        confirmation_code = SUCCESS;
    }
    else if (ack_package[4] == 0x02)
    {
        confirmation_code = CANT_DETECT_FINGER;
    }
    else if (ack_package[4] == 0x03)
    {
        confirmation_code = FAIL_TO_COLLECT_FINGER;
    }
    else
    {
        confirmation_code = RECEIVE_ERR;
    }

    return confirmation_code;
}

#include <stdint.h>
// Define constant values for command and confirmation codes
#define FINGER_CMD_GET_IMAGE 0x01
#define FINGER_CONF_SUCCESS 0x00
#define FINGER_CONF_ERR_PACKAGE 0x01
#define FINGER_CONF_NO_FINGER 0x02
#define FINGER_CONF_FAIL_COLLECT 0x03
void UART1_Init(void);
// Define structure for package content
typedef struct {
    uint8_t instructionCode;
} FingerPackageContent;

// Define structure for acknowledgement package
typedef struct {
    uint8_t confirmationCode;
} FingerAcknowledgePackage;

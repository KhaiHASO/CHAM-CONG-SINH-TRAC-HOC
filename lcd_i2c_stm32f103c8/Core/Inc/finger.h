#ifndef FINGER_H
#define FINGER_H

#include <stdint.h>

#define FINGER_HEADER 0xEF01
#define RECEIVE_ERR 0x01
#define CANT_DETECT_FINGER 0x02
#define FAIL_TO_COLLECT_FINGER 0x03

#define FINGER_IMAGE_BUFFER 0x01
#define FINGER_CHAR_BUFFER1 0x01
#define FINGER_CHAR_BUFFER2 0x02

#define FINGER_GETIMAGE 0x01
#define FINGER_GENCHAR 0x02
#define FINGER_SEARCH 0x04
#define FINGER_REGMODEL 0x05
#define FINGER_STORE 0x06
#define FINGER_DELETECHAR 0x0C
#define FINGER_EMPTY 0x0D
#define FINGER_VERIFY_PASSWORD 0x13

#define FINGER_SUCCESS 0x00
#define FINGER_ERROR_RECEIVING_PACKAGE 0x01
#define FINGER_CANNOT_DETECT_FINGER 0x02
#define FINGER_FAIL_TO_COLLECT_FINGER 0x03
#define FINGER_OVER_DISORDERLY_FINGERPRINT_IMAGE 0x06
#define FINGER_OVER_SMALLNESS_FINGERPRINT_IMAGE 0x07
#define FINGER_FAIL_TO_GENERATE_IMAGE 0x15
#define FINGER_NO_MATCH_IN_LIBRARY 0x09
#define FINGER_FAIL_TO_COMBINE_CHARACTER_FILES 0x0A
#define FINGER_ADDRESSING_PAGEID_BEYOND_LIBRARY 0x0B
#define FINGER_ERROR_WHEN_WRITING_FLASH 0x18
#define FINGER_FAIL_TO_DELETE_TEMPLATES 0x10
#define FINGER_FAIL_TO_CLEAR_FINGER_LIBRARY 0x11
#define FINGER_WRONG_PASSWORD 0x13

typedef struct {
    uint16_t header;
    uint32_t packageID;
    uint8_t packageLength;
    uint16_t instructionCode;
    uint8_t checksum;
} finger_command_t;

typedef struct {
    uint16_t header;
    uint32_t packageID;
    uint8_t packageLength;
    uint8_t confirmationCode;
    uint8_t checksum;
} finger_response_t;

uint8_t get_image(void);
uint8_t finger_gen_char(uint8_t bufferID);
uint8_t finger_search(uint8_t bufferID, uint16_t startPage, uint16_t pageNum, uint16_t *pageID, uint16_t *matchScore);
uint8_t finger_reg_model(void);
uint8_t finger_store(uint8_t bufferID, uint16_t pageID);
uint8_t finger_delete_char(uint16_t pageID, uint16_t N);
uint8_t finger_empty(void);
uint8_t finger_verify_password(uint32_t password);

#endif // FINGER_H

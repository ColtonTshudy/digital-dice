/* FT6336 Touch Screen Controller Driver for STM32F401CC */
#ifndef __FT6336_H
#define __FT6336_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f4xx_hal.h"

/* FT6336 I2C Address */
#define FT6336_I2C_ADDRESS          0x38

/* FT6336 Registers */
#define FT6336_REG_DEV_MODE         0x00
#define FT6336_REG_GEST_ID          0x01
#define FT6336_REG_TD_STATUS        0x02
#define FT6336_REG_P1_XH            0x03
#define FT6336_REG_P1_XL            0x04
#define FT6336_REG_P1_YH            0x05
#define FT6336_REG_P1_YL            0x06
#define FT6336_REG_P1_WEIGHT        0x07
#define FT6336_REG_P1_MISC          0x08
#define FT6336_REG_P2_XH            0x09
#define FT6336_REG_P2_XL            0x0A
#define FT6336_REG_P2_YH            0x0B
#define FT6336_REG_P2_YL            0x0C

#define FT6336_REG_CTRL             0x86
#define FT6336_REG_TIMEENTERMONITOR 0x87
#define FT6336_REG_PERIODACTIVE     0x88
#define FT6336_REG_PERIODMONITOR    0x89
#define FT6336_REG_CIPHER_MID       0x9F
#define FT6336_REG_CIPHER_LOW       0xA0
#define FT6336_REG_LIB_VERSION      0xA1
#define FT6336_REG_CHIPID           0xA3
#define FT6336_REG_FIRMID           0xA6
#define FT6336_REG_FOCALTECH_ID     0xA8

/* FT6336 Expected Chip IDs */
#define FT6336_FOCALTECH_ID         0x11
#define FT6336_CIPHER_MID           0x26
#define FT6336_CHIPID               0x64

/* I2C Timeout */
#define FT6336_I2C_TIMEOUT          100

/* Hardware Reset Pin (PB15) */
#define FT6336_RST_PIN              GPIO_PIN_15
#define FT6336_RST_PORT             GPIOB

/* Function Prototypes */
void     ft6336_InitResetPin(void);
void     ft6336_Init(uint16_t DeviceAddr);
uint16_t ft6336_ReadID(uint16_t DeviceAddr);
void     ft6336_Reset(uint16_t DeviceAddr);
void     ft6336_Start(uint16_t DeviceAddr);
uint8_t  ft6336_DetectTouch(uint16_t DeviceAddr);
void     ft6336_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y);
void     ft6336_EnableIT(uint16_t DeviceAddr);
void     ft6336_ClearIT(uint16_t DeviceAddr);
uint8_t  ft6336_GetITStatus(uint16_t DeviceAddr);
void     ft6336_DisableIT(uint16_t DeviceAddr);
uint8_t  ft6336_Scan(uint16_t DeviceAddr);

// #define FT6336_DEBUG 1
#ifdef FT6336_DEBUG
#include <stdio.h>
#define FT6336_DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define FT6336_DEBUG_PRINT(...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __FT6336_H */
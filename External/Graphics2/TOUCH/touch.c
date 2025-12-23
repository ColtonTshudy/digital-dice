#include "touch.h"
#include "lcd.h"

extern I2C_HandleTypeDef hi2c2; // Adjust to your I2C handle

#define FT_ADDR 0x70
#define FT_REG_NUM_FINGER 0x02
#define FT_TP1_REG 0x03
#define FT_TP2_REG 0x09
#define FT_ID_G_FOCALTECH_ID 0xA8

_m_tp_dev tp_dev = {
    TP_Init,
    NULL,
    {0},
    {0},
    0
};

static const uint16_t FT6336_TPX_TBL[2] = {FT_TP1_REG, FT_TP2_REG};

static uint8_t FT6336_WriteReg(uint8_t reg, uint8_t *buf, uint8_t len) {
    uint8_t data[len + 1];
    data[0] = reg;
    for (uint8_t i = 0; i < len; i++) {
        data[i + 1] = buf[i];
    }
    return HAL_I2C_Master_Transmit(&hi2c2, FT_ADDR, data, len + 1, HAL_MAX_DELAY);
}

static void FT6336_ReadReg(uint8_t reg, uint8_t *buf, uint8_t len) {
    HAL_I2C_Master_Transmit(&hi2c2, FT_ADDR, &reg, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c2, FT_ADDR | 0x01, buf, len, HAL_MAX_DELAY);
}

uint8_t TP_Init(void) {
    HAL_GPIO_WritePin(CTP_RST_GPIO_Port, CTP_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(CTP_RST_GPIO_Port, CTP_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(500);

    uint8_t temp;
    FT6336_ReadReg(FT_ID_G_FOCALTECH_ID, &temp, 1);
    if (temp != 0x11) return 1;

    tp_dev.scan = FT6336_Scan;
    return 0;
}

uint8_t FT6336_Scan(void) {
    uint8_t buf[4];
    uint8_t mode;
    static uint8_t t = 0;

    t++;
    if ((t % 10) == 0 || t < 10) {
        FT6336_ReadReg(FT_REG_NUM_FINGER, &mode, 1);
        
        if (mode && (mode < 3)) {
            uint8_t temp = 0xFF << mode;
            tp_dev.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
            
            for (uint8_t i = 0; i < CTP_MAX_TOUCH; i++) {
                FT6336_ReadReg(FT6336_TPX_TBL[i], buf, 4);
                
                if (tp_dev.sta & (1 << i)) {
                    tp_dev.x[i] = ((uint16_t)(buf[0] & 0x0F) << 8) + buf[1];
                    tp_dev.y[i] = ((uint16_t)(buf[2] & 0x0F) << 8) + buf[3];
                }
            }
            
            if (tp_dev.x[0] == 0 && tp_dev.y[0] == 0) mode = 0;
            t = 0;
            return 1;
        }
    }

    if (mode == 0) {
        if (tp_dev.sta & TP_PRES_DOWN) {
            tp_dev.sta &= ~(1 << 7);
        } else {
            tp_dev.x[0] = 0xFFFF;
            tp_dev.y[0] = 0xFFFF;
            tp_dev.sta &= 0xE0;
        }
    }

    if (t > 240) t = 10;
    return 0;
}

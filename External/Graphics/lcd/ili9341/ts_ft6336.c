/*
 * FT6336 touch driver STM32F4
 * I2C capacitive touch controller
 * Converted from XPT2046 driver by Roberto Benjami
 *
 * - hardware I2C
 */
#include <stdlib.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
#include "main.h"
#include "lcd.h"
#include "ts_ft6336.h"

//=============================================================================
/* FT6336 Register addresses */
#define FT6336_REG_MODE 0x00
#define FT6336_REG_TD_STATUS 0x02
#define FT6336_REG_TOUCH1_XH 0x03
#define FT6336_REG_TOUCH1_XL 0x04
#define FT6336_REG_TOUCH1_YH 0x05
#define FT6336_REG_TOUCH1_YL 0x06
#define FT6336_REG_TOUCH2_XH 0x09
#define FT6336_REG_TOUCH2_XL 0x0A
#define FT6336_REG_TOUCH2_YH 0x0B
#define FT6336_REG_TOUCH2_YL 0x0C

//=============================================================================
#define BITBAND_ACCESS(a, b)                                                  \
  *(volatile uint32_t *)(((uint32_t)&a & 0xF0000000) + 0x2000000              \
                         + (((uint32_t)&a & 0x000FFFFF) << 5) + (b << 2))

//-----------------------------------------------------------------------------
/* GPIO mode */
#define MODE_DIGITAL_INPUT 0x0
#define MODE_OUT 0x1
#define MODE_ALTER 0x2
#define MODE_ANALOG_INPUT 0x3

#define MODE_SPD_LOW 0x0
#define MODE_SPD_MEDIUM 0x1
#define MODE_SPD_HIGH 0x2
#define MODE_SPD_VHIGH 0x3

#define MODE_OT_PP 0x0
#define MODE_OT_OD 0x1

#define MODE_PU_NONE 0x0
#define MODE_PU_UP 0x1
#define MODE_PU_DOWN 0x2

#define GPIOX_PORT_(a, b) GPIO##a
#define GPIOX_PORT(a) GPIOX_PORT_(a)

#define GPIOX_PIN_(a, b) b
#define GPIOX_PIN(a) GPIOX_PIN_(a)

#define GPIOX_AFR_(a, b, c)                                                   \
  GPIO##b->AFR[c >> 3] = (GPIO##b->AFR[c >> 3] & ~(0x0F << (4 * (c & 7))))    \
                         | (a << (4 * (c & 7)));
#define GPIOX_AFR(a, b) GPIOX_AFR_(a, b)

#define GPIOX_MODER_(a, b, c)                                                 \
  GPIO##b->MODER = (GPIO##b->MODER & ~(3 << (2 * c))) | (a << (2 * c));
#define GPIOX_MODER(a, b) GPIOX_MODER_(a, b)

#define GPIOX_OTYPER_(a, b, c)                                                \
  GPIO##b->OTYPER = (GPIO##b->OTYPER & ~(1 << c)) | (a << c);
#define GPIOX_OTYPER(a, b) GPIOX_OTYPER_(a, b)

#define GPIOX_OSPEEDR_(a, b, c)                                               \
  GPIO##b->OSPEEDR = (GPIO##b->OSPEEDR & ~(3 << (2 * c))) | (a << (2 * c));
#define GPIOX_OSPEEDR(a, b) GPIOX_OSPEEDR_(a, b)

#define GPIOX_PUPDR_(a, b, c)                                                 \
  GPIO##b->PUPDR = (GPIO##b->PUPDR & ~(3 << (2 * c))) | (a << (2 * c));
#define GPIOX_PUPDR(a, b) GPIOX_PUPDR_(a, b)

#define GPIOX_ODR_(a, b) BITBAND_ACCESS(GPIO##a->ODR, b)
#define GPIOX_ODR(a) GPIOX_ODR_(a)

#define GPIOX_IDR_(a, b) BITBAND_ACCESS(GPIO##a->IDR, b)
#define GPIOX_IDR(a) GPIOX_IDR_(a)

#define GPIOX_CLOCK_(a, b) RCC_AHB1ENR_GPIO##a##EN
#define GPIOX_CLOCK(a) GPIOX_CLOCK_(a)

#define GPIOX_PORTNUM_A 1
#define GPIOX_PORTNUM_B 2
#define GPIOX_PORTNUM_C 3
#define GPIOX_PORTNUM_D 4
#define GPIOX_PORTNUM_E 5
#define GPIOX_PORTNUM_F 6
#define GPIOX_PORTNUM_G 7
#define GPIOX_PORTNUM_H 8
#define GPIOX_PORTNUM_I 9
#define GPIOX_PORTNUM_J 10
#define GPIOX_PORTNUM_K 11
#define GPIOX_PORTNUM_(a, b) GPIOX_PORTNUM_##a
#define GPIOX_PORTNUM(a) GPIOX_PORTNUM_(a)

//=============================================================================
#ifdef __GNUC__
#pragma GCC push_options
#pragma GCC optimize("O0")
#elif defined(__CC_ARM)
#pragma push
#pragma O0
#endif
void TS_IO_Delay(uint32_t c)
{
  while(c--)
    ;
}
#ifdef __GNUC__
#pragma GCC pop_options
#elif defined(__CC_ARM)
#pragma pop
#endif

//=============================================================================
/* Hardware I2C */
#if TS_I2C == 1
#define I2CX I2C1
#define TS_I2C_RCC_EN BITBAND_ACCESS(RCC->APB1ENR, RCC_APB1ENR_I2C1EN_Pos) = 1
#elif TS_I2C == 2
#define I2CX I2C2
#define TS_I2C_RCC_EN BITBAND_ACCESS(RCC->APB1ENR, RCC_APB1ENR_I2C2EN_Pos) = 1
#elif TS_I2C == 3
#define I2CX I2C3
#define TS_I2C_RCC_EN BITBAND_ACCESS(RCC->APB1ENR, RCC_APB1ENR_I2C3EN_Pos) = 1
#endif

//-----------------------------------------------------------------------------
/* I2C write single byte to register */
void I2C_WriteReg(uint8_t reg, uint8_t data)
{
  FT6336_DEBUG("Write reg 0x%02X = 0x%02X", reg, data);
  while(BITBAND_ACCESS(I2CX->SR2, I2C_SR2_BUSY_Pos))
    ;

  I2CX->CR1 |= I2C_CR1_START;
  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_SB_Pos))
    ;

  I2CX->DR = (FT6336_I2C_ADDR << 1);
  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_ADDR_Pos))
    ;
  (void)I2CX->SR2;

  I2CX->DR = reg;
  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_TXE_Pos))
    ;

  I2CX->DR = data;
  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_TXE_Pos))
    ;

  I2CX->CR1 |= I2C_CR1_STOP;
}

//-----------------------------------------------------------------------------
/* I2C read single byte from register */
uint8_t I2C_ReadReg(uint8_t reg)
{
  uint8_t data;

  while(BITBAND_ACCESS(I2CX->SR2, I2C_SR2_BUSY_Pos))
    ;

  I2CX->CR1 |= I2C_CR1_START;
  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_SB_Pos))
    ;

  I2CX->DR = (FT6336_I2C_ADDR << 1);
  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_ADDR_Pos))
    ;
  (void)I2CX->SR2;

  I2CX->DR = reg;
  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_TXE_Pos))
    ;

  I2CX->CR1 |= I2C_CR1_START;
  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_SB_Pos))
    ;

  I2CX->DR = (FT6336_I2C_ADDR << 1) | 0x01;
  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_ADDR_Pos))
    ;

  I2CX->CR1 &= ~I2C_CR1_ACK;
  (void)I2CX->SR2;
  I2CX->CR1 |= I2C_CR1_STOP;

  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_RXNE_Pos))
    ;
  data = I2CX->DR;

  FT6336_DEBUG("Read reg 0x%02X = 0x%02X", reg, data);
  return data;
}

//-----------------------------------------------------------------------------
/* I2C read multiple bytes from register */
void I2C_ReadMultiple(uint8_t reg, uint8_t *buffer, uint8_t len)
{
  while(BITBAND_ACCESS(I2CX->SR2, I2C_SR2_BUSY_Pos))
    ;

  I2CX->CR1 |= I2C_CR1_START;
  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_SB_Pos))
    ;

  I2CX->DR = (FT6336_I2C_ADDR << 1);
  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_ADDR_Pos))
    ;
  (void)I2CX->SR2;

  I2CX->DR = reg;
  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_TXE_Pos))
    ;

  I2CX->CR1 |= I2C_CR1_START;
  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_SB_Pos))
    ;

  I2CX->DR = (FT6336_I2C_ADDR << 1) | 0x01;
  while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_ADDR_Pos))
    ;

  I2CX->CR1 |= I2C_CR1_ACK;
  (void)I2CX->SR2;

  while(len > 0)
    {
      if(len == 1)
        {
          I2CX->CR1 &= ~I2C_CR1_ACK;
          I2CX->CR1 |= I2C_CR1_STOP;
        }

      while(!BITBAND_ACCESS(I2CX->SR1, I2C_SR1_RXNE_Pos))
        ;
      *buffer++ = I2CX->DR;
      len--;
    }
}

//-----------------------------------------------------------------------------
void TS_IO_Init(void)
{
  FT6336_DEBUG("Initializing I2C%d...", TS_I2C);
  uint32_t pclk1;

#if GPIOX_PORTNUM(TS_IRQ) >= GPIOX_PORTNUM_A
#define GPIOX_CLOCK_TS_IRQ GPIOX_CLOCK(TS_IRQ)
#else
#define GPIOX_CLOCK_TS_IRQ 0
#endif

#if GPIOX_PORTNUM(TS_RST) >= GPIOX_PORTNUM_A
#define GPIOX_CLOCK_TS_RST GPIOX_CLOCK(TS_RST)
#else
#define GPIOX_CLOCK_TS_RST 0
#endif

  RCC->AHB1ENR |= GPIOX_CLOCK(TS_SCL) | GPIOX_CLOCK(TS_SDA)
                  | GPIOX_CLOCK_TS_IRQ | GPIOX_CLOCK_TS_RST;

  /* Configure I2C pins */
  GPIOX_AFR(TS_I2C_AFR, TS_SCL);
  GPIOX_MODER(MODE_ALTER, TS_SCL);
  GPIOX_OTYPER(MODE_OT_OD, TS_SCL);
  GPIOX_OSPEEDR(MODE_SPD_VHIGH, TS_SCL);
  GPIOX_PUPDR(MODE_PU_UP, TS_SCL);

  GPIOX_AFR(TS_I2C_AFR, TS_SDA);
  GPIOX_MODER(MODE_ALTER, TS_SDA);
  GPIOX_OTYPER(MODE_OT_OD, TS_SDA);
  GPIOX_OSPEEDR(MODE_SPD_VHIGH, TS_SDA);
  GPIOX_PUPDR(MODE_PU_UP, TS_SDA);

/* Configure optional pins */
#if GPIOX_PORTNUM(TS_IRQ) >= GPIOX_PORTNUM_A
  GPIOX_MODER(MODE_DIGITAL_INPUT, TS_IRQ);
  GPIOX_PUPDR(MODE_PU_UP, TS_IRQ);
#endif

#if GPIOX_PORTNUM(TS_RST) >= GPIOX_PORTNUM_A
  GPIOX_MODER(MODE_OUT, TS_RST);
  GPIOX_OSPEEDR(MODE_SPD_VHIGH, TS_RST);
  GPIOX_ODR(TS_RST) = 0;
  TS_IO_Delay(10000);
  GPIOX_ODR(TS_RST) = 1;
  TS_IO_Delay(10000);
#endif

  /* Enable I2C clock */
  TS_I2C_RCC_EN;

  /* Reset I2C */
  I2CX->CR1 = I2C_CR1_SWRST;
  I2CX->CR1 = 0;

  /* Get PCLK1 frequency */
  pclk1 = SystemCoreClock
          >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos];

  /* Configure I2C timing */
  I2CX->CR2 = (pclk1 / 1000000) & 0x3F;

#if TS_I2C_SPEED == 0
  /* 100kHz standard mode */
  I2CX->CCR = (pclk1 / (100000 * 2)) & 0xFFF;
  I2CX->TRISE = ((pclk1 / 1000000) + 1) & 0x3F;
#else
  /* 400kHz fast mode */
  I2CX->CCR = I2C_CCR_FS | ((pclk1 / (400000 * 3)) & 0xFFF);
  I2CX->TRISE = (((pclk1 / 1000000) * 300) / 1000 + 1) & 0x3F;
#endif

  /* Enable I2C */
  I2CX->CR1 = I2C_CR1_PE;

  /* Small delay for initialization */
  TS_IO_Delay(10000);

  FT6336_DEBUG("I2C initialized, PCLK1=%lu Hz", pclk1);
  FT6336_DEBUG("I2C setup complete");
}

//-----------------------------------------------------------------------------
/* Read X coordinate of touch point */
uint16_t TS_IO_GetX(void)
{
  uint8_t data[2];
  I2C_ReadMultiple(FT6336_REG_TOUCH1_XH, data, 2);
  uint16_t ret =  ((data[0] & 0x0F) << 8) | data[1];
  FT6336_DEBUG("Y = %d", ret);
  return ret;
}

//-----------------------------------------------------------------------------
/* Read Y coordinate of touch point */
uint16_t TS_IO_GetY(void)
{
  uint8_t data[2];
  I2C_ReadMultiple(FT6336_REG_TOUCH1_YH, data, 2);
  uint16_t ret =  ((data[0] & 0x0F) << 8) | data[1];
  FT6336_DEBUG("Y = %d", ret);
  return ret;
}

//-----------------------------------------------------------------------------
/* Read second touch point X coordinate */
uint16_t TS_IO_GetX2(void)
{
  uint8_t data[2];
  I2C_ReadMultiple(FT6336_REG_TOUCH2_XH, data, 2);
  return ((data[0] & 0x0F) << 8) | data[1];
}

//-----------------------------------------------------------------------------
/* Read second touch point Y coordinate */
uint16_t TS_IO_GetY2(void)
{
  uint8_t data[2];
  I2C_ReadMultiple(FT6336_REG_TOUCH2_YH, data, 2);
  return ((data[0] & 0x0F) << 8) | data[1];
}

//-----------------------------------------------------------------------------
/* Get number of active touch points */
uint8_t TS_IO_GetTouchPoints(void)
{
  uint8_t points = I2C_ReadReg(FT6336_REG_TD_STATUS);
  return points & 0x0F;
}

//-----------------------------------------------------------------------------
/* Detect if touchscreen is pressed
   return:
   - 0 : touchscreen is not pressed
   - 1 : touchscreen is pressed */
uint8_t TS_IO_DetectToch(void)
{
  static uint8_t ts_inited = 0;

  if(!ts_inited)
    {
      TS_IO_Init();
      ts_inited = 1;
    }

#if GPIOX_PORTNUM(TS_IRQ) >= GPIOX_PORTNUM_A
  /* Use interrupt pin if configured */
  if(!GPIOX_IDR(TS_IRQ))
    return 1;
  else
    return 0;
#else
  /* Poll touch status register */
  if(TS_IO_GetTouchPoints() > 0)
    return 1;
  else
    return 0;
#endif
}

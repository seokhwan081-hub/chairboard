/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef signed long  s32;
typedef signed short s16;
typedef signed char  s8;

typedef signed long  const sc32;  /* Read Only */
typedef signed short const sc16;  /* Read Only */
typedef signed char  const sc8;   /* Read Only */

typedef volatile signed long  vs32;
typedef volatile signed short vs16;
typedef volatile signed char  vs8;

typedef volatile signed long  const vsc32;  /* Read Only */
typedef volatile signed short const vsc16;  /* Read Only */
typedef volatile signed char  const vsc8;   /* Read Only */

typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;

typedef unsigned long  const uc32;  /* Read Only */
typedef unsigned short const uc16;  /* Read Only */
typedef unsigned char  const uc8;   /* Read Only */

typedef volatile unsigned long  vu32;
typedef volatile unsigned short vu16;
typedef volatile unsigned char  vu8;

typedef volatile unsigned long  const vuc32;  /* Read Only */
typedef volatile unsigned short const vuc16;  /* Read Only */
typedef volatile unsigned char  const vuc8;   /* Read Only */

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

#define U8_MAX     ((u8)255)
#define S8_MAX     ((s8)127)
#define S8_MIN     ((s8)-128)
#define U16_MAX    ((u16)65535u)
#define S16_MAX    ((s16)32767)
#define S16_MIN    ((s16)-32768)
#define U32_MAX    ((u32)4294967295uL)
#define S32_MAX    ((s32)2147483647)
#define S32_MIN    ((s32)-2147483648)
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DRV1_IN3_Pin LL_GPIO_PIN_2
#define DRV1_IN3_GPIO_Port GPIOE
#define DRV1_IN4_Pin LL_GPIO_PIN_3
#define DRV1_IN4_GPIO_Port GPIOE
#define DRV1_nSLEEP_Pin LL_GPIO_PIN_4
#define DRV1_nSLEEP_GPIO_Port GPIOE
#define DRV1_nFAULT_Pin LL_GPIO_PIN_5
#define DRV1_nFAULT_GPIO_Port GPIOE
#define DRV2_IN1_Pin LL_GPIO_PIN_13
#define DRV2_IN1_GPIO_Port GPIOC
#define DRV2_IN2_Pin LL_GPIO_PIN_14
#define DRV2_IN2_GPIO_Port GPIOC
#define DRV1_IPROP1_Pin LL_GPIO_PIN_0
#define DRV1_IPROP1_GPIO_Port GPIOC
#define DRV1_IPROP2_Pin LL_GPIO_PIN_1
#define DRV1_IPROP2_GPIO_Port GPIOC
#define DRV2_IPROP1_Pin LL_GPIO_PIN_2
#define DRV2_IPROP1_GPIO_Port GPIOC
#define DRV2_IPROP2_Pin LL_GPIO_PIN_3
#define DRV2_IPROP2_GPIO_Port GPIOC
#define DRV3_IPROP1_Pin LL_GPIO_PIN_0
#define DRV3_IPROP1_GPIO_Port GPIOA
#define SENSOR_CHAIR1_Pin LL_GPIO_PIN_4
#define SENSOR_CHAIR1_GPIO_Port GPIOA
#define SENSOR_CHAIR2_Pin LL_GPIO_PIN_5
#define SENSOR_CHAIR2_GPIO_Port GPIOA
#define SENSOR_ROBOT_Pin LL_GPIO_PIN_6
#define SENSOR_ROBOT_GPIO_Port GPIOA
#define AD_POT_CHAIR1_Pin LL_GPIO_PIN_7
#define AD_POT_CHAIR1_GPIO_Port GPIOA
#define AD_POT_CHAIR2_Pin LL_GPIO_PIN_4
#define AD_POT_CHAIR2_GPIO_Port GPIOC
#define DRV2_nSLEEP_Pin LL_GPIO_PIN_0
#define DRV2_nSLEEP_GPIO_Port GPIOB
#define DRV2_nFAULT_Pin LL_GPIO_PIN_2
#define DRV2_nFAULT_GPIO_Port GPIOB
#define DRV2_IN3_Pin LL_GPIO_PIN_7
#define DRV2_IN3_GPIO_Port GPIOE
#define DRV2_IN4_Pin LL_GPIO_PIN_8
#define DRV2_IN4_GPIO_Port GPIOE
#define LMT_CHAIR2_FRT_Pin LL_GPIO_PIN_10
#define LMT_CHAIR2_FRT_GPIO_Port GPIOE
#define LMT_CHAIR2_BACK_Pin LL_GPIO_PIN_11
#define LMT_CHAIR2_BACK_GPIO_Port GPIOE
#define LMT_CHAIR2_DW_Pin LL_GPIO_PIN_13
#define LMT_CHAIR2_DW_GPIO_Port GPIOE

#define BOARD_B //해당 코드 주석 처리로 V3.0에 쓸지 V3.1에 쓸지 선택

#ifdef BOARD_B
#define SW_CHAIR2_FWD_Pin LL_GPIO_PIN_11
#define SW_CHAIR2_FWD_GPIO_Port GPIOA
#define SW_CHAIR2_BACK_Pin LL_GPIO_PIN_10
#define SW_CHAIR2_BACK_GPIO_Port GPIOA
#define SW_CHAIR2_UP_Pin LL_GPIO_PIN_9
#define SW_CHAIR2_UP_GPIO_Port GPIOA
#else
#define SW_CHAIR2_FWD_Pin LL_GPIO_PIN_14
#define SW_CHAIR2_FWD_GPIO_Port GPIOE
#define SW_CHAIR2_BACK_Pin LL_GPIO_PIN_15
#define SW_CHAIR2_BACK_GPIO_Port GPIOE
#define SW_CHAIR2_UP_Pin LL_GPIO_PIN_10
#define SW_CHAIR2_UP_GPIO_Port GPIOB
#endif
#ifdef BOARD_B
#define I2C_SCL_Pin LL_GPIO_PIN_15
#define I2C_SCL_GPIO_Port GPIOE
#define I2C_SDA_Pin LL_GPIO_PIN_14
#define I2C_SDA_GPIO_Port GPIOE
#define SW_CHAIR2_DW_Pin LL_GPIO_PIN_8
#define SW_CHAIR2_DW_GPIO_Port GPIOA
#else
#define I2C_SCL_Pin LL_GPIO_PIN_11
#define I2C_SCL_GPIO_Port GPIOB
#define I2C_SDA_Pin LL_GPIO_PIN_12
#define I2C_SDA_GPIO_Port GPIOB
#define SW_CHAIR2_DW_Pin LL_GPIO_PIN_13
#define SW_CHAIR2_DW_GPIO_Port GPIOB
#endif
#define CpuRun_Pin LL_GPIO_PIN_14
#define CpuRun_GPIO_Port GPIOB
#define DRV3_nSLEEP_Pin LL_GPIO_PIN_8
#define DRV3_nSLEEP_GPIO_Port GPIOD
#define DRV3_IN1_Pin LL_GPIO_PIN_9
#define DRV3_IN1_GPIO_Port GPIOD
#define DRV3_IN2_Pin LL_GPIO_PIN_10
#define DRV3_IN2_GPIO_Port GPIOD
#define DRV3_nFAULT_Pin LL_GPIO_PIN_11
#define DRV3_nFAULT_GPIO_Port GPIOD
#define LMT_ROBOT_DW_Pin LL_GPIO_PIN_6
#define LMT_ROBOT_DW_GPIO_Port GPIOC
#define LMT_ROBOT_UP_Pin LL_GPIO_PIN_7
#define LMT_ROBOT_UP_GPIO_Port GPIOC
#ifdef BOARD_B
#define SW_ROBOT_DW_Pin LL_GPIO_PIN_8
#define SW_ROBOT_DW_GPIO_Port GPIOC
#define SW_ROBOT_UP_Pin LL_GPIO_PIN_9
#define SW_ROBOT_UP_GPIO_Port GPIOC
#else
#define SW_ROBOT_DW_Pin LL_GPIO_PIN_8
#define SW_ROBOT_DW_GPIO_Port GPIOA
#define SW_ROBOT_UP_Pin LL_GPIO_PIN_9
#define SW_ROBOT_UP_GPIO_Port GPIOA
#endif
#define SW_CHAIR1_OK_Pin LL_GPIO_PIN_10
#define SW_CHAIR1_OK_GPIO_Port GPIOA
#ifdef BOARD_B
#define SW_CHAIR1_DW_Pin LL_GPIO_PIN_12
#define SW_CHAIR1_DW_GPIO_Port GPIOC
#else
#define SW_CHAIR1_DW_Pin LL_GPIO_PIN_11
#define SW_CHAIR1_DW_GPIO_Port GPIOA
#endif
#define SW_CHAIR1_UP_Pin LL_GPIO_PIN_0
#define SW_CHAIR1_UP_GPIO_Port GPIOD
#define SW_CHAIR1_BACK_Pin LL_GPIO_PIN_1
#define SW_CHAIR1_BACK_GPIO_Port GPIOD
#define SW_CHAIR1_FWD_Pin LL_GPIO_PIN_2
#define SW_CHAIR1_FWD_GPIO_Port GPIOD
#define LMT_CHAIR1_DW_Pin LL_GPIO_PIN_3
#define LMT_CHAIR1_DW_GPIO_Port GPIOD
#define LMT_CHAIR1_BACK_Pin LL_GPIO_PIN_5
#define LMT_CHAIR1_BACK_GPIO_Port GPIOD
#define LMT_CHAIR1_FRT_Pin LL_GPIO_PIN_5
#define LMT_CHAIR1_FRT_GPIO_Port GPIOB
#define DRV1_IN1_Pin LL_GPIO_PIN_0
#define DRV1_IN1_GPIO_Port GPIOE
#define DRV1_IN2_Pin LL_GPIO_PIN_1
#define DRV1_IN2_GPIO_Port GPIOE
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

/* USER CODE BEGIN Private defines */

//#define MOTOR_MULTI_MOVE
//#define HW_TEST
//#define RMC_BLACK

#define EEPROM_ADDR_FW_VERSION1										0
#define EEPROM_ADDR_FW_VERSION2										1

#define ACTIVE_LOW			0
#define INACTIVE_HIGH		1

#define HIGH_ACTIVE		1
#define LOW_INACTIVE     0

#define SENS_MARGIN	0
#define PRE_STOP		0

#define INCREASE			0
#define DECREASE	1

#define MOVE_OK				1
#define MOVE_LIMIT			2
#define MOVE_TIME_OVER		3

#define ERROR_CHAIR1_FORWARD			1
#define ERROR_CHAIR1_BACKWARD		2
#define ERROR_CHAIR1_UP				3
#define ERROR_CHAIR1_DOWN				4
#define ERROR_CHAIR2_FORWARD			5
#define ERROR_CHAIR2_BACKWARD		6
#define ERROR_CHAIR2_UP				7
#define ERROR_CHAIR2_DOWN				8
#define ERROR_ROBOT_UP					9
#define ERROR_ROBOT_DOWN				10

#define WARNING_HIGH_CURRENT_MOTOR1		1
#define WARNING_HIGH_CURRENT_MOTOR2		2
#define WARNING_MOTOR_DRIVER_FAULT		4

#define D_WControl      0xa0//0b10100000
#define D_RControl      0xa1//0b10100001

#define EEPROM_ADDR_FW_VERSION1										0
#define EEPROM_ADDR_FW_VERSION2										1

#define EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_MSB			2
#define EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_LSB				3
#define EEPROM_ADDR_LEFT_CHAIR_HEIGHT_HOME_POSITION_MSB			4
#define EEPROM_ADDR_LEFT_CHAIR_HEIGHT_HOME_POSITION_LSB			5

#define EEPROM_ADDR_RIGHT_CHAIR_BACK_HOME_POSITION_MSB			6
#define EEPROM_ADDR_RIGHT_CHAIR_BACK_HOME_POSITION_LSB			7
#define EEPROM_ADDR_RIGHT_CHAIR_HEIGHT_HOME_POSITION_MSB		8
#define EEPROM_ADDR_RIGHT_CHAIR_HEIGHT_HOME_POSITION_LSB			9

#define EEPROM_ADDR_ROBOT_HEIGHT_HOME_POSITION_MSB				10
#define EEPROM_ADDR_ROBOT_HEIGHT_HOME_POSITION_LSB				11

#define FWD_BACK_LIMIT_TIME 			16000//14sec Measure 2025.9.29
#define UP_DOWN_LIMIT_TIME 			24000//21sec
#define ROBOT_UP_DOWN_LIMIT_TIME 	27000// 23sec


#define UART_SMART_DEVCIE_REQUEST_GO_HOME_POSITION			0x45
#define UART_SMART_DEVCIE_REQUEST_GO_TARGET_POSITION			0x46
#define UART_ESP_SEND_ROBOT_STOP_RESPONSE						0x47
#define UART_SMART_DEVICE_SEND_POSITION_RESPONSE			0x48
//#define UART_SMART_DEVICE_SEND_POSITION_RESPONSE				0x13
#define UART_SMART_DEVICE_REQUEST_CURRENT_POSITION				0x49
#define UART_SMART_DEVICE_SEND_MODEL_NAME				0x4A
#define UART_PIC_SEND_ERROR_CODE									0x4E
#define UART_ESP_SEND_ROBOT_STOP									0x75


#define LEFT_CHAIR	1//CHAIR1
#define RIGHT_CHAIR	2//CHAIR2
#define ROBOT	4
#define HM_LEFT_CHAIR		0x10
#define HM_RIGHT_CHAIR		0x20

#define MODEL_HA		1
#define MODEL_HM		2

#define CHAIR_FRONT_BACK		1
#define CHAIR_UP_DOWN		2
#define ROBOT_UP_DOWN		4

//#define POTENTIO_CHAIR1_HOME_POSITION	78+3
//#define POTENTIO_CHAIR1_MAX_POSITION		168
//#define POTENTIO_CHAIR2_HOME_POSITION	78+3
//#define POTENTIO_CHAIR2_MAX_POSITION		168

#define STX		0xF1
#define ETX		0xF4
#define ACK		0x6
#define NACK	0xff

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
#ifdef BOARD_B
#define I2C_SCL_HIGH				LL_GPIO_SetOutputPin(GPIOE, I2C_SCL_Pin)
#define I2C_SCL_LOW					LL_GPIO_ResetOutputPin(GPIOE, I2C_SCL_Pin)
#define I2C_SDA_HIGH				LL_GPIO_SetOutputPin(GPIOE, I2C_SDA_Pin)
#define I2C_SDA_LOW					LL_GPIO_ResetOutputPin(GPIOE, I2C_SDA_Pin)
#define I2C_SDA_READ				LL_GPIO_IsInputPinSet(GPIOE, I2C_SDA_Pin)
#else
#define I2C_SCL_HIGH				LL_GPIO_SetOutputPin(GPIOB, I2C_SCL_Pin)//48번
#define I2C_SCL_LOW					LL_GPIO_ResetOutputPin(GPIOB, I2C_SCL_Pin)
#define I2C_SDA_HIGH				LL_GPIO_SetOutputPin(GPIOB, I2C_SDA_Pin)//51번 PIN
#define I2C_SDA_LOW					LL_GPIO_ResetOutputPin(GPIOB, I2C_SDA_Pin)
#define I2C_SDA_READ				LL_GPIO_IsInputPinSet(GPIOB, I2C_SDA_Pin)
#endif

#define SW_CHAIR1_FWD_CHECK 		LL_GPIO_IsInputPinSet(GPIOD, SW_CHAIR1_FWD_Pin)
#define SW_CHAIR1_BACK_CHECK 		LL_GPIO_IsInputPinSet(GPIOD, SW_CHAIR1_BACK_Pin)
#define SW_CHAIR1_UP_CHECK		LL_GPIO_IsInputPinSet(GPIOD, SW_CHAIR1_UP_Pin)
//#define SW_CHAIR1_OK_CHECK 		LL_GPIO_IsInputPinSet(GPIOA, SW_CHAIR1_OK_Pin)
#ifdef BOARD_B
#define SW_CHAIR2_FWD_CHECK		LL_GPIO_IsInputPinSet(GPIOA, SW_CHAIR2_FWD_Pin)
#define SW_CHAIR2_BACK_CHECK 		LL_GPIO_IsInputPinSet(GPIOA, SW_CHAIR2_BACK_Pin)
#define SW_CHAIR2_UP_CHECK 		LL_GPIO_IsInputPinSet(GPIOA, SW_CHAIR2_UP_Pin)
#else
#define SW_CHAIR2_FWD_CHECK		LL_GPIO_IsInputPinSet(GPIOE, SW_CHAIR2_FWD_Pin)
#define SW_CHAIR2_BACK_CHECK 		LL_GPIO_IsInputPinSet(GPIOE, SW_CHAIR2_BACK_Pin)
#define SW_CHAIR2_UP_CHECK 		LL_GPIO_IsInputPinSet(GPIOB, SW_CHAIR2_UP_Pin)
#endif
//#define SW_CHAIR2_OK_CHECK 		LL_GPIO_IsInputPinSet(GPIOB, SW_CHAIR2_OK_Pin)
#ifdef BOARD_B
#define SW_ROBOT_UP_CHECK 		LL_GPIO_IsInputPinSet(GPIOC, SW_ROBOT_UP_Pin)
#define SW_ROBOT_DW_CHECK 		LL_GPIO_IsInputPinSet(GPIOC, SW_ROBOT_DW_Pin)
#else
#define SW_ROBOT_UP_CHECK 		LL_GPIO_IsInputPinSet(GPIOA, SW_ROBOT_UP_Pin)
#define SW_ROBOT_DW_CHECK 		LL_GPIO_IsInputPinSet(GPIOA, SW_ROBOT_DW_Pin)
#endif
#ifdef RMC_BLACK
#define SW_CHAIR1_DW_CHECK 		(SW_CHAIR1_BACK_CHECK & SW_CHAIR2_UP_CHECK)
#define SW_CHAIR2_DW_CHECK		(SW_CHAIR2_BACK_CHECK & SW_CHAIR1_UP_CHECK)
#else
  #ifdef BOARD_B
  #define SW_CHAIR1_DW_CHECK 		LL_GPIO_IsInputPinSet(GPIOC, SW_CHAIR1_DW_Pin)
  #define SW_CHAIR2_DW_CHECK		LL_GPIO_IsInputPinSet(GPIOA, SW_CHAIR2_DW_Pin)
  #else
  #define SW_CHAIR1_DW_CHECK 		LL_GPIO_IsInputPinSet(GPIOA, SW_CHAIR1_DW_Pin)
  #define SW_CHAIR2_DW_CHECK		LL_GPIO_IsInputPinSet(GPIOB, SW_CHAIR2_DW_Pin)
  #endif
#endif

#define LIMIT_CHAIR1_FRONT_CHECK		LL_GPIO_IsInputPinSet(GPIOB, LMT_CHAIR1_FRT_Pin)
#define LIMIT_CHAIR1_BACK_CHECK		LL_GPIO_IsInputPinSet(GPIOD, LMT_CHAIR1_BACK_Pin)
#define LIMIT_CHAIR1_DW_CHECK 			LL_GPIO_IsInputPinSet(GPIOD, LMT_CHAIR1_DW_Pin)
#define LIMIT_CHAIR2_FRONT_CHECK 		LL_GPIO_IsInputPinSet(GPIOE, LMT_CHAIR2_FRT_Pin)
#define LIMIT_CHAIR2_BACK_CHECK 		LL_GPIO_IsInputPinSet(GPIOE, LMT_CHAIR2_BACK_Pin)
#define LIMIT_CHAIR2_DW_CHECK		LL_GPIO_IsInputPinSet(GPIOE, LMT_CHAIR2_DW_Pin)
#define LIMIT_ROBOT_UP_CHECK 			LL_GPIO_IsInputPinSet(GPIOC, LMT_ROBOT_UP_Pin)
#define LIMIT_ROBOT_DW_CHECK 			LL_GPIO_IsInputPinSet(GPIOC, LMT_ROBOT_DW_Pin)

#define DRV1_nFAULT_CHECK 			LL_GPIO_IsInputPinSet(GPIOE, DRV1_nFAULT_Pin)
#define DRV2_nFAULT_CHECK 			LL_GPIO_IsInputPinSet(GPIOB, DRV2_nFAULT_Pin)
#define DRV3_nFAULT_CHECK 			LL_GPIO_IsInputPinSet(GPIOD, DRV3_nFAULT_Pin)

			


#define ADC_CHAIR1_FWD_BACK_CURRENT_PROTECTION_INPUT			(TRISA0 = 1)
#define ADC_CHAIR1_UP_DOWN_CURRENT_PROTECTION_INPUT			(TRISA1 = 1)
#define ADC_CHAIR2_FWD_BACK_CURRENT_PROTECTION_INPUT			(TRISA2 = 1)
#define ADC_CHAIR2_UP_DOWN_CURRENT_PROTECTION_INPUT			(TRISA3 = 1)
#define ADC_ROBOT_UP_DOWN_CURRENT_PROTECTION_INPUT			(TRISA4 = 1)
#define ADC_CHAIR1_POSITION_INPUT									(TRISA6 = 1)
#define ADC_CHAIR2_POSITION_INPUT									(TRISF7 = 1)

#define ADC_LASER_SENSOR1_INPUT									(TRISG0 = 1)
#define ADC_LASER_SENSOR3_INPUT									(TRISG4 = 1)
#define ADC_LASER_SENSOR5_INPUT									(TRISF4 = 1)



#define DRV_CHAIR1_FWD_HIGH		LL_GPIO_SetOutputPin(GPIOE, DRV1_IN1_Pin)
#define DRV_CHAIR1_FWD_LOW		LL_GPIO_ResetOutputPin(GPIOE, DRV1_IN1_Pin)
#define DRV_CHAIR1_REV_HIGH		LL_GPIO_SetOutputPin(GPIOE, DRV1_IN2_Pin)
#define DRV_CHAIR1_REV_LOW		LL_GPIO_ResetOutputPin(GPIOE, DRV1_IN2_Pin)

#define DRV_CHAIR2_FWD_HIGH		LL_GPIO_SetOutputPin(GPIOE, DRV1_IN3_Pin)//DRV1 IN3, IN4
#define DRV_CHAIR2_FWD_LOW		LL_GPIO_ResetOutputPin(GPIOE, DRV1_IN3_Pin)
#define DRV_CHAIR2_REV_HIGH		LL_GPIO_SetOutputPin(GPIOE, DRV1_IN4_Pin)
#define DRV_CHAIR2_REV_LOW		LL_GPIO_ResetOutputPin(GPIOE, DRV1_IN4_Pin)

#define DRV_CHAIR2_UP_HIGH		LL_GPIO_SetOutputPin(GPIOC, DRV2_IN1_Pin)
#define DRV_CHAIR2_UP_LOW			LL_GPIO_ResetOutputPin(GPIOC, DRV2_IN1_Pin)
#define DRV_CHAIR2_DW_HIGH		LL_GPIO_SetOutputPin(GPIOC, DRV2_IN2_Pin)
#define DRV_CHAIR2_DW_LOW		LL_GPIO_ResetOutputPin(GPIOC, DRV2_IN2_Pin)

#define DRV_CHAIR1_UP_HIGH		LL_GPIO_SetOutputPin(GPIOE, DRV2_IN3_Pin)
#define DRV_CHAIR1_UP_LOW			LL_GPIO_ResetOutputPin(GPIOE, DRV2_IN3_Pin)
#define DRV_CHAIR1_DW_HIGH		LL_GPIO_SetOutputPin(GPIOE, DRV2_IN4_Pin)
#define DRV_CHAIR1_DW_LOW		LL_GPIO_ResetOutputPin(GPIOE, DRV2_IN4_Pin)

#define DRV_ROBOT_UP_HIGH		LL_GPIO_SetOutputPin(GPIOD, DRV3_IN1_Pin)
#define DRV_ROBOT_UP_LOW		LL_GPIO_ResetOutputPin(GPIOD, DRV3_IN1_Pin)
#define DRV_ROBOT_DW_HIGH		LL_GPIO_SetOutputPin(GPIOD, DRV3_IN2_Pin)
#define DRV_ROBOT_DW_LOW		LL_GPIO_ResetOutputPin(GPIOD, DRV3_IN2_Pin)

#define DRV1_nSlEEP_DISABLE			LL_GPIO_SetOutputPin(GPIOE, DRV1_nSLEEP_Pin)
#define DRV1_nSlEEP_ENABLE			LL_GPIO_ResetOutputPin(GPIOE, DRV1_nSLEEP_Pin)

#define DRV2_nSlEEP_DISABLE			LL_GPIO_SetOutputPin(GPIOB, DRV2_nSLEEP_Pin)
#define DRV2_nSlEEP_ENABLE			LL_GPIO_ResetOutputPin(GPIOB, DRV2_nSLEEP_Pin)

#define DRV3_nSlEEP_DISABLE			LL_GPIO_SetOutputPin(GPIOD, DRV3_nSLEEP_Pin)
#define DRV3_nSlEEP_ENABLE			LL_GPIO_ResetOutputPin(GPIOD, DRV3_nSLEEP_Pin)


#define MCU_LED_ON					LL_GPIO_ResetOutputPin(GPIOB, CpuRun_Pin)
#define MCU_LED_OFF				LL_GPIO_SetOutputPin(GPIOB, CpuRun_Pin)


void delayMsec(u32 delayTime);
void delay(void);
void EEPROM_Write(u16 Address, u8 Data);
u8 EEPROM_Read(u16 Address);
u8 EepromRead(u16 Raddress);
void EepromWrite(u16 Waddress, u8 WData);
void EEPROM_Erase(void);
void I2C_Ack(void);
u8 I2C_Read_Data(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write_Data(u8 data);
void TimingDelay_Decrement(void);
void I2C_SCL_out_Mode(void);
void I2C_SDA_in_Mode(void);
void I2C_SDA_out_Mode(void);


u8 LeftChairGoFront(u8 Auto, u16 TargetPos);
u8 LeftChairGoBack(u8 Auto, u16 TargetPos);
u8 LeftChairGoUp(u8 Auto, u16 TargetPos);
u8 LeftChairGoDown(u8 Auto, u16 TargetPos);
u8 RightChairGoFront(u8 Auto, u16 TargetPos);
u8 RightChairGoBack(u8 Auto, u16 TargetPos);
u8 RightChairGoUp(u8 Auto, u16 TargetPos);
u8 RightChairGoDown(u8 Auto, u16 TargetPos);
u8 RobotGoUp(u8 Auto, u16 TargetPos);
u8 RobotGoDown(u8 Auto, u16 TargetPos);
void SPMode(void);
void ADC1_Init_LL(void);

void MasterComm1(void);
void UartProcess(void);
void UartResponseProcess(void);
void UartSmartDeviceRequestGoHomePositionResponse(u8 LR, u8 WhichOne, u16 Length);	 //0x10
void UartSmartDeviceRequestGoTargetPositionResponse(u8 Complete, u8 Dir);	 //0x11
void UartEspRequestRobotStopResponse(u8 WhichOne);//0x12

void UartPICSendLengthDataAfterManualAdjustment(u8 WhichOne);//0x13
void UartSmartDeviceResponseManualAdjustment();//0x13
void UartSmartDeviceRequestCurrentPositionResponse(u8 WhichOne);	 //0x14
void UartPICSendErrorCode(u8 WhichOne, u8 ErrorCode1, u8 ErrorCode2);//0x1E
void UartPICSendErrorCode(u8 WhichOne, u8 ErrorCode1, u8 ErrorCode2);
void UartSmartDeviceSendModelNameResponse(u8 ModelNo);
typedef struct {
	u16 Timer;
	u16 SPMode;
	u8 ChairName;
	u8 MotorName;
	u8 ModelName;
}
BdcSystem_InitTypeDef;

typedef struct {
	u16 ErrorCode;
	u16 WarningCode;
	u16 Length;

}
MotorCtrl_InitTypeDef;

typedef struct {
	u32 FindPosition;
	u32 voltage;
	u16 TargetPosition;
	u16 CurrentPosition;
	u16 HomePosition;
	u16 PrePosition;
	u8 HomePosi[2];
	u8 Length;
	u8 Leng[2];
}
LaserSensor_InitTypeDef;

typedef struct {
	u32 FindPosition;
	u32 voltage;
	u16 TargetPosition;
	u16 CurrentPosition;
	u16 HomePosition;
	u16 PrePosition;
	u8 HomePosi[2];
	u16 Length;
	u8 Leng[2];
	u16 HomeCheckTimer;
	u8 HomeCheckFlag;
}
Potentiometer_InitTypeDef;

typedef struct {
	u16 ResultValue;
	u8 SelectChannel;
	u16 RobotHeightSensor;//Sensor
	u16 Chair1HeightSensor;//Sensor
	u16 Chair2HeightSensor;//Sensor
	u16 Chair1LengthSensor;//Sensor
	u16 Chair2LengthSensor;//Sensor
	u16 Chair1HeightPot;//Potentiometer
	u16 Chair2HeightPot;//Potentiometer
	u16 Drv1Protection1;
	u16 Drv1Protection2;
	u16 Drv2Protection1;
	u16 Drv2Protection2;
	u16 Drv3Protection1;
	u8 AdcCnt;
	u16 AdcBuf[10];
	u16 AdcAvg;
	u32 AdcSum;
}
ADC_Healerbot_InitTypeDef;

typedef struct {

	u8 Cnt;
	u16 AdcBuf[10];
	u16 AdcAvg;
	u32 AdcSum;
}
AVERAGE_InitTypeDef;


typedef struct {
	u16 AnalogCurrent;
	u16 MotorTimer;
	u8 MoveState;
	u8 Direction;

}
BDCMotor_InitTypeDef;

#define COM1_DMA_RX_BUF_SIZE   64U

typedef struct {
 	u8 RxBuf[30];
	u8 TxBuf[30];
	u8 DMA_Rx[COM1_DMA_RX_BUF_SIZE];
	u32 DMA_RxHead;
	u16 RxCount;
	u8 TxCount;
	u8 Flag;
	u8 AckCheck;
	u16 Timer;
	u8 Retry;
	u16 CommandDataLength;
}
Comm_InitTypeDef;

typedef struct {
 	u16 Second;
	u16 Motor1FwdLimitDelay;
	u16 Motor2UpLimitDelay;
	u16 Motor3FwdLimitDelay;
	u16 Motor4UpLimitDelay;
	u16 Motor5UpLimitDelay;
	u16 Motor1RevLimitDelay;
	u16 Motor2DownLimitDelay;
	u16 Motor3RevLimitDelay;
	u16 Motor4DownLimitDelay;
	u16 Motor5DownLimitDelay;
	u16 Minute;
	u16 ForBackwardCount;
	u16 UpDownCount;
	u16 RobotCount;
}
HWTest_InitTypeDef;



/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif  __MAIN_H

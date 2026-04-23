/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */


Comm_InitTypeDef Comm1;
extern ADC_Healerbot_InitTypeDef ADConverter;
extern AVERAGE_InitTypeDef Chair1LengthAvg, Chair2LengthAvg, Chair1HeightAvg, Chair2HeightAvg, RobotHeightAvg;
extern LaserSensor_InitTypeDef LaserChair1Up,LaserChair1Front, LaserChair2Up,LaserChair2Front, LaserRobotUp;
extern Potentiometer_InitTypeDef PotentioChair1Height, PotentioChair2Height;
extern BDCMotor_InitTypeDef BDCMotor1, BDCMotor2, BDCMotor3, BDCMotor4, BDCMotor5;
extern HWTest_InitTypeDef HWTest;
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
u32 systick_counter;
u32 Timer7Counter;
extern u16 internal_adc_value[10];
u32 Timer6Counter;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
	systick_counter++;
	if(systick_counter >= U32_MAX)
		systick_counter = 0;
	TimingDelay_Decrement();
	//TimingDelay_Decrement();
  /* USER CODE END SysTick_IRQn 0 */

  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles ADC1 and ADC2 global interrupts.
  */
void ADC1_2_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_2_IRQn 0 */

  /* USER CODE END ADC1_2_IRQn 0 */
  /* USER CODE BEGIN ADC1_2_IRQn 1 */

  /* USER CODE END ADC1_2_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles UART4 global interrupt.
  */
void UART4_IRQHandler(void)
{
  /* USER CODE BEGIN UART4_IRQn 0 */

	/* IDLE 인터럽트: 송신측이 전송을 멈추면 발생 → 패킷 수신 완료 감지 */
	if (LL_USART_IsActiveFlag_IDLE(UART4))
	{
		/* STM32F1: IDLE 플래그 클리어 = SR 읽기 후 DR 읽기 (DMA 사용 중이므로 DR은 비어있음) */
		LL_USART_ClearFlag_IDLE(UART4);

		/* DMA write 포인터 계산: 버퍼 크기 - DMA 남은 카운터 = 현재 쓴 위치 */
		u32 dma_head = COM1_DMA_RX_BUF_SIZE - LL_DMA_GetDataLength(DMA2, LL_DMA_CHANNEL_3);
		u32 old_head = Comm1.DMA_RxHead;
		u32 received_len;

		/* Circular 버퍼 wrap-around 고려한 수신 바이트 수 계산 */
		if (dma_head >= old_head)
			received_len = dma_head - old_head;
		else
			received_len = COM1_DMA_RX_BUF_SIZE - old_head + dma_head;

		Comm1.DMA_RxHead = dma_head;

		if (received_len == 0)
			return;

		/* RxBuf 남은 공간 초과 시 전체 리셋 (버퍼 오버플로우 방지) */
		if ((Comm1.RxCount + received_len) > sizeof(Comm1.RxBuf))
		{
			Comm1.RxCount = 0;
			memset(&Comm1.RxBuf, 0, sizeof(Comm1.RxBuf));
			return;
		}

		/* Circular DMA 버퍼 → RxBuf에 누적 (wrap-around 처리)
		 * USB-UART 어댑터가 패킷을 분할 전송할 경우 여러 IDLE에 걸쳐 조립 */
		if (dma_head > old_head)
		{
			memcpy(&Comm1.RxBuf[Comm1.RxCount], &Comm1.DMA_Rx[old_head], received_len);
		}
		else
		{
			u32 first_part = COM1_DMA_RX_BUF_SIZE - old_head;
			memcpy(&Comm1.RxBuf[Comm1.RxCount], &Comm1.DMA_Rx[old_head], first_part);
			memcpy(&Comm1.RxBuf[Comm1.RxCount + first_part], &Comm1.DMA_Rx[0], dma_head);
		}

		Comm1.RxCount += (u16)received_len;

		/* 헤더 3바이트(STX+LEN_MSB+LEN_LSB) 미만이면 추가 수신 대기 */
		if (Comm1.RxCount < 3U)
			return;

		/* STX 검사: 잘못된 시작이면 전체 리셋 */
		if (Comm1.RxBuf[0] != STX)
		{
			Comm1.RxCount = 0;
			memset(&Comm1.RxBuf, 0, sizeof(Comm1.RxBuf));
			return;
		}

		Comm1.CommandDataLength = ((u16)(Comm1.RxBuf[1] << 8) & 0xFF00)
		                        | ((u16)(Comm1.RxBuf[2]) & 0x00FF);

		/* 패킷이 아직 다 안 왔으면 다음 IDLE까지 대기 */
		if (Comm1.RxCount < (Comm1.CommandDataLength + 5U))
			return;

		/* ETX 검사 */
		if (Comm1.RxBuf[Comm1.CommandDataLength + 4] != ETX)
		{
			Comm1.RxCount = 0;
			memset(&Comm1.RxBuf, 0, sizeof(Comm1.RxBuf));
			return;
		}

		Comm1.Flag = 1;
	}

	/* Overrun 오류: DMA_RxHead를 현재 DMA 위치로 동기화 */
	if (LL_USART_IsActiveFlag_ORE(UART4))
	{
		LL_USART_ClearFlag_ORE(UART4);
		Comm1.DMA_RxHead = COM1_DMA_RX_BUF_SIZE - LL_DMA_GetDataLength(DMA2, LL_DMA_CHANNEL_3);
	}

	/* Framing 오류 클리어 */
	if (LL_USART_IsActiveFlag_FE(UART4))
	{
		LL_USART_ClearFlag_FE(UART4);
	}

  /* USER CODE END UART4_IRQn 0 */
  /* USER CODE BEGIN UART4_IRQn 1 */

  /* USER CODE END UART4_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */
	u8 j;

	if(LL_TIM_IsActiveFlag_UPDATE(TIM7)){
		LL_TIM_ClearFlag_UPDATE(TIM7);      

		Timer7Counter++;

		if(Timer7Counter == 500){
			MCU_LED_ON;
		}
		else if(Timer7Counter >= 1000){
			MCU_LED_OFF;
			Timer7Counter = 0;
			#ifdef HW_TEST
			HWTest.Second++;
			if(HWTest.Second == 60){
				HWTest.Second = 0;
				HWTest.Minute++;
				if(HWTest.Minute >= U16_MAX)
					HWTest.Minute = 0;
					
			}
			//printf("MM:  %d, SS:  %d, FB:  %d, UD:  %d, RB:  %d\n",HWTest.Minute,  HWTest.Second, HWTest.ForBackwardCount,
			//	HWTest.UpDownCount, HWTest.RobotCount);
			#endif
		}
		if(PotentioChair1Height.HomeCheckTimer)
		PotentioChair1Height.HomeCheckTimer--;
		if(PotentioChair2Height.HomeCheckTimer)
			PotentioChair2Height.HomeCheckTimer--;
			
		#if 1
		Chair1LengthAvg.AdcBuf[Chair1LengthAvg.Cnt] = internal_adc_value[6];
		Chair1LengthAvg.Cnt++;
		if(Chair1LengthAvg.Cnt > 9){
			Chair1LengthAvg.AdcSum = 0;
			Chair1LengthAvg.Cnt = 0;
			for(j=0;j<10;j++){
				Chair1LengthAvg.AdcSum += Chair1LengthAvg.AdcBuf[j];
				ADConverter.Chair1LengthSensor = (u16)(Chair1LengthAvg.AdcSum/10);
			}
			LaserChair1Front.voltage = (((u32)ADConverter.Chair1LengthSensor* 3300)>>12);
			#if 0
			LaserChair1Front.FindPosition = (((float)28200/(LaserChair1Front.voltage - 190))*1000)/129;
			#else
			LaserChair1Front.FindPosition = (u32)(((float)28200/(LaserChair1Front.voltage - 190))*1000)/129;
			#endif
			LaserChair1Front.CurrentPosition = (u16)LaserChair1Front.FindPosition;
			LaserChair1Up.CurrentPosition = LaserChair1Front.CurrentPosition;
		}
		Chair2LengthAvg.AdcBuf[Chair2LengthAvg.Cnt] = internal_adc_value[7];
		Chair2LengthAvg.Cnt++;
		if(Chair2LengthAvg.Cnt > 9){
			Chair2LengthAvg.AdcSum = 0;
			Chair2LengthAvg.Cnt = 0;
			for(j=0;j<10;j++){
				Chair2LengthAvg.AdcSum += Chair2LengthAvg.AdcBuf[j];
				ADConverter.Chair2LengthSensor = (u16)(Chair2LengthAvg.AdcSum/10);
			}
			LaserChair2Front.voltage = (((u32)ADConverter.Chair2LengthSensor * 3300)>>12);
			#if 0
			LaserChair2Front.FindPosition = (((float)28200/(LaserChair2Front.voltage - 190))*1000)/129;
			#else
			LaserChair2Front.FindPosition = (u16)(((float)28200/(LaserChair2Front.voltage - 190))*1000)/129;
			#endif
			LaserChair2Front.CurrentPosition = (u16)LaserChair2Front.FindPosition;
			LaserChair2Up.CurrentPosition = LaserChair2Front.CurrentPosition;
		}
		//ADConverter.RobotHeightSensor = internal_adc_value[8];
		#if 0
		//LaserRobotUp.voltage = (((u32)ADConverter.RobotHeightSensor * 3300)>>12);
		//LaserRobotUp.FindPosition = (u16)(((float)28200/(LaserRobotUp.voltage - 190))*1000)/129;
		//LaserRobotUp.CurrentPosition = (u16)LaserRobotUp.FindPosition;
		#else
		RobotHeightAvg.AdcBuf[RobotHeightAvg.Cnt] = internal_adc_value[8];
		RobotHeightAvg.Cnt++;
		if(RobotHeightAvg.Cnt >= 5){
			u16 tmp[5];
			u16 swap_val;
			u8 k;
			RobotHeightAvg.Cnt = 0;
			for(j=0; j<5; j++) tmp[j] = RobotHeightAvg.AdcBuf[j];
			// 버블 정렬 (5샘플 미디안 필터)
			for(j=0; j<4; j++){
				for(k=j+1; k<5; k++){
					if(tmp[k] < tmp[j]){
						swap_val = tmp[j]; tmp[j] = tmp[k]; tmp[k] = swap_val;
					}
				}
			}
			ADConverter.RobotHeightSensor = tmp[2]; // 중앙값
			LaserRobotUp.voltage = (((u32)ADConverter.RobotHeightSensor * 3300) >> 12);
			LaserRobotUp.FindPosition = (u16)(((float)28200/(LaserRobotUp.voltage - 190))*1000)/129;
			LaserRobotUp.CurrentPosition = (u16)LaserRobotUp.FindPosition;
		}
		#endif
		#if 1
		Chair1HeightAvg.AdcBuf[Chair1HeightAvg.Cnt] = internal_adc_value[9]>>2;
		Chair1HeightAvg.Cnt++;
		if(Chair1HeightAvg.Cnt > 9){
			Chair1HeightAvg.AdcSum = 0;
			Chair1HeightAvg.Cnt = 0;
			for(j=0;j<10;j++){
				Chair1HeightAvg.AdcSum += Chair1HeightAvg.AdcBuf[j];
				ADConverter.Chair1HeightPot = (u16)(Chair1HeightAvg.AdcSum/10);
				PotentioChair1Height.CurrentPosition = (u16)((float)ADConverter.Chair1HeightPot/6.7);//0~80mm
			}
		}
		#else
		ADConverter.Chair1HeightPot = internal_adc_value[9]>>2;// ADC: 180~430
		PotentioChair1Height.CurrentPosition = (u16)((float)ADConverter.Chair1HeightPot/2.5);//72~172mm
		#endif
		#if 1
		Chair2HeightAvg.AdcBuf[Chair2HeightAvg.Cnt] = internal_adc_value[4]>>2;
		Chair2HeightAvg.Cnt++;
		if(Chair2HeightAvg.Cnt > 9){
			Chair2HeightAvg.AdcSum = 0;
			Chair2HeightAvg.Cnt = 0;
			for(j=0;j<10;j++){
				Chair2HeightAvg.AdcSum += Chair2HeightAvg.AdcBuf[j];
				ADConverter.Chair2HeightPot = (u16)(Chair2HeightAvg.AdcSum/10);
				PotentioChair2Height.CurrentPosition = (u16)((float)ADConverter.Chair2HeightPot/2.5);//0~80mm
			}
		}
		#else
		ADConverter.Chair2HeightPot = internal_adc_value[4]>>2;// ADC: 180~430
		PotentioChair2Height.CurrentPosition = (u16)((float)ADConverter.Chair2HeightPot/2.5);
		#endif
		ADConverter.Drv1Protection1 = internal_adc_value[0]>>2;
		ADConverter.Drv1Protection2 = internal_adc_value[1]>>2;
		ADConverter.Drv2Protection1 = internal_adc_value[2]>>2;
		ADConverter.Drv2Protection2 = internal_adc_value[3]>>2;
		ADConverter.Drv3Protection1 = internal_adc_value[5]>>2;
		//printf("H1:  %d\n",PotentioChair1Height.CurrentPosition);
		#endif
		if(BDCMotor1.MotorTimer){
			BDCMotor1.MotorTimer--;
		}
		if(BDCMotor2.MotorTimer){
			BDCMotor2.MotorTimer--;		
		}
		if(BDCMotor3.MotorTimer){
			BDCMotor3.MotorTimer--;	
		}
		if(BDCMotor4.MotorTimer){
			BDCMotor4.MotorTimer--;
		}
		if(BDCMotor5.MotorTimer){
			BDCMotor5.MotorTimer--;
		}
		#ifdef HW_TEST
		if((LIMIT_CHAIR2_FRONT_CHECK == ACTIVE_LOW) && (HWTest.Motor3RevLimitDelay== 0)){
			DRV_CHAIR2_FWD_LOW;
			HWTest.Motor3RevLimitDelay = 2000;
			HWTest.ForBackwardCount++;
			printf("MM:  %d, SS:  %d, FB:  %d\n",HWTest.Minute,  HWTest.Second, HWTest.ForBackwardCount);
		}
		else if((LIMIT_CHAIR2_BACK_CHECK == ACTIVE_LOW) && (HWTest.Motor3FwdLimitDelay == 0)){
			DRV_CHAIR2_FWD_LOW;
			HWTest.Motor3FwdLimitDelay = 2000;
			HWTest.ForBackwardCount++;
			printf("MM:  %d, SS:  %d, FB:  %d\n",HWTest.Minute,  HWTest.Second, HWTest.ForBackwardCount);
		}
		if(HWTest.Motor3RevLimitDelay){// go rev
			HWTest.Motor3RevLimitDelay--;
			if(HWTest.Motor3RevLimitDelay == 1000){
				DRV_CHAIR2_FWD_HIGH;
				DRV_CHAIR2_REV_HIGH;
			}
		}
		if(HWTest.Motor3FwdLimitDelay){// go fwd
			HWTest.Motor3FwdLimitDelay--;
			if(HWTest.Motor3FwdLimitDelay == 1000){
				DRV_CHAIR2_FWD_HIGH;
				DRV_CHAIR2_REV_LOW;
			}
		}
		if((PotentioChair2Height.CurrentPosition > 170) && (HWTest.Motor4DownLimitDelay== 0)){
			DRV_CHAIR2_UP_LOW;
			HWTest.Motor4DownLimitDelay = 4000;
			HWTest.UpDownCount++;
			printf("MM:  %d, SS:  %d, UD:  %d\n",HWTest.Minute,  HWTest.Second, HWTest.UpDownCount);
		}
		else if((PotentioChair2Height.CurrentPosition < 78) &&  (HWTest.Motor4UpLimitDelay == 0)){
			DRV_CHAIR2_UP_LOW;
			HWTest.Motor4UpLimitDelay = 4000;
			HWTest.UpDownCount++;
			printf("MM:  %d, SS:  %d, UD:  %d\n",HWTest.Minute,  HWTest.Second, HWTest.UpDownCount);
		}
		if(HWTest.Motor4DownLimitDelay){// go down
			HWTest.Motor4DownLimitDelay--;
			if(HWTest.Motor4DownLimitDelay == 3000){
				DRV_CHAIR2_UP_HIGH;
				DRV_CHAIR2_DW_LOW;
			}
		}
		if(HWTest.Motor4UpLimitDelay){// go up
			HWTest.Motor4UpLimitDelay--;
			if(HWTest.Motor4UpLimitDelay == 3000){
				DRV_CHAIR2_UP_HIGH;
				DRV_CHAIR2_DW_HIGH;
			}
		}
		if((LIMIT_ROBOT_UP_CHECK == ACTIVE_LOW) && (HWTest.Motor5UpLimitDelay == 0)){
			DRV_ROBOT_UP_LOW;
			HWTest.Motor5UpLimitDelay = 6000;
			HWTest.RobotCount++;
			printf("MM:  %d, SS:  %d, RB:  %d\n",HWTest.Minute,  HWTest.Second, HWTest.RobotCount);
		}
		else if((LIMIT_ROBOT_DW_CHECK == ACTIVE_LOW) && (HWTest.Motor5DownLimitDelay == 0)){
			DRV_ROBOT_UP_LOW;
			HWTest.Motor5DownLimitDelay = 6000;
			HWTest.RobotCount++;
			printf("MM:  %d, SS:  %d, RB:  %d\n",HWTest.Minute,  HWTest.Second, HWTest.RobotCount);
		}
		if(HWTest.Motor5DownLimitDelay){// go up
			HWTest.Motor5DownLimitDelay--;
			if(HWTest.Motor5DownLimitDelay == 1000){
				DRV_ROBOT_UP_HIGH;
				DRV_ROBOT_DW_LOW;
			}
		}
		if(HWTest.Motor5UpLimitDelay){// go down
			HWTest.Motor5UpLimitDelay--;
			if(HWTest.Motor5UpLimitDelay == 1000){
				DRV_ROBOT_UP_HIGH;
				DRV_ROBOT_DW_HIGH;
			}
		}
		#endif
	}

  /* USER CODE END TIM7_IRQn 0 */
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

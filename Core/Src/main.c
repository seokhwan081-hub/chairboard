/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f1xx_ll_cortex.h"
#include "stdio.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
BdcSystem_InitTypeDef BdcSystem;


BDCMotor_InitTypeDef BDCMotor1, BDCMotor2, BDCMotor3, BDCMotor4, BDCMotor5;
ADC_Healerbot_InitTypeDef ADConverter;
AVERAGE_InitTypeDef Chair1LengthAvg, Chair2LengthAvg, Chair1HeightAvg, Chair2HeightAvg, RobotHeightAvg;


MotorCtrl_InitTypeDef Chair1, Chair2, Robot;
LaserSensor_InitTypeDef LaserChair1Up,LaserChair1Front, LaserChair2Up,LaserChair2Front, LaserRobotUp;
Potentiometer_InitTypeDef PotentioChair1Height, PotentioChair2Height;

extern Comm_InitTypeDef Comm1;
extern u32 Timer7Counter;

HWTest_InitTypeDef HWTest;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static vu32 TimingDelay;
u16 internal_adc_value[10];

volatile u8 g_is_homing = 0; //원점잡기 동작 중 여부 플래그
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_UART4_Init(void);
static void MX_TIM7_Init(void);
/* USER CODE BEGIN PFP */

void delayMsec(u32 delayTime);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_3);

  /* MemoryManagement_IRQn interrupt configuration */
  NVIC_SetPriority(MemoryManagement_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),3, 0));
  /* BusFault_IRQn interrupt configuration */
  NVIC_SetPriority(BusFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),4, 0));
  /* UsageFault_IRQn interrupt configuration */
  NVIC_SetPriority(UsageFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),3, 0));
  /* SVCall_IRQn interrupt configuration */
  NVIC_SetPriority(SVCall_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),2, 0));
  /* DebugMonitor_IRQn interrupt configuration */
  NVIC_SetPriority(DebugMonitor_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0));
  /* PendSV_IRQn interrupt configuration */
  NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),4, 0));
  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),4, 0));

  /** NONJTRST: Full SWJ (JTAG-DP + SW-DP) but without NJTRST
  */
  LL_GPIO_AF_Remap_SWJ_NONJTRST();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  LL_Init1msTick(SystemCoreClock);
  LL_SYSTICK_EnableIT();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_UART4_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
  LL_TIM_EnableIT_UPDATE(TIM7);
  LL_TIM_EnableCounter(TIM7);
  //LL_ADC_EnableIT_EOS(ADC1);
  //LL_ADC_Enable(ADC1);
  //LL_ADC_REG_StartConversionSWStart(ADC1);
  ADC1_Init_LL();

  I2C_SDA_out_Mode();
  	I2C_SCL_out_Mode();

  	LL_GPIO_SetOutputPin(I2C_SCL_GPIO_Port, I2C_SCL_Pin);
  	LL_GPIO_SetOutputPin(I2C_SDA_GPIO_Port, I2C_SDA_Pin);

  	LL_GPIO_SetOutputPin(GPIOE, DRV1_nSLEEP_Pin);
  	LL_GPIO_SetOutputPin(GPIOB, DRV2_nSLEEP_Pin);
  	LL_GPIO_SetOutputPin(GPIOD, DRV3_nSLEEP_Pin);

  	LL_GPIO_ResetOutputPin(GPIOE, DRV1_IN1_Pin);
  	LL_GPIO_ResetOutputPin(GPIOE, DRV1_IN2_Pin);
  	LL_GPIO_ResetOutputPin(GPIOE, DRV1_IN3_Pin);
  	LL_GPIO_ResetOutputPin(GPIOE, DRV1_IN4_Pin);

  	LL_GPIO_ResetOutputPin(GPIOC, DRV2_IN1_Pin);
	LL_GPIO_ResetOutputPin(GPIOC, DRV2_IN2_Pin);
	LL_GPIO_ResetOutputPin(GPIOC, DRV2_IN3_Pin);
	LL_GPIO_ResetOutputPin(GPIOC, DRV2_IN4_Pin);

	LL_GPIO_ResetOutputPin(GPIOD, DRV3_IN1_Pin);
	LL_GPIO_ResetOutputPin(GPIOD, DRV3_IN2_Pin);

	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_7, LL_GPIO_MODE_INPUT);
LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);

  printf("Power On\n");

   delayMsec(10);
	//while(1){
	//EepromWrite(EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_MSB, 1);
	LaserRobotUp.HomePosi[0] = EepromRead(EEPROM_ADDR_ROBOT_HEIGHT_HOME_POSITION_MSB);
	if(LaserRobotUp.HomePosi[0] == 0xff){
		EepromWrite(EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_MSB, 0);
		#if 1
		EepromWrite(EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_LSB, 0);
		EepromWrite(EEPROM_ADDR_LEFT_CHAIR_HEIGHT_HOME_POSITION_MSB, 0);
		EepromWrite(EEPROM_ADDR_LEFT_CHAIR_HEIGHT_HOME_POSITION_LSB, 0);
		EepromWrite(EEPROM_ADDR_RIGHT_CHAIR_BACK_HOME_POSITION_MSB, 0);
		EepromWrite(EEPROM_ADDR_RIGHT_CHAIR_BACK_HOME_POSITION_LSB, 0);
		EepromWrite(EEPROM_ADDR_RIGHT_CHAIR_HEIGHT_HOME_POSITION_MSB, 0);
		EepromWrite(EEPROM_ADDR_RIGHT_CHAIR_HEIGHT_HOME_POSITION_LSB, 0);
		EepromWrite(EEPROM_ADDR_ROBOT_HEIGHT_HOME_POSITION_MSB, 0);
		EepromWrite(EEPROM_ADDR_ROBOT_HEIGHT_HOME_POSITION_LSB, 0);
		#endif
		printf("EEPRom Init OK\n");
	}
    delayMsec(10);
	LaserChair1Front.HomePosi[0] = EepromRead(EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_MSB);
	#if 1
	LaserChair1Front.HomePosi[1] = EepromRead(EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_LSB);

	PotentioChair1Height.HomePosi[0] = EepromRead(EEPROM_ADDR_LEFT_CHAIR_HEIGHT_HOME_POSITION_MSB);
	PotentioChair1Height.HomePosi[1] = EepromRead(EEPROM_ADDR_LEFT_CHAIR_HEIGHT_HOME_POSITION_LSB);
	LaserChair2Front.HomePosi[0] = EepromRead(EEPROM_ADDR_RIGHT_CHAIR_BACK_HOME_POSITION_MSB);
	LaserChair2Front.HomePosi[1] = EepromRead(EEPROM_ADDR_RIGHT_CHAIR_BACK_HOME_POSITION_LSB);
	PotentioChair2Height.HomePosi[0] = EepromRead(EEPROM_ADDR_RIGHT_CHAIR_HEIGHT_HOME_POSITION_MSB);
	PotentioChair2Height.HomePosi[1] = EepromRead(EEPROM_ADDR_RIGHT_CHAIR_HEIGHT_HOME_POSITION_LSB);
	LaserRobotUp.HomePosi[0] = EepromRead(EEPROM_ADDR_ROBOT_HEIGHT_HOME_POSITION_MSB);
	LaserRobotUp.HomePosi[1] = EepromRead(EEPROM_ADDR_ROBOT_HEIGHT_HOME_POSITION_LSB);
	#endif
	LaserChair1Front.HomePosition = (((u16)LaserChair1Front.HomePosi[0] << 8) & 0xFF00) + ((u16)LaserChair1Front.HomePosi[1] & 0x00FF);
	PotentioChair1Height.HomePosition = (((u16)PotentioChair1Height.HomePosi[0] << 8) & 0xFF00) + ((u16)PotentioChair1Height.HomePosi[1] & 0x00FF);
	LaserChair2Front.HomePosition = (((u16)LaserChair2Front.HomePosi[0] << 8) & 0xFF00) + ((u16)LaserChair2Front.HomePosi[1] & 0x00FF);
	PotentioChair2Height.HomePosition = (((u16)PotentioChair2Height.HomePosi[0] << 8) & 0xFF00) + ((u16)PotentioChair2Height.HomePosi[1] & 0x00FF);
	LaserRobotUp.HomePosition = (((u16)LaserRobotUp.HomePosi[0] << 8) & 0xFF00) + ((u16)LaserRobotUp.HomePosi[1] & 0x00FF);
	printf("Home Position: D1: %d, H1: %d, D2: %d, H2: %d, R: %d\n",LaserChair1Front.HomePosition, PotentioChair1Height.HomePosition,
		LaserChair2Front.HomePosition, PotentioChair2Height.HomePosition,LaserRobotUp.HomePosition);
//}

printf("Power On\n");
delayMsec(10);
/* RXNE 인터럽트 불필요: DMA + IDLE 방식으로 대체됨 */
	if(SW_ROBOT_UP_CHECK == HIGH_ACTIVE)
	{
		printf("This is SP Mode. To out of this mode, reset power\n");
		BdcSystem.SPMode = 1;
		SPMode();
	}
	#ifdef HW_TEST
	delayMsec(500);
	if(LIMIT_ROBOT_UP_CHECK == ACTIVE_LOW){
		DRV_ROBOT_UP_HIGH;
		DRV_ROBOT_DW_HIGH;
	}
	else if(LIMIT_ROBOT_DW_CHECK == ACTIVE_LOW){
		DRV_ROBOT_UP_HIGH;
		DRV_ROBOT_DW_LOW;
	}
	else{
		DRV_ROBOT_UP_HIGH;
		DRV_ROBOT_DW_LOW;
	}
	if(LIMIT_CHAIR2_FRONT_CHECK == ACTIVE_LOW){// GO back
		DRV_CHAIR2_FWD_HIGH;
		DRV_CHAIR2_REV_HIGH;
	}
	else if(LIMIT_CHAIR2_BACK_CHECK == ACTIVE_LOW){// go fwd
		DRV_CHAIR2_FWD_HIGH;
		DRV_CHAIR2_REV_LOW;
	}
	else{
		DRV_CHAIR2_FWD_HIGH;
		DRV_CHAIR2_REV_LOW;
	}
	DRV_CHAIR2_UP_HIGH;
	DRV_CHAIR2_DW_HIGH;
	printf("MM:  %d, SS:  %d, FB:  %d, UD:  %d, RB:  %d\n",HWTest.Minute,  HWTest.Second, HWTest.ForBackwardCount,
			HWTest.UpDownCount, HWTest.RobotCount);
	while(1){
		//printf("MM:  %d, SS:  %d, FB:  %d, UD:  %d, RB:  %d\n",HWTest.Minute,  HWTest.Second, HWTest.ForBackwardCount,
		//	HWTest.UpDownCount, HWTest.RobotCount);
		//delayMsec(2000);
	}
	#endif



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	MasterComm1();
	UartResponseProcess();
	if(Timer7Counter == 200){
		//printf("D: 1:  %d, 2:  %d, R:  %d\n",LaserChair1Front.CurrentPosition, LaserChair2Front.CurrentPosition, LaserRobotUp.CurrentPosition);
		//UartPICSendErrorCode(1, 2, 0);
	}
	//UartCom();		
	//ADCSelectChannel(ADConverter.SelectChannel);
	if(SW_CHAIR1_FWD_CHECK== HIGH_ACTIVE){
		printf("Ch1 fwd\n");
		if(LeftChairGoFront(0, 0) == 1)
			Chair1.ErrorCode = ERROR_CHAIR1_FORWARD;
		UartPICSendLengthDataAfterManualAdjustment(LEFT_CHAIR);

	}
	#if 1
	else if((SW_CHAIR1_BACK_CHECK == HIGH_ACTIVE) && (SW_CHAIR2_UP_CHECK == LOW_INACTIVE)){
		delayMsec(10);
		if(SW_CHAIR2_UP_CHECK == LOW_INACTIVE){
			printf("Ch1 back\n");
			if(LeftChairGoBack(0, 0) == 1)
				Chair1.ErrorCode = ERROR_CHAIR1_BACKWARD;
			UartPICSendLengthDataAfterManualAdjustment(LEFT_CHAIR);
		}
	}
	else if((SW_CHAIR1_UP_CHECK == HIGH_ACTIVE) && (SW_CHAIR2_BACK_CHECK == LOW_INACTIVE)){
		delayMsec(10);
		if(SW_CHAIR2_BACK_CHECK == LOW_INACTIVE){
			printf("Ch1 up\n");
			if(LeftChairGoUp(0, 0) == 1)
				Chair1.ErrorCode = ERROR_CHAIR1_UP;
			UartPICSendLengthDataAfterManualAdjustment(LEFT_CHAIR);
		}
	}
	else if(SW_CHAIR1_DW_CHECK == HIGH_ACTIVE){
		printf("Ch1 dw\n");
		if(LeftChairGoDown(0, 0) == 1)
			Chair1.ErrorCode = ERROR_CHAIR1_DOWN;
		UartPICSendLengthDataAfterManualAdjustment(LEFT_CHAIR);
	}
	else if(SW_CHAIR2_FWD_CHECK== HIGH_ACTIVE){
		printf("Ch2 fwd\n");
		if(RightChairGoFront(0, 0) == 1)
			Chair2.ErrorCode = ERROR_CHAIR2_FORWARD;
		UartPICSendLengthDataAfterManualAdjustment(RIGHT_CHAIR);
	}
	else if((SW_CHAIR2_BACK_CHECK == HIGH_ACTIVE) && (SW_CHAIR1_UP_CHECK == LOW_INACTIVE)){
		delayMsec(10);
		if(SW_CHAIR1_UP_CHECK == LOW_INACTIVE){
			printf("Ch2 back\n");
			if(RightChairGoBack(0, 0) == 1)
				Chair2.ErrorCode = ERROR_CHAIR2_BACKWARD;
			UartPICSendLengthDataAfterManualAdjustment(RIGHT_CHAIR);
		}
	}
	else if((SW_CHAIR2_UP_CHECK == HIGH_ACTIVE) && (SW_CHAIR1_BACK_CHECK == LOW_INACTIVE)){
		delayMsec(10);
		if(SW_CHAIR1_BACK_CHECK == LOW_INACTIVE){
			printf("Ch2 up\n");
			if(RightChairGoUp(0, 0) == 1)
				Chair2.ErrorCode = ERROR_CHAIR2_UP;
			UartPICSendLengthDataAfterManualAdjustment(RIGHT_CHAIR);
		}
	}
	else if(SW_CHAIR2_DW_CHECK == HIGH_ACTIVE){
		printf("Ch2 dw\n");
		if(RightChairGoDown(0, 0) == 1)
			Chair2.ErrorCode = ERROR_CHAIR2_DOWN;
		UartPICSendLengthDataAfterManualAdjustment(RIGHT_CHAIR);
	}
	else if(SW_ROBOT_UP_CHECK == HIGH_ACTIVE){
		printf("Robot up\n");
		if(RobotGoUp(0, 0) == 1)
			Chair2.ErrorCode = ERROR_ROBOT_UP;
		UartPICSendLengthDataAfterManualAdjustment(ROBOT);

	}
	else if(SW_ROBOT_DW_CHECK == HIGH_ACTIVE){
		printf("Robot dw\n");
		if(RobotGoDown(0, 0) == 1)
			Chair2.ErrorCode = ERROR_ROBOT_DOWN;
		UartPICSendLengthDataAfterManualAdjustment(ROBOT);

	}
	
	if(DRV1_nFAULT_CHECK == ACTIVE_LOW){
		Chair1.WarningCode |= WARNING_MOTOR_DRIVER_FAULT;

	}
	#endif
    //#if 1 //pin test
	//if(LIMIT_SW_FRONT_CHECK == ACTIVE_LOW){
		Chair1.WarningCode = 1;
	//}

		//#endif
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_4);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(72000000);
  LL_SetSystemCoreClock(72000000);
  LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSRC_PCLK2_DIV_2);
}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);

  /* TIM7 interrupt Init */
  NVIC_SetPriority(TIM7_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(TIM7_IRQn);

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  TIM_InitStruct.Prescaler = 71;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 1000;
  LL_TIM_Init(TIM7, &TIM_InitStruct);
  LL_TIM_EnableARRPreload(TIM7);
  LL_TIM_SetTriggerOutput(TIM7, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM7);
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4);

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
  /**UART4 GPIO Configuration
  PC10   ------> UART4_TX
  PC11   ------> UART4_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* UART4 interrupt Init */
  NVIC_SetPriority(UART4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0));
  NVIC_EnableIRQ(UART4_IRQn);

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  LL_USART_Init(UART4, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(UART4);
  LL_USART_Enable(UART4);
  /* USER CODE BEGIN UART4_Init 2 */

  /* DMA2 CH3: UART4 RX (STM32F105 하드와이어드 채널) */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

  LL_DMA_ConfigAddresses(DMA2, LL_DMA_CHANNEL_3,
                         (uint32_t)&UART4->DR,
                         (uint32_t)Comm1.DMA_Rx,
                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  LL_DMA_SetDataLength(DMA2, LL_DMA_CHANNEL_3, COM1_DMA_RX_BUF_SIZE);
  LL_DMA_SetMode(DMA2, LL_DMA_CHANNEL_3, LL_DMA_MODE_CIRCULAR);
  LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_CHANNEL_3, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_CHANNEL_3, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetMemorySize(DMA2, LL_DMA_CHANNEL_3, LL_DMA_MDATAALIGN_BYTE);
  LL_DMA_SetPeriphSize(DMA2, LL_DMA_CHANNEL_3, LL_DMA_PDATAALIGN_BYTE);
  LL_DMA_SetChannelPriorityLevel(DMA2, LL_DMA_CHANNEL_3, LL_DMA_PRIORITY_HIGH);

  LL_USART_EnableDMAReq_RX(UART4);
  LL_USART_EnableIT_IDLE(UART4);

  LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_3);

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  /**USART2 GPIO Configuration
  PA2   ------> USART2_TX
  PA3   ------> USART2_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USART2 interrupt Init */
  NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),3, 0));
  NVIC_EnableIRQ(USART2_IRQn);

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART2, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART2);
  LL_USART_Enable(USART2);
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOE);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);

  /**/
  LL_GPIO_ResetOutputPin(GPIOE, DRV1_IN3_Pin|DRV1_IN4_Pin|DRV1_nSLEEP_Pin|DRV2_IN3_Pin
                          |DRV2_IN4_Pin|DRV1_IN1_Pin|DRV1_IN2_Pin);

  /**/
  LL_GPIO_ResetOutputPin(GPIOC, DRV2_IN1_Pin|DRV2_IN2_Pin);

  /**/
  #ifdef BOARD_B
  LL_GPIO_ResetOutputPin(GPIOB, DRV2_nSLEEP_Pin|CpuRun_Pin);
  LL_GPIO_ResetOutputPin(GPIOE, I2C_SCL_Pin);
  #else
  LL_GPIO_ResetOutputPin(GPIOB, DRV2_nSLEEP_Pin|I2C_SCL_Pin|CpuRun_Pin);
  #endif

  /**/
  LL_GPIO_ResetOutputPin(GPIOD, DRV3_nSLEEP_Pin|DRV3_IN1_Pin|DRV3_IN2_Pin);

  /**/
  LL_GPIO_SetOutputPin(I2C_SDA_GPIO_Port, I2C_SDA_Pin);

  /**/
  GPIO_InitStruct.Pin = DRV1_IN3_Pin|DRV1_IN4_Pin|DRV1_nSLEEP_Pin|DRV2_IN3_Pin
                          |DRV2_IN4_Pin|DRV1_IN1_Pin|DRV1_IN2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = DRV1_nFAULT_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(DRV1_nFAULT_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = DRV2_IN1_Pin|DRV2_IN2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = DRV2_nSLEEP_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(DRV2_nSLEEP_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = DRV2_nFAULT_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(DRV2_nFAULT_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LMT_CHAIR2_FRT_Pin|LMT_CHAIR2_BACK_Pin|LMT_CHAIR2_DW_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /**/
  #ifdef BOARD_B
  GPIO_InitStruct.Pin = SW_CHAIR2_FWD_Pin|SW_CHAIR2_BACK_Pin|SW_CHAIR2_UP_Pin|SW_CHAIR2_DW_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  #else
  GPIO_InitStruct.Pin = SW_CHAIR2_FWD_Pin|SW_CHAIR2_BACK_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = SW_CHAIR2_UP_Pin|SW_CHAIR2_DW_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  #endif

  /**/
  GPIO_InitStruct.Pin = I2C_SCL_Pin|I2C_SDA_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(I2C_SCL_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = CpuRun_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(CpuRun_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = DRV3_nSLEEP_Pin|DRV3_IN1_Pin|DRV3_IN2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = DRV3_nFAULT_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(DRV3_nFAULT_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LMT_ROBOT_DW_Pin|LMT_ROBOT_UP_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /**/
  #ifdef BOARD_B
  GPIO_InitStruct.Pin = SW_CHAIR1_OK_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SW_ROBOT_DW_Pin|SW_ROBOT_UP_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SW_CHAIR1_DW_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  #else
  GPIO_InitStruct.Pin = SW_ROBOT_DW_Pin|SW_ROBOT_UP_Pin|SW_CHAIR1_OK_Pin|SW_CHAIR1_DW_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  #endif

  /**/
  GPIO_InitStruct.Pin = SW_CHAIR1_UP_Pin|SW_CHAIR1_BACK_Pin|SW_CHAIR1_FWD_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LMT_CHAIR1_DW_Pin|LMT_CHAIR1_BACK_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LMT_CHAIR1_FRT_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(LMT_CHAIR1_FRT_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
u8 LeftChairGoFront(u8 Auto, u16 TargetPos)
{
	u8 time_over = 0;
	u16 loop_count = 0;
	
	//if(LIMIT_CHAIR1_FRONT_CHECK == ACTIVE_LOW)
	if((LIMIT_CHAIR1_FRONT_CHECK == ACTIVE_LOW) && (g_is_homing ==0)) 
		return 0;
	if(TargetPos > 10){
		TargetPos = TargetPos - PRE_STOP;
	}
	BDCMotor1.MotorTimer = FWD_BACK_LIMIT_TIME;
	DRV_CHAIR1_FWD_HIGH;
	DRV_CHAIR1_REV_HIGH;
	#ifdef MOTOR_MULTI_MOVE
	BDCMotor1.Direction = INCREASE;
	#else
	printf("Left Chair Forward(D1):  %d\n",LaserChair1Front.CurrentPosition);
	while(1){
		loop_count++;
		if(loop_count == 10000){
			printf("D1:  %d\n",LaserChair1Front.CurrentPosition);
			loop_count = 0;
		}
		if(Auto == 1){
			if(LaserChair1Front.CurrentPosition <= TargetPos){
				printf("Forward..Left Chair reach the target: %d\n", LaserChair1Front.CurrentPosition);
				break;
			}
			else if(BDCMotor1.MotorTimer == 0){
				printf("Time Over\n");
				time_over = 1;
				break;
			}
		}
		else{
			if(LIMIT_CHAIR1_FRONT_CHECK == ACTIVE_LOW){
				printf("Chair1 FWD Limit SW: %d\n", LaserChair1Front.CurrentPosition);
				break;
			}
			else if(SW_CHAIR1_FWD_CHECK == LOW_INACTIVE){
				printf("Chair1 fwd button release\n");
				break;

			}
			else if(BDCMotor1.MotorTimer == 0){
				time_over = 1;
				break;
			}
		}

	}
	DRV_CHAIR1_FWD_LOW;
	while(SW_CHAIR1_FWD_CHECK == HIGH_ACTIVE);
	#endif
	return time_over;	
}

u8 LeftChairGoBack(u8 Auto, u16 TargetPos)//OK
{
	u8 time_over = 0;
	u16 loop_count = 0;
	
	//if(LIMIT_CHAIR1_BACK_CHECK == ACTIVE_LOW)
	if((LIMIT_CHAIR1_BACK_CHECK == ACTIVE_LOW) && (g_is_homing ==0))
		return 0;
	if(TargetPos > 10){
		TargetPos = TargetPos + PRE_STOP;
	}
	BDCMotor1.MotorTimer = FWD_BACK_LIMIT_TIME;
	DRV_CHAIR1_FWD_HIGH;
	DRV_CHAIR1_REV_LOW;
	printf("Left Chair Backward(D1):  %d\n",LaserChair1Front.CurrentPosition);
	while(1){
		loop_count++;
		if(loop_count == 10000){
			printf("D1:  %d\n",LaserChair1Front.CurrentPosition);
			loop_count = 0;
		}
		if(Auto == 1){
			if(LaserChair1Front.CurrentPosition >= TargetPos){
				printf("Backward..Left Chair reach the target: %d\n", LaserChair1Front.CurrentPosition);
				break;
			}
			else if(LIMIT_CHAIR1_BACK_CHECK == ACTIVE_LOW){
				printf("Chair1 BACK Limit SW: %d\n", LaserChair1Front.CurrentPosition);
				break;
			}
			else if(BDCMotor1.MotorTimer == 0){
				printf("Time Over\n");
				time_over = 1;
				break;
			}
		}
		else{
			if(LIMIT_CHAIR1_BACK_CHECK == ACTIVE_LOW){
				printf("Chair1 BACK Limit SW: %d\n", LaserChair1Front.CurrentPosition);
				break;
			}
			else if(SW_CHAIR1_BACK_CHECK == LOW_INACTIVE){
				printf("Chair1 back button release\n");
				break;
			}
			else if(BDCMotor1.MotorTimer == 0){
				time_over = 1;
				break;
			}
		}

	}
	DRV_CHAIR1_FWD_LOW;
	while(SW_CHAIR1_BACK_CHECK == HIGH_ACTIVE);
	
	return time_over;
}

u8 LeftChairGoUp(u8 Auto, u16 TargetPos)//OK
{
	u8 time_over = 0;
	u16 loop_count = 0;
	
	BDCMotor2.MotorTimer = UP_DOWN_LIMIT_TIME;
	DRV_CHAIR1_UP_HIGH;
	DRV_CHAIR1_DW_LOW;
	printf("Left Chair Up(H1):  %d\n",PotentioChair1Height.CurrentPosition);
	while(1){
		loop_count++;
		if(loop_count == 10000){
			printf("H1:  %d\n",PotentioChair1Height.CurrentPosition);
			loop_count = 0;
		}
		if(Auto == 1){
			if(PotentioChair1Height.CurrentPosition >= TargetPos){
				printf("Backward..Left Chair reach the target: %d\n", PotentioChair1Height.CurrentPosition);
				break;
			}
			else if(BDCMotor2.MotorTimer == 0){
				printf("Time Over\n");
				time_over = 1;
				break;
			}
		}
		else{
			if(SW_CHAIR1_UP_CHECK == LOW_INACTIVE){
				printf("Chair1 up button release\n");
				break;
			}
			else if(BDCMotor2.MotorTimer == 0){
				time_over = 1;
				break;
			}
		}
	}
	DRV_CHAIR1_UP_LOW;
	while(SW_CHAIR1_UP_CHECK == HIGH_ACTIVE);
	return time_over;
}

u8 LeftChairGoDown(u8 Auto, u16 TargetPos)//OK
{
	u8 time_over = 0;
	u16 loop_count = 0;
	u16 lastPosition = 0;
	u16 startPosition = 0;
	u8 descentConfirmed = 0;
	s16 diff;

	//if(LIMIT_CHAIR1_DW_CHECK == ACTIVE_LOW)
	if((LIMIT_CHAIR1_DW_CHECK == ACTIVE_LOW) && (g_is_homing ==0))
		return 0;
	BDCMotor2.MotorTimer = UP_DOWN_LIMIT_TIME;
	startPosition = PotentioChair1Height.CurrentPosition;
	lastPosition = PotentioChair1Height.CurrentPosition;
	PotentioChair1Height.HomeCheckFlag = 0;
	DRV_CHAIR1_UP_HIGH;
	DRV_CHAIR1_DW_HIGH;
	printf("Left Chair Down(H1) start:  %d\n",PotentioChair1Height.CurrentPosition);
	while(1){
		loop_count++;
		if(loop_count == 10000){
			printf("H1:  %d\n",PotentioChair1Height.CurrentPosition);
			loop_count = 0;
		}
		if(Auto == 1){
			if(PotentioChair1Height.CurrentPosition <= TargetPos){
				printf("Going down..Left Chair reach the target: %d\n", PotentioChair1Height.CurrentPosition);
				break;
			}
			else if(TargetPos == 0){
				// 1단계: 하강 확인 OR "이미 최하점" 판정
				if(descentConfirmed == 0){
					// 하강 감지: 시작값보다 2 이상 감소
					if((startPosition > PotentioChair1Height.CurrentPosition) &&
					   ((startPosition - PotentioChair1Height.CurrentPosition) >= 2)){
						descentConfirmed = 1;
						lastPosition = PotentioChair1Height.CurrentPosition;
						PotentioChair1Height.HomeCheckFlag = 0;
						printf("Left Chair descent confirmed at: %d\n", PotentioChair1Height.CurrentPosition);
					}
					// 이미 최하점 판정: startPosition ±1 내에서 3초 유지 → 현재 위치 홈 설정
					else{
						diff = (s16)PotentioChair1Height.CurrentPosition - (s16)startPosition;
						if((diff >= -1) && (diff <= 1)){
							if(PotentioChair1Height.HomeCheckFlag == 0){
								PotentioChair1Height.HomeCheckTimer = 3000;
								PotentioChair1Height.HomeCheckFlag = 1;
							}
							else if(PotentioChair1Height.HomeCheckTimer == 0){
								PotentioChair1Height.HomeCheckFlag = 0;
								printf("Left Chair already at bottom: %d\n", PotentioChair1Height.CurrentPosition);
								break;
							}
						}
						else{
							PotentioChair1Height.HomeCheckTimer = 3000;
							PotentioChair1Height.HomeCheckFlag = 1;
						}
					}
				}
				// 2단계: 하강 확인 후 ±1 허용 오차로 3초 안정화 검사
				else{
					diff = (s16)PotentioChair1Height.CurrentPosition - (s16)lastPosition;
					if((diff >= -1) && (diff <= 1)){
						if(PotentioChair1Height.HomeCheckFlag == 0){
							PotentioChair1Height.HomeCheckTimer = 3000;
							PotentioChair1Height.HomeCheckFlag = 1;
						}
						else if(PotentioChair1Height.HomeCheckTimer == 0){
							PotentioChair1Height.HomeCheckFlag = 0;
							printf("Left Chair Height stable at: %d\n", PotentioChair1Height.CurrentPosition);
							break;
						}
					}
					else{
						lastPosition = PotentioChair1Height.CurrentPosition;
						PotentioChair1Height.HomeCheckTimer = 3000;
						PotentioChair1Height.HomeCheckFlag = 1;
					}
				}
				/* 이전 버전 (단순 3초 안정화 - 모터 반응 지연 중 오판 문제)
				if(PotentioChair1Height.CurrentPosition == lastPosition){
					if(PotentioChair1Height.HomeCheckFlag == 0){
						PotentioChair1Height.HomeCheckTimer = 3000;
						PotentioChair1Height.HomeCheckFlag = 1;
					}
					else if(PotentioChair1Height.HomeCheckTimer == 0){
						PotentioChair1Height.HomeCheckFlag = 0;
						printf("Left Chair Height stable at: %d\n", PotentioChair1Height.CurrentPosition);
						break;
					}
				}
				else{
					lastPosition = PotentioChair1Height.CurrentPosition;
					PotentioChair1Height.HomeCheckTimer = 3000;
					PotentioChair1Height.HomeCheckFlag = 1;
				}*/
				/*
				if((PotentioChair1Height.CurrentPosition < POTENTIO_CHAIR1_HOME_POSITION) && (PotentioChair1Height.HomeCheckFlag == 0)){
					PotentioChair1Height.HomeCheckTimer = 5000;
					PotentioChair1Height.HomeCheckFlag = 1;
				}
				else if((PotentioChair1Height.HomeCheckTimer < 100) && (PotentioChair1Height.HomeCheckTimer > 10)){
					PotentioChair1Height.HomeCheckFlag = 0;
					break;
				}
				*/
				if(BDCMotor2.MotorTimer == 0){
					printf("Time Over\n");
					time_over = 1;
					break;
				}
			}
			else if(BDCMotor2.MotorTimer == 0){
				printf("Time Over\n");
				time_over = 1;
				break;
			}
		}
		else{
			if(SW_CHAIR1_DW_CHECK == LOW_INACTIVE){
				printf("Chair1 down button release\n");
				break;
			}
			else if(BDCMotor2.MotorTimer == 0){
				time_over = 1;
				break;
			}
		}
	}
	DRV_CHAIR1_UP_LOW;
	while(SW_CHAIR1_DW_CHECK == HIGH_ACTIVE);
	
	return time_over;
}


u8 RightChairGoFront(u8 Auto, u16 TargetPos)//OK
{
	u8 time_over = 0;
	u16 loop_count = 0;
	
	// if(LIMIT_CHAIR2_FRONT_CHECK == ACTIVE_LOW)
	if((LIMIT_CHAIR2_FRONT_CHECK == ACTIVE_LOW) && (g_is_homing ==0))
		return 0;
	if(TargetPos > 10){
		TargetPos = TargetPos - PRE_STOP;
	}
	BDCMotor3.MotorTimer = FWD_BACK_LIMIT_TIME;
	DRV_CHAIR2_FWD_HIGH;
	DRV_CHAIR2_REV_LOW;
	printf("Right Chair Forward(D2):  %d\n",LaserChair2Front.CurrentPosition);
	while(1){
		loop_count++;
		if(loop_count == 10000){
			printf("D2:  %d\n",LaserChair2Front.CurrentPosition);
			loop_count = 0;
		}
		if(Auto == 1){

			if(LaserChair2Front.CurrentPosition <= TargetPos){
				printf("Forward..Left Chair reach the target: %d\n", LaserChair2Front.CurrentPosition);
				break;
			}
			else if(BDCMotor3.MotorTimer == 0){
				printf("Time Over\n");
				time_over = 1;
				break;
			}
		}
		else{
			if(LIMIT_CHAIR2_FRONT_CHECK == ACTIVE_LOW){
				printf("Chair2 FWD Limit SW: %d\n", LaserChair2Front.CurrentPosition);
				break;
			}
			else if(SW_CHAIR2_FWD_CHECK == LOW_INACTIVE){
				printf("Chair2 FWD button release\n");
				break;
			}
			else if(BDCMotor3.MotorTimer == 0){
				time_over = 1;
				break;
			}
		}

	}
	DRV_CHAIR2_FWD_LOW;
	while(SW_CHAIR2_FWD_CHECK == HIGH_ACTIVE);
	return time_over;
}
u8 RightChairGoBack(u8 Auto, u16 TargetPos)//OK
{
	u8 time_over = 0;
	u16 loop_count = 0;
	
	if(LIMIT_CHAIR2_BACK_CHECK == ACTIVE_LOW)
		return 0;
	if(TargetPos > 10){
		TargetPos = TargetPos + PRE_STOP;
	}
	BDCMotor3.MotorTimer = FWD_BACK_LIMIT_TIME;
	DRV_CHAIR2_FWD_HIGH;
	DRV_CHAIR2_REV_HIGH;
	printf("Right Chair Backward(D2):  %d\n",LaserChair2Front.CurrentPosition);
	while(1){
		loop_count++;
		if(loop_count == 10000){
			printf("D2:  %d\n",LaserChair2Front.CurrentPosition);
			loop_count = 0;
		}
		if(Auto == 1){
			if(LaserChair2Front.CurrentPosition >= TargetPos){
				printf("Backward..Left Chair reach the target: %d\n", LaserChair2Front.CurrentPosition);
				break;
			}
			else if(LIMIT_CHAIR2_BACK_CHECK == ACTIVE_LOW){
				printf("Chair2 BACK Limit SW: %d\n", LaserChair2Front.CurrentPosition);
				break;
			}
			else if(BDCMotor3.MotorTimer == 0){
				printf("Time Over\n");
				time_over = 1;
				break;
			}
		}
		else{
			if(LIMIT_CHAIR2_BACK_CHECK == ACTIVE_LOW){
				printf("Chair2 BACK Limit SW: %d\n", LaserChair2Front.CurrentPosition);
				break;
			}
			else if(SW_CHAIR2_BACK_CHECK == LOW_INACTIVE){
				printf("Chair2 back button release\n");
				break;
			}
			else if(BDCMotor3.MotorTimer == 0){
				time_over = 1;
				break;
			}
		}
	}
	DRV_CHAIR2_FWD_LOW;
	while(SW_CHAIR2_BACK_CHECK == HIGH_ACTIVE);
	
	return time_over;
}
u8 RightChairGoUp(u8 Auto, u16 TargetPos)//OK
{
	u8 time_over = 0;
	u16 loop_count = 0;
	
	//if(LIMIT_CHAIR1_UP_CHECK == ACTIVE_LOW)
	//	return 0;
	BDCMotor4.MotorTimer = UP_DOWN_LIMIT_TIME;
	DRV_CHAIR2_UP_HIGH;
	DRV_CHAIR2_DW_HIGH;
	printf("Right Chair Up(H2):  %d\n",PotentioChair2Height.CurrentPosition);
	while(1){
		loop_count++;
		if(loop_count == 10000){
			printf("H2:  %d\n",PotentioChair2Height.CurrentPosition);
			loop_count = 0;
		}
		if(Auto == 1){
			if(PotentioChair2Height.CurrentPosition >= TargetPos){
				printf("Up..Right Chair reach the target: %d\n", PotentioChair2Height.CurrentPosition);
				break;
			}
			else if(BDCMotor4.MotorTimer == 0){
				printf("Time Over\n");
				time_over = 1;
				break;
			}
			
		}
		else{
			if(SW_CHAIR2_UP_CHECK == LOW_INACTIVE){
				printf("Chair2 up button release\n");
				break;
			}
			else if(BDCMotor4.MotorTimer == 0){
				time_over = 1;
				break;
			}
		}

	}
	DRV_CHAIR2_UP_LOW;
	while(SW_CHAIR2_UP_CHECK == HIGH_ACTIVE);
	return time_over;
}
u8 RightChairGoDown(u8 Auto, u16 TargetPos)//OK
{
	u8 time_over = 0;
	u16 loop_count = 0;
	u16 lastPosition = 0;
	u16 startPosition = 0;
	u8 descentConfirmed = 0;
	s16 diff;

	if(LIMIT_CHAIR2_DW_CHECK == ACTIVE_LOW)
		return 0;
	BDCMotor4.MotorTimer = UP_DOWN_LIMIT_TIME;
	startPosition = PotentioChair2Height.CurrentPosition;
	lastPosition = PotentioChair2Height.CurrentPosition;
	PotentioChair2Height.HomeCheckFlag = 0;
	DRV_CHAIR2_UP_HIGH;
	DRV_CHAIR2_DW_LOW;
	printf("Right Chair Down(H2) start:  %d\n",PotentioChair2Height.CurrentPosition);
	while(1){
		loop_count++;
		if(loop_count == 10000){
			printf("H2:  %d\n",PotentioChair2Height.CurrentPosition);
			loop_count = 0;
		}
		if(Auto == 1){
			if(PotentioChair2Height.CurrentPosition <= TargetPos){
				printf("Down..Right Chair reach the target: %d\n", PotentioChair2Height.CurrentPosition);
				break;
			}
			else if(TargetPos == 0){
				// 1단계: 하강 확인 OR "이미 최하점" 판정
				if(descentConfirmed == 0){
					// 하강 감지: 시작값보다 2 이상 감소
					if((startPosition > PotentioChair2Height.CurrentPosition) &&
					   ((startPosition - PotentioChair2Height.CurrentPosition) >= 2)){
						descentConfirmed = 1;
						lastPosition = PotentioChair2Height.CurrentPosition;
						PotentioChair2Height.HomeCheckFlag = 0;
						printf("Right Chair descent confirmed at: %d\n", PotentioChair2Height.CurrentPosition);
					}
					// 이미 최하점 판정: startPosition ±1 내에서 3초 유지 → 현재 위치 홈 설정
					else{
						diff = (s16)PotentioChair2Height.CurrentPosition - (s16)startPosition;
						if((diff >= -1) && (diff <= 1)){
							if(PotentioChair2Height.HomeCheckFlag == 0){
								PotentioChair2Height.HomeCheckTimer = 3000;
								PotentioChair2Height.HomeCheckFlag = 1;
							}
							else if(PotentioChair2Height.HomeCheckTimer == 0){
								PotentioChair2Height.HomeCheckFlag = 0;
								printf("Right Chair already at bottom: %d\n", PotentioChair2Height.CurrentPosition);
								break;
							}
						}
						else{
							PotentioChair2Height.HomeCheckTimer = 3000;
							PotentioChair2Height.HomeCheckFlag = 1;
						}
					}
				}
				// 2단계: 하강 확인 후 ±1 허용 오차로 3초 안정화 검사
				else{
					diff = (s16)PotentioChair2Height.CurrentPosition - (s16)lastPosition;
					if((diff >= -1) && (diff <= 1)){
						if(PotentioChair2Height.HomeCheckFlag == 0){
							PotentioChair2Height.HomeCheckTimer = 3000;
							PotentioChair2Height.HomeCheckFlag = 1;
						}
						else if(PotentioChair2Height.HomeCheckTimer == 0){
							PotentioChair2Height.HomeCheckFlag = 0;
							printf("Right Chair Height stable at: %d\n", PotentioChair2Height.CurrentPosition);
							break;
						}
					}
					else{
						lastPosition = PotentioChair2Height.CurrentPosition;
						PotentioChair2Height.HomeCheckTimer = 3000;
						PotentioChair2Height.HomeCheckFlag = 1;
					}
				}
				/* 이전 버전 (단순 3초 안정화 - 모터 반응 지연 중 오판 문제)
				if(PotentioChair2Height.CurrentPosition == lastPosition){
					if(PotentioChair2Height.HomeCheckFlag == 0){
						PotentioChair2Height.HomeCheckTimer = 3000;
						PotentioChair2Height.HomeCheckFlag = 1;
					}
					else if(PotentioChair2Height.HomeCheckTimer == 0){
						PotentioChair2Height.HomeCheckFlag = 0;
						printf("Right Chair Height stable at: %d\n", PotentioChair2Height.CurrentPosition);
						break;
					}
				}
				else{
					lastPosition = PotentioChair2Height.CurrentPosition;
					PotentioChair2Height.HomeCheckTimer = 3000;
					PotentioChair2Height.HomeCheckFlag = 1;
				}*/
				/*
				if((PotentioChair2Height.CurrentPosition < POTENTIO_CHAIR2_HOME_POSITION) && (PotentioChair2Height.HomeCheckFlag == 0)){
					PotentioChair2Height.HomeCheckTimer = 5000;
					PotentioChair2Height.HomeCheckFlag = 1;
					printf("flag: %d\n",PotentioChair2Height.HomeCheckFlag);
				}
				else if((PotentioChair2Height.HomeCheckTimer < 100) && (PotentioChair2Height.HomeCheckTimer > 10)){
					PotentioChair2Height.HomeCheckFlag = 0;
					printf("flag: %d\n",PotentioChair2Height.HomeCheckFlag);
					break;
				}
				*/
				if(BDCMotor4.MotorTimer == 0){
					printf("Time Over\n");
					time_over = 1;
					break;
				}
			}
			else if(BDCMotor4.MotorTimer == 0){
				printf("Time Over\n");
				time_over = 1;
				break;
			}
		}
		else{
			if(SW_CHAIR2_DW_CHECK == LOW_INACTIVE){
				printf("Chair2 down button release\n");
				break;
			}
			else if(BDCMotor4.MotorTimer == 0){
				time_over = 1;
				break;
			}
		}
	}
	DRV_CHAIR2_UP_LOW;
	while(SW_CHAIR2_DW_CHECK == HIGH_ACTIVE);
	
	return time_over;
}
u8 RobotGoUp(u8 Auto, u16 TargetPos)//OK
{
	u8 time_over = 0;
	u16 loop_count = 0;

	if(TargetPos > 10){
		TargetPos = TargetPos -PRE_STOP;
	}
	//if(LaserRobotUp.CurrentPosition > 250){
	if(LIMIT_ROBOT_UP_CHECK == ACTIVE_LOW){
		//printf("Robot Up limit\n");
		return 0;
	}
	BDCMotor5.MotorTimer = ROBOT_UP_DOWN_LIMIT_TIME;
	DRV_ROBOT_UP_HIGH;
	DRV_ROBOT_DW_LOW;
	printf("Robot Up(H3):  %d\n",LaserRobotUp.CurrentPosition);
	while(1){
		loop_count++;
		if(loop_count == 10000){
			printf("H3:  %d\n",LaserRobotUp.CurrentPosition);
			loop_count = 0;
		}
		if(Auto == 1){
			if(LaserRobotUp.CurrentPosition >= TargetPos){
				printf("Up..Robot reach the target: %d\n", LaserRobotUp.CurrentPosition);
				break;
			}
			else if(LIMIT_ROBOT_UP_CHECK == ACTIVE_LOW){
				printf("Robot going up limit: %d\n", LaserRobotUp.CurrentPosition);
				break;
			}
			else if(BDCMotor5.MotorTimer == 0){
				printf("Time Over\n");
				time_over = 1;
				break;
			}
		}
		else{
			//if(LaserRobotUp.CurrentPosition > 250){
			if(LIMIT_ROBOT_UP_CHECK == ACTIVE_LOW){
				printf("Robot going up limit: %d\n", LaserRobotUp.CurrentPosition);
				break;
			}
			else if(SW_ROBOT_UP_CHECK == LOW_INACTIVE){

				printf("Robot up button release\n");
				break;
			}	
			else if(BDCMotor5.MotorTimer == 0){
				printf("Time Over\n");
				time_over = 1;
				break;
			}
		}
	}
	DRV_ROBOT_UP_LOW;
	while(SW_ROBOT_UP_CHECK == HIGH_ACTIVE);
	return time_over;
}
u8 RobotGoDown(u8 Auto, u16 TargetPos)//OK
{
	u8 time_over = 0;
	u16 loop_count = 0;

	if(LIMIT_ROBOT_DW_CHECK == ACTIVE_LOW){
		//printf("Robot Down limit\n");
		return 0;
	}
	if(TargetPos > 10){
		TargetPos = TargetPos + PRE_STOP;
	}
	BDCMotor5.MotorTimer = ROBOT_UP_DOWN_LIMIT_TIME;
	DRV_ROBOT_UP_HIGH;
	DRV_ROBOT_DW_HIGH;
	printf("Robot Down(H3):  %d\n",LaserRobotUp.CurrentPosition);
	while(1){
		loop_count++;
		if(loop_count == 10000){
			printf("H3:  %d\n",LaserRobotUp.CurrentPosition);
			loop_count = 0;
		}
		if(Auto == 1){
			if(LaserRobotUp.CurrentPosition <= TargetPos){
				printf("Down..Robot reach the target: %d\n", LaserRobotUp.CurrentPosition);
				break;
			}
			if(LIMIT_ROBOT_DW_CHECK == ACTIVE_LOW){
				printf("Robot going down limit: %d\n", LaserRobotUp.CurrentPosition);
				break;
			}
			else if(BDCMotor5.MotorTimer == 0){
				printf("Time Over\n");
				time_over = 1;
				break;
			}
		}
		else{
			if(LIMIT_ROBOT_DW_CHECK == ACTIVE_LOW){
				printf("Robot going down limit: %d\n", LaserRobotUp.CurrentPosition);
				break;
			}
			else if(SW_ROBOT_DW_CHECK == LOW_INACTIVE){

				printf("Robot down button release\n");
				break;
			}	
			else if(BDCMotor5.MotorTimer == 0){
				printf("Time Over\n");
				time_over = 1;
				break;
			}
		}
	}
	DRV_ROBOT_UP_LOW;
	while(SW_ROBOT_DW_CHECK == HIGH_ACTIVE);
	return time_over;
}
void SPMode(void)
{
	while(SW_ROBOT_UP_CHECK == HIGH_ACTIVE);
	//ADConverter.SelectChannel = ADC_CHANNEL_44;
	while(1){
		delayMsec(100);
		//ADCSelectChannel(ADConverter.SelectChannel);
		if(SW_CHAIR1_FWD_CHECK == HIGH_ACTIVE){
			printf("Chair1 FWD Button\n");
			printf("Chair1 Front Laser distance:  %d\n",LaserChair1Front.CurrentPosition);
		}
		if((SW_CHAIR1_BACK_CHECK == HIGH_ACTIVE) && (SW_CHAIR2_UP_CHECK == LOW_INACTIVE)){
			printf("Chair1 BACK Button\n");
		}
		if((SW_CHAIR1_UP_CHECK == HIGH_ACTIVE) && (SW_CHAIR2_BACK_CHECK == LOW_INACTIVE)){
			printf("Chair1 UP Button\n");
			printf("Chair2 Up Laser distance:  %d\n",LaserChair1Up.CurrentPosition);
			printf("Chair1 Up Potentiometer distance:  %d\n",PotentioChair1Height.CurrentPosition);
			
		}
		if(SW_CHAIR1_DW_CHECK == HIGH_ACTIVE){
			printf("Chair1 DOWN Button\n");
		}
		if(SW_CHAIR2_FWD_CHECK == HIGH_ACTIVE){
			printf("Chair2 FWD Button\n");
			printf("Chair2 Front Laser distance:  %d\n",LaserChair2Front.CurrentPosition);
		}
		if((SW_CHAIR2_BACK_CHECK == HIGH_ACTIVE) && (SW_CHAIR1_UP_CHECK == LOW_INACTIVE)){
			printf("Chair2 BACK Button\n");
		}
		if((SW_CHAIR2_UP_CHECK == HIGH_ACTIVE) && (SW_CHAIR1_BACK_CHECK == LOW_INACTIVE)){
			printf("Chair2 UP Button\n");
			printf("Chair2 Up Laser distance:  %d\n",LaserChair2Up.CurrentPosition);
			printf("Chair2 Up Potentiometer distance:  %d\n",PotentioChair2Height.CurrentPosition);
		}
		if(SW_CHAIR2_DW_CHECK == HIGH_ACTIVE){
			printf("Chair2 DOWN Button\n");
		}
		if(SW_ROBOT_UP_CHECK == HIGH_ACTIVE){
			printf("Robot UP Button\n");
			printf("Robot Up Laser distance:  %d\n",LaserRobotUp.CurrentPosition);
		}
		if(SW_ROBOT_DW_CHECK == HIGH_ACTIVE){
			printf("Robot DOWN Button\n");
		}
		if(LIMIT_CHAIR1_FRONT_CHECK == ACTIVE_LOW){
			printf("Chair1 FWD Limit SW\n");
		}
		if(LIMIT_CHAIR1_BACK_CHECK == ACTIVE_LOW){
			printf("Chair1 BACK Limit SW\n");
		}
		if(LIMIT_CHAIR1_DW_CHECK == ACTIVE_LOW){
			printf("Chair1 DOWN Limit SW\n");
		}
		if(LIMIT_CHAIR2_FRONT_CHECK == ACTIVE_LOW){
			printf("Chair2 FWD Limit SW\n");
		}
		if(LIMIT_CHAIR2_BACK_CHECK == ACTIVE_LOW){
			printf("Chair2 BACK Limit SW\n");
		}
		if(LIMIT_CHAIR2_DW_CHECK == ACTIVE_LOW){
			printf("Chair1 DOWN Limit SW\n");
		}
		if(LIMIT_ROBOT_UP_CHECK == ACTIVE_LOW){
			printf("Robot UP Limit SW\n");
		}
		if(LIMIT_ROBOT_DW_CHECK == ACTIVE_LOW){
			printf("Robot DW Limit SW\n");
		}
	}
}

void ADC1_Init_LL(void)
{
    u8 i;
	
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);


    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_1, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_2, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_3, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_ANALOG);


    LL_ADC_InitTypeDef adc_init = {0};
    adc_init.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    adc_init.SequencersScanMode = LL_ADC_SEQ_SCAN_ENABLE;
    LL_ADC_Init(ADC1, &adc_init);

    LL_ADC_REG_InitTypeDef adc_reg = {0};
    adc_reg.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    adc_reg.SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS;
    adc_reg.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
    adc_reg.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
    LL_ADC_REG_Init(ADC1, &adc_reg);

    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_10);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_11);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_12);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_4, LL_ADC_CHANNEL_13);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_5, LL_ADC_CHANNEL_14);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_6, LL_ADC_CHANNEL_0);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_7, LL_ADC_CHANNEL_4);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_8, LL_ADC_CHANNEL_5);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_9, LL_ADC_CHANNEL_6);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_10, LL_ADC_CHANNEL_7);

    for (i=0; i<10; i++)
        LL_ADC_SetChannelSamplingTime(ADC1, i + 10, LL_ADC_SAMPLINGTIME_7CYCLES_5); 

    LL_ADC_Enable(ADC1);
    LL_mDelay(1);
    LL_ADC_StartCalibration(ADC1);
    while (LL_ADC_IsCalibrationOnGoing(ADC1));
    LL_mDelay(1);


    LL_DMA_DeInit(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_InitTypeDef dma_init = {0};
    dma_init.PeriphOrM2MSrcAddress = (uint32_t)&ADC1->DR;
    dma_init.MemoryOrM2MDstAddress = (uint32_t)internal_adc_value;
    dma_init.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    dma_init.Mode = LL_DMA_MODE_CIRCULAR;
    dma_init.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_init.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_init.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
    dma_init.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
    dma_init.NbData = 10;
    dma_init.Priority = LL_DMA_PRIORITY_HIGH;
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &dma_init);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

    LL_ADC_REG_StartConversionSWStart(ADC1);
}


/*
0 STX
1 Length
2 Command
3 Data
4 CS
5 ETX

0 STX
1 Length MSB
2 Length LSB
3 Command
4 Address
5 data1

6 Checksum
7 ETX

Checksum = Command + Address+ Data1+...+DataN
lengh = command + address + data(min 3byte)
*/
void MasterComm1()
{
	u8 get_leng, checksum = 0, i = 0;
	
	if(Comm1.Flag == 1){
		get_leng = Comm1.RxBuf[2];
		//printf("%d, %d, %d,%d, %d, %d,%d, %d, %d\n", Comm.RxBuf[0],Comm.RxBuf[1],Comm.RxBuf[2],Comm.RxBuf[3],Comm.RxBuf[4],Comm.RxBuf[5],Comm.RxBuf[6],Comm.RxBuf[7],Comm.RxBuf[8]);
		if((Comm1.RxBuf[0] == STX) && (Comm1.RxBuf[get_leng+4] == ETX)){
			//checksum = Comm.RxBuf[3];
			//printf("rcv\n");
			
			for(i=3; i<(get_leng+3); i++){
				checksum += Comm1.RxBuf[i];
			}
			//printf("cs:%d\n", checksum);
			if(checksum == Comm1.RxBuf[get_leng+3]){
				printf("Received Command: 0x%x\n", Comm1.RxBuf[3]);
				UartProcess();
			}
			else 
				printf("checksum error\n");
			memset(Comm1.RxBuf, 0, sizeof(Comm1.RxBuf));
	        	Comm1.RxCount = 0;	
			Comm1.Flag = 0;   
		}
		else{
			memset(Comm1.RxBuf, 0, sizeof(Comm1.RxBuf));
	        	Comm1.RxCount = 0;	
			Comm1.Flag = 0;   
		}
	}
	if(Comm1.AckCheck == 1){
		if(Comm1.Timer == 0){
			Comm1.Retry++;
			if(Comm1.Retry < 3){
				//UartSendPacketMode();
			}
			else{
				printf("Send error: %d\n", Comm1.Retry);
				Comm1.AckCheck = 0;
				Comm1.Retry = 0;
			}
		}
	}
	return;
}

void UartResponseProcess(void)
{
	if(BDCMotor1.MoveState > 0){
		if(BDCMotor1.MoveState == MOVE_OK){
			UartSmartDeviceRequestGoTargetPositionResponse(1, 1);
		}
		else if(BDCMotor1.MoveState == MOVE_LIMIT){
			UartSmartDeviceRequestGoTargetPositionResponse(1, 1);
		}
		else if(BDCMotor1.MoveState == MOVE_TIME_OVER){
			UartPICSendErrorCode(LEFT_CHAIR, 2, 0);
		}
		BDCMotor1.MoveState = 0;
	}
	if(BDCMotor2.MoveState > 0){
		if(BDCMotor2.MoveState == MOVE_OK){
			UartSmartDeviceRequestGoTargetPositionResponse(1, 1);
		}
		else if(BDCMotor2.MoveState == MOVE_LIMIT){
			UartSmartDeviceRequestGoTargetPositionResponse(1, 1);
		}
		else if(BDCMotor2.MoveState == MOVE_TIME_OVER){
			UartPICSendErrorCode(LEFT_CHAIR, 2, 0);
		}
		BDCMotor2.MoveState = 0;
	}
	if(BDCMotor3.MoveState > 0){
		if(BDCMotor3.MoveState == MOVE_OK){
			UartSmartDeviceRequestGoTargetPositionResponse(1, 1);
		}
		else if(BDCMotor3.MoveState == MOVE_LIMIT){
			UartSmartDeviceRequestGoTargetPositionResponse(1, 1);
		}
		else if(BDCMotor3.MoveState == MOVE_TIME_OVER){
			UartPICSendErrorCode(RIGHT_CHAIR, 2, 0);
		}
		BDCMotor3.MoveState = 0;
	}
	if(BDCMotor4.MoveState > 0){
		if(BDCMotor4.MoveState == MOVE_OK){
			UartSmartDeviceRequestGoTargetPositionResponse(1, 1);
		}
		else if(BDCMotor4.MoveState == MOVE_LIMIT){
			UartSmartDeviceRequestGoTargetPositionResponse(1, 1);
		}
		else if(BDCMotor4.MoveState == MOVE_TIME_OVER){
			UartPICSendErrorCode(RIGHT_CHAIR, 2, 0);
		}
		BDCMotor4.MoveState = 0;
	}
	if(BDCMotor5.MoveState > 0){
		if(BDCMotor5.MoveState == MOVE_OK){
			UartSmartDeviceRequestGoTargetPositionResponse(1, 1);
		}
		else if(BDCMotor5.MoveState == MOVE_LIMIT){
			UartSmartDeviceRequestGoTargetPositionResponse(1, 1);
		}
		else if(BDCMotor5.MoveState == MOVE_TIME_OVER){
			UartPICSendErrorCode(ROBOT, 2, 0);
		}
		BDCMotor5.MoveState = 0;
	}
	
}

void UartProcess()
{
	u8 error = 0;
	
	switch (Comm1.RxBuf[3])//Command, Comm.RxBuf[4]: Address
	{
		
		case UART_SMART_DEVCIE_REQUEST_GO_HOME_POSITION:
			//UartPICSendErrorCode(Comm.RxBuf[5], 4, 0);
			//LaserChair1Up.HomePosition = 250;
			//LaserChair2Up.HomePosition = 250;
			BdcSystem.ChairName = Comm1.RxBuf[5];
			BdcSystem.MotorName = Comm1.RxBuf[6];
			UartSmartDeviceRequestGoHomePositionResponse(0, 0 ,0);
			printf("5:  %d, 6:  %d\n",BdcSystem.ChairName, BdcSystem.MotorName);
			if(BdcSystem.ChairName == LEFT_CHAIR){//Data1: Left/Right
				if(BdcSystem.MotorName == CHAIR_FRONT_BACK){
					if(LeftChairGoBack(1, 65535)){
						UartPICSendErrorCode(BdcSystem.ChairName, 4, 0);
					}
					else{
						LaserChair1Front.HomePosition = LaserChair1Front.CurrentPosition;
						LaserChair1Front.HomePosi[0] = (u8)((LaserChair1Front.HomePosition & 0xFF00) >> 8);
						LaserChair1Front.HomePosi[1] = (u8)(LaserChair1Front.HomePosition & 0x00FF);
						EEPROM_Write(EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_MSB, LaserChair1Front.HomePosi[0]);
						EEPROM_Write(EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_LSB, LaserChair1Front.HomePosi[1]);
						delayMsec(20);
						printf("Left Chair Back Home MSB:  %d, LSB:  %d\n",EEPROM_Read(EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_MSB), 
							EEPROM_Read(EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_LSB));
						printf("Left Chair Back Home Position:  %d\n", LaserChair1Front.HomePosition);
						UartSmartDeviceRequestGoHomePositionResponse(BdcSystem.ChairName, BdcSystem.MotorName,LaserChair1Front.HomePosition);
					}
				}
				else if(BdcSystem.MotorName == CHAIR_UP_DOWN){
					if(LeftChairGoDown(1, 0)){	
						UartPICSendErrorCode(BdcSystem.ChairName, 4, 0);
					}
					else{
						PotentioChair1Height.HomePosition = PotentioChair1Height.CurrentPosition;
						PotentioChair1Height.HomePosi[0] = (u8)((PotentioChair1Height.HomePosition & 0xFF00) >> 8);
						PotentioChair1Height.HomePosi[1] = (u8)(PotentioChair1Height.HomePosition & 0x00FF);
						EEPROM_Write(EEPROM_ADDR_LEFT_CHAIR_HEIGHT_HOME_POSITION_MSB, PotentioChair1Height.HomePosi[0]);
						EEPROM_Write(EEPROM_ADDR_LEFT_CHAIR_HEIGHT_HOME_POSITION_LSB, PotentioChair1Height.HomePosi[1]);
						delayMsec(20);
						printf("Left Chair Down Home MSB:  %d, LSB:  %d\n",EEPROM_Read(EEPROM_ADDR_LEFT_CHAIR_HEIGHT_HOME_POSITION_MSB), 
							EEPROM_Read(EEPROM_ADDR_LEFT_CHAIR_HEIGHT_HOME_POSITION_LSB));
						printf("Left Chair Down Home Position:  %d\n", PotentioChair1Height.HomePosition);
						UartSmartDeviceRequestGoHomePositionResponse(BdcSystem.ChairName, BdcSystem.MotorName,PotentioChair1Height.HomePosition);
					}
				}
				break;
			}
			else if(BdcSystem.ChairName == RIGHT_CHAIR){
				if(BdcSystem.MotorName == CHAIR_FRONT_BACK){
					if(RightChairGoBack(1, 65535)){	//2026
					
						UartPICSendErrorCode(RIGHT_CHAIR, 4, 0);
					}
					else{
						LaserChair2Front.HomePosition = LaserChair2Front.CurrentPosition;
						LaserChair2Front.HomePosi[0] = (u8)((LaserChair2Front.HomePosition & 0xFF00) >> 8);
						LaserChair2Front.HomePosi[1] = (u8)(LaserChair2Front.HomePosition & 0x00FF);
						EEPROM_Write(EEPROM_ADDR_RIGHT_CHAIR_BACK_HOME_POSITION_MSB, LaserChair2Front.HomePosi[0]);
						EEPROM_Write(EEPROM_ADDR_RIGHT_CHAIR_BACK_HOME_POSITION_LSB, LaserChair2Front.HomePosi[1]);
						delayMsec(20);
						printf("Right Chair Back Home MSB:  %d, LSB:  %d\n",EEPROM_Read(EEPROM_ADDR_RIGHT_CHAIR_BACK_HOME_POSITION_MSB), 
							EEPROM_Read(EEPROM_ADDR_RIGHT_CHAIR_BACK_HOME_POSITION_LSB));
						printf("Right Chair Back Home Position:  %d\n", LaserChair2Front.HomePosition);
						UartSmartDeviceRequestGoHomePositionResponse(BdcSystem.ChairName, BdcSystem.MotorName ,LaserChair2Front.HomePosition);
					}
				}
				else if(BdcSystem.MotorName == CHAIR_UP_DOWN){
					if(RightChairGoDown(1, 0)){	
						UartPICSendErrorCode(BdcSystem.ChairName , 4, 0);
					}
					else{
						PotentioChair2Height.HomePosition = PotentioChair2Height.CurrentPosition;
						PotentioChair2Height.HomePosi[0] = (u8)((PotentioChair2Height.HomePosition & 0xFF00) >> 8);
						PotentioChair2Height.HomePosi[1] = (u8)(PotentioChair2Height.HomePosition & 0x00FF);
						EEPROM_Write(EEPROM_ADDR_RIGHT_CHAIR_HEIGHT_HOME_POSITION_MSB, PotentioChair2Height.HomePosi[0]);
						EEPROM_Write(EEPROM_ADDR_RIGHT_CHAIR_HEIGHT_HOME_POSITION_LSB, PotentioChair2Height.HomePosi[1]);
						delayMsec(20);
						printf("Right Chair Down Home MSB:  %d, LSB:  %d\n",EEPROM_Read(EEPROM_ADDR_RIGHT_CHAIR_HEIGHT_HOME_POSITION_MSB), 
							EEPROM_Read(EEPROM_ADDR_RIGHT_CHAIR_HEIGHT_HOME_POSITION_LSB));
						printf("Right Chair Down Home Position:  %d\n", PotentioChair2Height.HomePosition);
						UartSmartDeviceRequestGoHomePositionResponse(BdcSystem.ChairName, BdcSystem.MotorName ,PotentioChair2Height.HomePosition);
					}
				}
				break;
			}
			else if(BdcSystem.ChairName == ROBOT){
				if(RobotGoDown(1, 0)){//Error
					UartPICSendErrorCode(LEFT_CHAIR, 4, 0);//left, error1, error2
				}
				else{
					LaserRobotUp.HomePosition = LaserRobotUp.CurrentPosition;
					LaserRobotUp.HomePosi[0] = (u8)((LaserRobotUp.HomePosition & 0xFF00) >> 8);
					LaserRobotUp.HomePosi[1] = (u8)(LaserRobotUp.HomePosition & 0x00FF);
					EEPROM_Write(EEPROM_ADDR_ROBOT_HEIGHT_HOME_POSITION_MSB, LaserRobotUp.HomePosi[0]);
					EEPROM_Write(EEPROM_ADDR_ROBOT_HEIGHT_HOME_POSITION_LSB, LaserRobotUp.HomePosi[1]);
					delayMsec(20);
					printf("Robot Down Home MSB:  %d, LSB:  %d\n",EEPROM_Read(EEPROM_ADDR_ROBOT_HEIGHT_HOME_POSITION_MSB), 
						EEPROM_Read(EEPROM_ADDR_ROBOT_HEIGHT_HOME_POSITION_LSB));
					printf("Robot Down Home Position:  %d\n", LaserRobotUp.HomePosition);
					UartSmartDeviceRequestGoHomePositionResponse(BdcSystem.ChairName, BdcSystem.MotorName, LaserRobotUp.HomePosition);//Whichone, Length
				}
				break;
			}
			else if(BdcSystem.ChairName == HM_LEFT_CHAIR){
				LaserChair1Up.HomePosition = 250;
				UartSmartDeviceRequestGoHomePositionResponse(BdcSystem.ChairName, BdcSystem.MotorName, LaserChair1Up.HomePosition);
				break;
			}
			else if(BdcSystem.ChairName == HM_RIGHT_CHAIR){
				LaserChair2Up.HomePosition = 250;
				UartSmartDeviceRequestGoHomePositionResponse(BdcSystem.ChairName, BdcSystem.MotorName, LaserChair2Up.HomePosition);
				break;
			}
		case UART_SMART_DEVCIE_REQUEST_GO_TARGET_POSITION:
			LaserChair1Up.HomePosition = 250;
			LaserChair2Up.HomePosition = 250;
                        BdcSystem.ChairName = Comm1.RxBuf[5];
			printf("Command 0x46 : Which Chair: %d\n", Comm1.RxBuf[5]);
			UartSmartDeviceRequestGoTargetPositionResponse(0,0);
			if(BdcSystem.ChairName == RIGHT_CHAIR){
				printf("Move Right Home\n");
				if(LIMIT_CHAIR1_BACK_CHECK == ACTIVE_LOW){

				}
				else{
					BDCMotor1.MotorTimer = FWD_BACK_LIMIT_TIME;
					DRV_CHAIR1_FWD_HIGH;
					DRV_CHAIR1_REV_LOW;
				}
				if(LIMIT_CHAIR1_DW_CHECK == ACTIVE_LOW){

				}
				else{
					BDCMotor2.MotorTimer = UP_DOWN_LIMIT_TIME;
					DRV_CHAIR1_UP_HIGH;
					DRV_CHAIR1_DW_HIGH;
				}
				while(1){
					if(LIMIT_CHAIR1_BACK_CHECK == ACTIVE_LOW)
						DRV_CHAIR1_FWD_LOW;
					if(PotentioChair1Height.CurrentPosition <= (PotentioChair1Height.HomePosition+1))
						DRV_CHAIR1_UP_LOW;
					if((LIMIT_CHAIR1_BACK_CHECK == ACTIVE_LOW) && 
						(PotentioChair1Height.CurrentPosition <= (PotentioChair1Height.HomePosition+1))){
						DRV_CHAIR1_FWD_LOW;
						DRV_CHAIR1_UP_LOW;
						break;

					}
					if((BDCMotor1.MotorTimer == 0) && (BDCMotor2.MotorTimer == 0)){
						DRV_CHAIR1_FWD_LOW;
						DRV_CHAIR1_UP_LOW;
						UartPICSendErrorCode(BdcSystem.ChairName, 2, 0);
						break;
					}
				}
			}
			else if(BdcSystem.ChairName == LEFT_CHAIR){
				printf("Move Left Home\n");
				if(LIMIT_CHAIR2_BACK_CHECK == ACTIVE_LOW){

				}
				else{
					BDCMotor3.MotorTimer = FWD_BACK_LIMIT_TIME;
					DRV_CHAIR2_FWD_HIGH;
					DRV_CHAIR2_REV_HIGH;
				}
				if(LIMIT_CHAIR2_DW_CHECK == ACTIVE_LOW){

				}
				else{
					BDCMotor4.MotorTimer = UP_DOWN_LIMIT_TIME;
					DRV_CHAIR2_UP_HIGH;
					DRV_CHAIR2_DW_LOW;
				}
				while(1){
					if(LIMIT_CHAIR2_BACK_CHECK == ACTIVE_LOW)
						DRV_CHAIR2_FWD_LOW;
					if(PotentioChair2Height.CurrentPosition <= (PotentioChair2Height.HomePosition+1))
						DRV_CHAIR2_UP_LOW;
					if((LIMIT_CHAIR2_BACK_CHECK == ACTIVE_LOW) && 
						(PotentioChair2Height.CurrentPosition <= (PotentioChair2Height.HomePosition+1))){
						DRV_CHAIR2_FWD_LOW;
						DRV_CHAIR2_UP_LOW;
						break;

					}
					if((BDCMotor3.MotorTimer == 0) && (BDCMotor4.MotorTimer == 0)){
						DRV_CHAIR2_FWD_LOW;
						DRV_CHAIR2_UP_LOW;
						UartPICSendErrorCode(BdcSystem.ChairName, 2, 0);
						break;
					}
				}
			}
			#if 1
			
			if(BdcSystem.ChairName == LEFT_CHAIR){
				LaserChair1Front.TargetPosition = ((u16)(Comm1.RxBuf[6] << 8) & 0xFF00) + (u16)(Comm1.RxBuf[7] & 0x00FF);
				PotentioChair1Height.TargetPosition = ((u16)(Comm1.RxBuf[8] << 8) & 0xFF00) + (u16)(Comm1.RxBuf[9] & 0x00FF);
				LaserRobotUp.TargetPosition = ((u16)(Comm1.RxBuf[10] << 8) & 0xFF00) + (Comm1.RxBuf[11] & 0x00FF);
				printf("Target: %d, %d, %d\n", LaserChair1Front.TargetPosition, PotentioChair1Height.TargetPosition, LaserRobotUp.TargetPosition);
				LaserChair1Front.TargetPosition = LaserChair1Front.HomePosition - LaserChair1Front.TargetPosition;
				PotentioChair1Height.TargetPosition = PotentioChair1Height.TargetPosition + PotentioChair1Height.HomePosition;
				LaserRobotUp.TargetPosition = LaserRobotUp.TargetPosition + LaserRobotUp.HomePosition;
				printf("Target + home: %d, %d, %d\n", LaserChair1Front.TargetPosition, PotentioChair1Height.TargetPosition, LaserRobotUp.TargetPosition);
				#if 1
				if(LaserChair1Front.TargetPosition < (LaserChair1Front.CurrentPosition + SENS_MARGIN)){
					if(LeftChairGoFront(1, LaserChair1Front.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 2, 0);
						printf("error1\n");
					}
					else{
						UartSmartDeviceRequestGoTargetPositionResponse(1, 1);//???: 1,F/B:1
						printf("response1\n");
					}	
				}
				else if(LaserChair1Front.TargetPosition > (LaserChair1Front.CurrentPosition - SENS_MARGIN)){
					if(LeftChairGoBack(1, LaserChair1Front.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 2, 0);
						printf("error2\n");
					}
					else{
						UartSmartDeviceRequestGoTargetPositionResponse(1, 1);//???: 1,F/B:1
						printf("response2\n");
					}
				}
				else{
					UartSmartDeviceRequestGoTargetPositionResponse(1, 1);//???: 1,F/B:1
					printf("response3\n");

				}

				if(PotentioChair1Height.TargetPosition > (PotentioChair1Height.CurrentPosition + SENS_MARGIN)){
					if(LeftChairGoUp(1, PotentioChair1Height.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 2, 0);
						printf("error3\n");
					}
					else{
						UartSmartDeviceRequestGoTargetPositionResponse(1, 2);//???: 1,U/D:2
						printf("response4\n");
					}
				}
				else if(PotentioChair1Height.TargetPosition < (PotentioChair1Height.CurrentPosition - SENS_MARGIN)){
					if(LeftChairGoDown(1, PotentioChair1Height.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 2, 0);
						printf("error4\n");
					}
					else{
						UartSmartDeviceRequestGoTargetPositionResponse(1, 2);//???: 1,U/D:2
						printf("response5\n");
					}
				}
				else{
					UartSmartDeviceRequestGoTargetPositionResponse(1, 2);//???: 1,U/D:2
				}
				if(LaserRobotUp.TargetPosition > (LaserRobotUp.CurrentPosition + SENS_MARGIN)){
					if(RobotGoUp(1, LaserRobotUp.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 4, 0);
						printf("error5\n");
					}
					else{
						UartSmartDeviceRequestGoTargetPositionResponse(1, 4);//???: 1,Robot U/D: 4
						printf("response6\n");
					}

				}
				else if(LaserRobotUp.TargetPosition < (LaserRobotUp.CurrentPosition - SENS_MARGIN)){
					if(RobotGoDown(1, LaserRobotUp.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 4, 0);
						printf("error6\n");
					}
					else{
						UartSmartDeviceRequestGoTargetPositionResponse(1, 4);//???: 1,Robot U/D: 4
						printf("response7\n");
					}
				}
				else{
					UartSmartDeviceRequestGoTargetPositionResponse(1, 4);//???: 1,Robot U/D: 4
					printf("response8\n");
				}
				#endif
			}
			else if(BdcSystem.ChairName == RIGHT_CHAIR){
				LaserChair2Front.TargetPosition = ((u16)(Comm1.RxBuf[6] << 8) & 0xFF00) + (u16)(Comm1.RxBuf[7] & 0x00FF);
				PotentioChair2Height.TargetPosition = ((u16)(Comm1.RxBuf[8] << 8) & 0xFF00) + (u16)(Comm1.RxBuf[9] & 0x00FF);
				LaserRobotUp.TargetPosition = ((u16)(Comm1.RxBuf[10] << 8) & 0xFF00) + (Comm1.RxBuf[11] & 0x00FF);
				printf("Target: %d, %d, %d\n", LaserChair2Front.TargetPosition, PotentioChair2Height.TargetPosition, LaserRobotUp.TargetPosition);
				LaserChair2Front.TargetPosition = LaserChair2Front.HomePosition - LaserChair2Front.TargetPosition;
				PotentioChair2Height.TargetPosition = PotentioChair2Height.TargetPosition + PotentioChair2Height.HomePosition;
				LaserRobotUp.TargetPosition = LaserRobotUp.TargetPosition + LaserRobotUp.HomePosition;

				printf("Target + home: %d, %d, %d\n", LaserChair2Front.TargetPosition, PotentioChair2Height.TargetPosition, LaserRobotUp.TargetPosition);
				#if 1
				if(LaserChair2Front.TargetPosition <  LaserChair2Front.CurrentPosition){
					if(RightChairGoFront(1, LaserChair2Front.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 2, 0);
					}
					else{
						UartSmartDeviceRequestGoTargetPositionResponse(1, 1);//???: 1,F/B:1
						printf("response1\n");
					}	
				}
				else{
					if(RightChairGoBack(1, LaserChair2Front.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 2, 0);
					}
					else{
						UartSmartDeviceRequestGoTargetPositionResponse(1, 1);//???: 1,F/B:1
						printf("response2\n");
					}
				}
				#endif
				if(PotentioChair2Height.TargetPosition > PotentioChair2Height.CurrentPosition){
					if(RightChairGoUp(1, PotentioChair2Height.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 2, 0);
					}
					else{
						//UartSmartDeviceRequestCurrentPositionResponse(BdcSystem.ChairName);
						printf("response3\n");
						UartSmartDeviceRequestGoTargetPositionResponse(1, 2);//???: 1,F/B:1
					}
				}
				else{
					if(RightChairGoDown(1, PotentioChair2Height.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 2, 0);
					}
					else{
						//UartSmartDeviceRequestCurrentPositionResponse(BdcSystem.ChairName);
						printf("response4\n");
						UartSmartDeviceRequestGoTargetPositionResponse(1, 2);//???: 1,F/B:1
					}
				}
				#if 1
				if(LaserRobotUp.TargetPosition > LaserRobotUp.CurrentPosition){
					if(RobotGoUp(1, LaserRobotUp.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 4, 0);
					}
					else{
						UartSmartDeviceRequestGoTargetPositionResponse(1, 4);//???: 1,Robot U/D: 4
						printf("response5\n");
					}

				}
				else{
					if(RobotGoDown(1, LaserRobotUp.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 4, 0);
					}
					else{
						UartSmartDeviceRequestGoTargetPositionResponse(1, 4);//???: 1,Robot U/D: 4
						printf("response6\n");
					}
				}
				#endif
			}
			else if(BdcSystem.ChairName == HM_LEFT_CHAIR){
				LaserChair1Front.TargetPosition = ((u16)(Comm1.RxBuf[6] << 8) & 0xFF00) + (u16)(Comm1.RxBuf[7] & 0x00FF);
				LaserChair1Up.TargetPosition = ((u16)(Comm1.RxBuf[8] << 8) & 0xFF00) + (u16)(Comm1.RxBuf[9] & 0x00FF);
				LaserRobotUp.TargetPosition = ((u16)(Comm1.RxBuf[10] << 8) & 0xFF00) + (Comm1.RxBuf[11] & 0x00FF);
				printf("Target: %d, %d, %d\n", LaserChair1Front.TargetPosition, LaserChair1Up.TargetPosition, LaserRobotUp.TargetPosition);
				LaserChair1Front.TargetPosition = LaserChair1Front.HomePosition - LaserChair1Front.TargetPosition;
				LaserChair1Up.TargetPosition = LaserChair1Up.TargetPosition + LaserChair1Up.HomePosition;
				LaserRobotUp.TargetPosition = LaserRobotUp.TargetPosition + LaserRobotUp.HomePosition;
				//printf("Healerbot HM LEFT\n", LaserRobotUp.TargetPosition,  LaserRobotUp.CurrentPosition);
				UartSmartDeviceRequestGoTargetPositionResponse(1, 1);
				delayMsec(500);
				UartSmartDeviceRequestGoTargetPositionResponse(1, 2);
				if(LaserRobotUp.TargetPosition > LaserRobotUp.CurrentPosition){
	
					if(RobotGoUp(1, LaserRobotUp.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 4, 0);
					}
					else{
						UartSmartDeviceRequestGoTargetPositionResponse(1, 4);//???: 1,Robot U/D: 4
						printf("response5\n");
					}

				}
				else{
					if(RobotGoDown(1, LaserRobotUp.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 4, 0);
					}
					else{
						UartSmartDeviceRequestGoTargetPositionResponse(1, 4);//???: 1,Robot U/D: 4
						printf("response6\n");
					}
				}
				break;
			}
			else if(BdcSystem.ChairName == HM_RIGHT_CHAIR){
				LaserChair2Front.TargetPosition = ((u16)(Comm1.RxBuf[6] << 8) & 0xFF00) + (u16)(Comm1.RxBuf[7] & 0x00FF);
				LaserChair2Up.TargetPosition = ((u16)(Comm1.RxBuf[8] << 8) & 0xFF00) + (u16)(Comm1.RxBuf[9] & 0x00FF);
				LaserRobotUp.TargetPosition = ((u16)(Comm1.RxBuf[10] << 8) & 0xFF00) + (Comm1.RxBuf[11] & 0x00FF);
				printf("Target: %d, %d, %d\n", LaserChair2Front.TargetPosition, LaserChair2Up.TargetPosition, LaserRobotUp.TargetPosition);
				LaserChair2Front.TargetPosition = LaserChair2Front.HomePosition - LaserChair2Front.TargetPosition;
				LaserChair2Up.TargetPosition = LaserChair2Up.TargetPosition + LaserChair2Up.HomePosition;
				LaserRobotUp.TargetPosition = LaserRobotUp.TargetPosition + LaserRobotUp.HomePosition;
				printf("Healerbot HM RIGHT\n");
				UartSmartDeviceRequestGoTargetPositionResponse(1, 1);
				delayMsec(500);
				UartSmartDeviceRequestGoTargetPositionResponse(1, 2);
				if(LaserRobotUp.TargetPosition > LaserRobotUp.CurrentPosition){
	
					if(RobotGoUp(1, LaserRobotUp.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 4, 0);
					}
					else{
						UartSmartDeviceRequestGoTargetPositionResponse(1, 4);//???: 1,Robot U/D: 4
						printf("response5\n");
					}

				}
				else{
					if(RobotGoDown(1, LaserRobotUp.TargetPosition)){
						UartPICSendErrorCode(BdcSystem.ChairName, 4, 0);
					}
					else{
						UartSmartDeviceRequestGoTargetPositionResponse(1, 4);//???: 1,Robot U/D: 4
						printf("response6\n");
					}
				}
				break;
			}
			#endif
			break;

		
		case UART_SMART_DEVICE_SEND_MODEL_NAME :
			BdcSystem.ModelName = Comm1.RxBuf[5];
			LaserChair1Up.HomePosition = 250;
			LaserChair2Up.HomePosition = 250;
			printf("Command 0x4A : Model Name: %d\n", BdcSystem.ModelName);
			UartSmartDeviceSendModelNameResponse(BdcSystem.ModelName);

			break;

		case UART_ESP_SEND_ROBOT_STOP :
			if(Comm1.RxBuf[5] == 5){//??????? 
				UartEspRequestRobotStopResponse(BdcSystem.ChairName);
			}
			break;

		case UART_SMART_DEVICE_REQUEST_CURRENT_POSITION:
			BdcSystem.ChairName = Comm1.RxBuf[5];
			printf("Command 0x49 : Which Chair: %d\n", BdcSystem.ChairName);
			UartSmartDeviceRequestCurrentPositionResponse(BdcSystem.ChairName);
			break;

		case UART_SMART_DEVICE_SEND_POSITION_RESPONSE:
			if(Comm1.RxBuf[5] == 0){//Response OK
				//UartPICSendLengthDataAfterManualAdjustment(u8 WhichOne)	
				printf("Manual adjustment response ok\n");
			}
			else{//Retry or Error
				UartPICSendErrorCode(Comm1.RxBuf[5], 8, 0);
			}
			break;

		default:
			break;
	}
}

void UartSmartDeviceRequestGoHomePositionResponse(u8 LR, u8 WhichOne, u16 Length)
{
	u8 Buf[50];
	u8 i=0, j=0,length=0;
	u16 checksum=0;

	length = 6;
	Buf[i++] = STX;
	Buf[i++] = 0;//length msb
	Buf[i++] = length;
	Buf[i++] = UART_SMART_DEVCIE_REQUEST_GO_HOME_POSITION;
	Buf[i++] = 0;//address
	Buf[i++] = LR;
	Buf[i++] = WhichOne;
	Buf[i++] =  (u8)((Length >> 8) & 0x00FF); 
	Buf[i++] = (u8)(Length & 0x00FF);

	for(j=3; j< (length+3); j++){
		checksum += Buf[j];
	}
	Buf[i++] = (u8)checksum;
	Buf[i] = ETX;

	for (j=0; j<=i;j++){
    		while (!LL_USART_IsActiveFlag_TXE(UART4));
    		LL_USART_TransmitData8(UART4, (uint8_t)Buf[j]);
	}
	while (!LL_USART_IsActiveFlag_TC(UART4));
	
	
	return;
}

void UartSmartDeviceRequestGoTargetPositionResponse(u8 Complete, u8 Dir)
{
	u8 Buf[50];
	u8 i=0, j=0,length=0;
	u16 checksum=0;

	length = 4;
	Buf[i++] = STX;
	Buf[i++] = 0;//length msb
	Buf[i++] = length;
	Buf[i++] = UART_SMART_DEVCIE_REQUEST_GO_TARGET_POSITION;
	Buf[i++] = 0;//address
	Buf[i++] =  Complete; 
	Buf[i++] = Dir;

	for(j=3; j< (length+3); j++){
		checksum += Buf[j];
	}
	Buf[i++] = (u8)checksum;
	Buf[i] = ETX;

	for (j=0; j<=i;j++){
    		while (!LL_USART_IsActiveFlag_TXE(UART4));
    		LL_USART_TransmitData8(UART4, (uint8_t)Buf[j]);
	}
	while (!LL_USART_IsActiveFlag_TC(UART4));
	return;
}

void UartSmartDeviceSendModelNameResponse(u8 ModelNo)
{
	u8 Buf[50];
	u8 i=0, j=0,length=0;
	u16 checksum=0;

	length = 4;
	Buf[i++] = STX;
	Buf[i++] = 0;//length msb
	Buf[i++] = length;
	Buf[i++] = UART_SMART_DEVICE_SEND_MODEL_NAME;
	Buf[i++] = 0;//address
	Buf[i++] =  ModelNo; 
	Buf[i++] = 0;

	for(j=3; j< (length+3); j++){
		checksum += Buf[j];
	}
	Buf[i++] = (u8)checksum;
	Buf[i] = ETX;

	for (j=0; j<=i;j++){
    		while (!LL_USART_IsActiveFlag_TXE(UART4));
    		LL_USART_TransmitData8(UART4, (uint8_t)Buf[j]);
	}
	while (!LL_USART_IsActiveFlag_TC(UART4));
	return;
}

void UartEspRequestRobotStopResponse(u8 WhichOne)
{
	u8 Buf[50];
	u8 i=0, j=0,length=0;
	u16 checksum=0;

	length = 9;
	Buf[i++] = STX;
	Buf[i++] = 0;//length msb
	Buf[i++] = length;
	
	Buf[i++] = UART_ESP_SEND_ROBOT_STOP_RESPONSE;
	Buf[i++] = 0;//address
	Buf[i++] = WhichOne;
	if(WhichOne == LEFT_CHAIR){
		Buf[i++] =  (u8)((LaserChair1Front.CurrentPosition >> 8) & 0x00FF); 
		Buf[i++] = (u8)(LaserChair1Front.CurrentPosition & 0x00FF);
		Buf[i++] =  (u8)((PotentioChair1Height.CurrentPosition >> 8) & 0x00FF); 
		Buf[i++] = (u8)(PotentioChair1Height.CurrentPosition & 0x00FF);
	}
	else if(WhichOne == RIGHT_CHAIR){
		Buf[i++] =  (u8)((LaserChair2Front.CurrentPosition >> 8) & 0x00FF); 
		Buf[i++] = (u8)(LaserChair2Front.CurrentPosition & 0x00FF);
		Buf[i++] =  (u8)((PotentioChair2Height.CurrentPosition >> 8) & 0x00FF); 
		Buf[i++] = (u8)(PotentioChair2Height.CurrentPosition & 0x00FF);
	}
	else if(WhichOne == HM_LEFT_CHAIR){
		Buf[i++] =  (u8)((LaserChair1Front.CurrentPosition >> 8) & 0x00FF); 
		Buf[i++] = (u8)(LaserChair1Front.CurrentPosition & 0x00FF);
		Buf[i++] =  (u8)((LaserChair1Up.CurrentPosition >> 8) & 0x00FF); 
		Buf[i++] = (u8)(LaserChair1Up.CurrentPosition & 0x00FF);
	}
	else if(WhichOne == HM_RIGHT_CHAIR){
		Buf[i++] =  (u8)((LaserChair2Front.CurrentPosition >> 8) & 0x00FF); 
		Buf[i++] = (u8)(LaserChair2Front.CurrentPosition & 0x00FF);
		Buf[i++] =  (u8)((LaserChair2Up.CurrentPosition >> 8) & 0x00FF); 
		Buf[i++] = (u8)(LaserChair2Up.CurrentPosition & 0x00FF);
	}
	Buf[i++] =  (u8)((LaserRobotUp.CurrentPosition >> 8) & 0x00FF); 
	Buf[i++] = (u8)(LaserRobotUp.CurrentPosition & 0x00FF);
	for(j=3; j< (length+3); j++){
		checksum += Buf[j];
	}
	Buf[i++] = (u8)checksum;
	Buf[i] = ETX;

	for (j=0; j<=i;j++){
    		while (!LL_USART_IsActiveFlag_TXE(UART4));
    		LL_USART_TransmitData8(UART4, (uint8_t)Buf[j]);
	}
	while (!LL_USART_IsActiveFlag_TC(UART4));
	
	return;


}


void UartSmartDeviceRequestCurrentPositionResponse(u8 WhichOne)
{
	u8 Buf[50];
	u8 i=0, j=0,length=0;
	u16 checksum=0;
	u16 diff[5];

	length = 9;
	Buf[i++] = STX;
	Buf[i++] = 0;//length msb
	Buf[i++] = length;
	
	//Buf[i++] = UART_SMART_DEVICE_REQUEST_CURRENT_POSITION;
	Buf[i++] = UART_SMART_DEVICE_REQUEST_CURRENT_POSITION;
	Buf[i++] = 0;//address
	Buf[i++] = WhichOne;
	if(LaserChair1Front.CurrentPosition > LaserChair1Front.HomePosition)
		diff[0] = LaserChair1Front.HomePosition - LaserChair1Front.CurrentPosition;
	else
		diff[0] = 0;
	if(WhichOne == LEFT_CHAIR){
		if(PotentioChair1Height.CurrentPosition > PotentioChair1Height.HomePosition)
			diff[1] = PotentioChair1Height.CurrentPosition - PotentioChair1Height.HomePosition;
		else
			diff[1] = 0;
	}
	else if(WhichOne == HM_LEFT_CHAIR){
		if(LaserChair1Up.CurrentPosition > LaserChair1Up.HomePosition)
			diff[1] = LaserChair1Up.CurrentPosition - LaserChair1Up.HomePosition;
		else
			diff[1] = 0;

	}
	
	if(LaserChair2Front.CurrentPosition > LaserChair2Front.HomePosition)
		diff[2] = LaserChair2Front.HomePosition - LaserChair2Front.CurrentPosition;
	else
		diff[2] = 0;
	if(WhichOne == RIGHT_CHAIR){
		if(PotentioChair2Height.CurrentPosition > PotentioChair2Height.HomePosition)
			diff[3] = PotentioChair2Height.CurrentPosition - PotentioChair2Height.HomePosition;
		else
			diff[3] = 0;
	}
	else if(WhichOne == HM_RIGHT_CHAIR){
		if(LaserChair2Up.CurrentPosition > LaserChair2Up.HomePosition)
			diff[3] = LaserChair2Up.CurrentPosition - LaserChair2Up.HomePosition;
		else
			diff[3] = 0;
	}
	if(LaserRobotUp.CurrentPosition > LaserRobotUp.HomePosition)
		diff[4] = LaserRobotUp.CurrentPosition - LaserRobotUp.HomePosition;
	else
		diff[4] = 0;
	
	if((WhichOne == LEFT_CHAIR) || (WhichOne == HM_LEFT_CHAIR)){
		Buf[i++] =  (u8)((diff[0] >> 8) & 0x00FF); 
		Buf[i++] = (u8)(diff[0] & 0x00FF);
		Buf[i++] =  (u8)((diff[1] >> 8) & 0x00FF); 
		Buf[i++] = (u8)(diff[1] & 0x00FF);
	}
	else if((WhichOne == RIGHT_CHAIR) || (WhichOne == HM_RIGHT_CHAIR)){
		Buf[i++] =  (u8)((diff[2] >> 8) & 0x00FF); 
		Buf[i++] = (u8)(diff[2] & 0x00FF);
		Buf[i++] =  (u8)((diff[3] >> 8) & 0x00FF); 
		Buf[i++] = (u8)(diff[3] & 0x00FF);
	}
	Buf[i++] =  (u8)((diff[4] >> 8) & 0x00FF); 
	Buf[i++] = (u8)(diff[4] & 0x00FF);
	for(j=3; j< (length+3); j++){
		checksum += Buf[j];
	}
	Buf[i++] = (u8)checksum;
	Buf[i] = ETX;

	for (j=0; j<=i;j++){
    		while (!LL_USART_IsActiveFlag_TXE(UART4));
    		LL_USART_TransmitData8(UART4, (uint8_t)Buf[j]);
	}
	while (!LL_USART_IsActiveFlag_TC(UART4));
	if((WhichOne == LEFT_CHAIR) || (WhichOne == RIGHT_CHAIR)){
		printf("Left Current Position: %d, %d\n", LaserChair1Front.CurrentPosition, PotentioChair1Height.CurrentPosition);
		printf("Right Current Position: %d, %d\n", LaserChair2Front.CurrentPosition, PotentioChair2Height.CurrentPosition);
	}
	else if((WhichOne == HM_LEFT_CHAIR) || (WhichOne == HM_RIGHT_CHAIR)){
		printf("HM Left Current Position: %d, %d\n", LaserChair1Front.CurrentPosition, LaserChair1Up.CurrentPosition);
		printf("HM Right Current Position: %d, %d\n", LaserChair2Front.CurrentPosition, LaserChair2Up.CurrentPosition);

	}
	printf("ROBOT Current Position: %d\n", LaserRobotUp.CurrentPosition);
	
	printf("Left diff Position: %d, %d, %d\n", diff[0], diff[1], diff[4]);
	printf("Right diff Position: %d, %d, %d\n", diff[2], diff[3], diff[4]);

	return;
}


void UartPICSendLengthDataAfterManualAdjustment(u8 WhichOne)
{
	u8 Buf[50];
	u8 i=0, j=0,length=0;
	u16 checksum=0;
	u16 diff[5];

	length = 9;
	Buf[i++] = STX;
	Buf[i++] = 0;//length msb
	Buf[i++] = length;
	
	Buf[i++] = UART_SMART_DEVICE_SEND_POSITION_RESPONSE;
	Buf[i++] = 0;//address
	Buf[i++] = WhichOne;
	if(LaserChair1Front.CurrentPosition < LaserChair1Front.HomePosition)
		diff[0] = LaserChair1Front.HomePosition - LaserChair1Front.CurrentPosition;
	else
		diff[0] = 0;

	if(PotentioChair1Height.CurrentPosition > PotentioChair1Height.HomePosition)
		diff[1] = PotentioChair1Height.CurrentPosition - PotentioChair1Height.HomePosition;
	else
		diff[1] = 0;
	if(LaserChair2Front.CurrentPosition < LaserChair2Front.HomePosition)
		diff[2] = LaserChair2Front.HomePosition - LaserChair2Front.CurrentPosition;
	else
		diff[2] = 0;

	if(PotentioChair2Height.CurrentPosition > PotentioChair2Height.HomePosition)
		diff[3] = PotentioChair2Height.CurrentPosition - PotentioChair2Height.HomePosition;
	else
		diff[3] = 0;
	if((WhichOne ==ROBOT) && (BdcSystem.ModelName == MODEL_HM)){
		if(LaserChair1Up.CurrentPosition > LaserChair1Up.HomePosition)
			diff[1] = LaserChair1Up.CurrentPosition - LaserChair1Up.HomePosition;
		else
			diff[1] = 0;

		if(LaserChair2Up.CurrentPosition > LaserChair2Up.HomePosition)
			diff[3] = LaserChair2Up.CurrentPosition - LaserChair2Up.HomePosition;
		else
			diff[3] = 0;
	}

	if(LaserRobotUp.CurrentPosition > LaserRobotUp.HomePosition)
		diff[4] = LaserRobotUp.CurrentPosition - LaserRobotUp.HomePosition;
	else
		diff[4] = 0;
	if((WhichOne == LEFT_CHAIR) ||(WhichOne == ROBOT)){
		Buf[i++] =  (u8)((diff[0] >> 8) & 0x00FF); 
		Buf[i++] = (u8)(diff[0] & 0x00FF);
		Buf[i++] =  (u8)((diff[1] >> 8) & 0x00FF); 
 		Buf[i++] = (u8)(diff[1] & 0x00FF);
 	}
 	else if(WhichOne == RIGHT_CHAIR){
 		Buf[i++] =  (u8)((diff[2] >> 8) & 0x00FF); 
		Buf[i++] = (u8)(diff[2] & 0x00FF);
		Buf[i++] =  (u8)((diff[3] >> 8) & 0x00FF); 
		Buf[i++] = (u8)(diff[3] & 0x00FF);
	}
	Buf[i++] =  (u8)((diff[4] >> 8) & 0x00FF); 
	Buf[i++] = (u8)(diff[4] & 0x00FF);
	
	for(j=3; j< (length+3); j++){
		checksum += Buf[j];
	}
	Buf[i++] = (u8)checksum;
	Buf[i] = ETX;

	for (j=0; j<=i;j++){
    		while (!LL_USART_IsActiveFlag_TXE(UART4));
    		LL_USART_TransmitData8(UART4, (uint8_t)Buf[j]);
	}
	while (!LL_USART_IsActiveFlag_TC(UART4));
	if((BdcSystem.ModelName == MODEL_HA) || (BdcSystem.ModelName == 0)){
		printf("Left Current Position: %d, %d, %d\n", LaserChair1Front.CurrentPosition, PotentioChair1Height.CurrentPosition, LaserRobotUp.CurrentPosition);
		printf("Right Current Position: %d, %d, %d\n", LaserChair2Front.CurrentPosition, PotentioChair2Height.CurrentPosition, LaserRobotUp.CurrentPosition);
	}
	else{
		printf("HM Left Current Position: %d, %d, %d\n", LaserChair1Front.CurrentPosition, LaserChair1Up.CurrentPosition, LaserRobotUp.CurrentPosition);
		printf("HM Right Current Position: %d, %d, %d\n", LaserChair2Front.CurrentPosition, LaserChair2Up.CurrentPosition, LaserRobotUp.CurrentPosition);

	}
	printf("Left diff Position: %d, %d, %d\n", diff[0], diff[1], diff[4]);
	printf("Right diff Position: %d, %d, %d\n", diff[2], diff[3], diff[4]);

	//return;
}
void UartPICSendErrorCode(u8 WhichOne, u8 ErrorCode1, u8 ErrorCode2)
{
	u8 Buf[50];
	u8 i=0, j=0,length=0;
	u16 checksum=0;

	length = 5;
	Buf[i++] = STX;
	Buf[i++] = 0;//length msb
	Buf[i++] = length;
	Buf[i++] = UART_PIC_SEND_ERROR_CODE;
	Buf[i++] = 0;//address
	Buf[i++] = LEFT_CHAIR;//
	Buf[i++] =  ErrorCode1; 
	Buf[i++] = ErrorCode2;

	for(j=3; j< (length+3); j++){
		checksum += Buf[j];
	}
	Buf[i++] = (u8)checksum;
	Buf[i] = ETX;

	for (j=0; j<=i;j++){
    		while (!LL_USART_IsActiveFlag_TXE(UART4));
    		LL_USART_TransmitData8(UART4, (uint8_t)Buf[j]);
	}
	while (!LL_USART_IsActiveFlag_TC(UART4));
	printf("ERR: %d, %d, %d\n", WhichOne, ErrorCode1, ErrorCode2);
}

void nop_op(void)
{
	delay();
}

void I2C_Start(void)
{
	I2C_SDA_LOW;
	nop_op();

	I2C_SCL_LOW;
	nop_op();

	I2C_SDA_HIGH;
	nop_op();

	I2C_SCL_HIGH;
	nop_op();

	I2C_SDA_LOW;
	nop_op();

	I2C_SCL_LOW;
	nop_op();
}

void I2C_Stop(void)
{
		I2C_SDA_LOW;
		nop_op();

		I2C_SCL_HIGH;
		nop_op();

		I2C_SDA_HIGH;
}

void I2C_Ack(void)
{
	I2C_SDA_LOW;
	I2C_SCL_HIGH;
	nop_op();
	I2C_SCL_LOW;
	I2C_SDA_LOW;
	nop_op();
}

void I2C_Write_Data(u8 data)
{
	u8 BitNo;
	u8 Wdata;

	Wdata=data;

	for(BitNo=0;BitNo<8;BitNo++)
	{
		if(Wdata & 0x80){
			I2C_SDA_HIGH;
			I2C_SCL_HIGH;
		}
		else{
			I2C_SDA_LOW;
			I2C_SCL_HIGH;
		}

		nop_op();
		Wdata=(Wdata<<1);
		I2C_SCL_LOW;
		nop_op();
	}
}

u8 I2C_Read_Data(void)
{
	u8 ReadData=0;
	u8 BitNo;

	I2C_SDA_HIGH;
	nop_op();
	I2C_SDA_in_Mode();
 	nop_op();
	//-----------------------

	for(BitNo=0;BitNo<8;BitNo++){
		I2C_SCL_HIGH;
		ReadData<<=1;

		nop_op();

		if(I2C_SDA_READ == 1) ReadData|=1;
		else ReadData&=0xfe;

		I2C_SCL_LOW;
		nop_op();
	}
	//-----------------------
	I2C_SDA_out_Mode();
	nop_op();
	//-----------------------

	return ReadData;
}

void EepromWrite(u16 Waddress, u8 WData)
{
		u16 HiAddress;
		u16 LowAddress;
		//u16 i;

		HiAddress=(Waddress & 0xff00)>>8;
		LowAddress=(Waddress & 0xff);
		//-------------------------------------------------
		I2C_Start();
		I2C_Write_Data(D_WControl);
		I2C_Ack();

		I2C_Write_Data((u8)HiAddress);	// HI ADDRESS
		I2C_Ack();

		I2C_Write_Data((u8)LowAddress);	// LOW ADDRESS
		I2C_Ack();
		//-------------------------------------------------
		I2C_Write_Data(WData);
		I2C_Ack();
		I2C_Stop();
		//--------------------------------------------------

		//- write 사이클 시간 max 5 msec !
//		for (i=0;i<2500;i++) ;
		//for (i=0;i<625;i++) ; //4Mhz : one opcode = 1us, 즉 625us
		delayMsec(5);
}

u8 EepromRead(u16 Raddress)
{
	u8 ReadData;
	u16 HiAddress;
	u16 LowAddress;

	HiAddress=(Raddress & 0xff00)>>8;
	LowAddress=(Raddress & 0xff);

	//--------------------------
	I2C_Start();
	I2C_Write_Data(D_WControl);
	I2C_Ack();

	I2C_Write_Data((u8)HiAddress);
	I2C_Ack();

	I2C_Write_Data((u8)LowAddress);
	I2C_Ack();
	//--------------------------

	I2C_Start();
	I2C_Write_Data(D_RControl);
	I2C_Ack();

	ReadData=I2C_Read_Data();
	I2C_Stop();
	//--------------------------

	return ReadData;
}

void EEPROM_Write(u16 Address, u8 Data)
{
		EepromWrite(Address, Data);
}

u8 EEPROM_Read(u16 Address)
{
		return EepromRead(Address);
}

void EEPROM_Save(u16 Address, u8 *Buffer, u16 Size)
{
		u16 i;

		for (i = 0; i < Size; i++)
				EEPROM_Write(Address + i, *(Buffer + i));
}
void EEPROM_Erase()
{
		u16 i;

		for (i = 0; i < 100; i++)
				EEPROM_Write(i, 0);
}

void EEPROM_Load(u16 Address, u8 *Buffer, u16 Size)
{
		u16 i;

		for (i = 0; i < Size; i++)
				*(Buffer + i) = EEPROM_Read(Address + i);
}


void I2C_SDA_in_Mode()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = I2C_SDA_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;

    LL_GPIO_Init(I2C_SDA_GPIO_Port, &GPIO_InitStruct);
}

void I2C_SCL_out_Mode()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = I2C_SCL_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;

    LL_GPIO_Init(I2C_SCL_GPIO_Port, &GPIO_InitStruct);
}

void I2C_SDA_out_Mode()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = I2C_SDA_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;

    LL_GPIO_Init(I2C_SDA_GPIO_Port, &GPIO_InitStruct);
}



PUTCHAR_PROTOTYPE
{
	while (!LL_USART_IsActiveFlag_TXE(USART2)) {}
	LL_USART_TransmitData8(USART2, (uint8_t)ch);
	while (!LL_USART_IsActiveFlag_TC(USART2)) {}

	return ch;

}



void delay()
{
  vu8 i=0xFF;

  while(i--)
  asm("nop");
}

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

void delayMsec(u32 delayTime)
{

  /* Enable the SysTick Counter */
  LL_SYSTICK_EnableIT();
  TimingDelay = delayTime;

  while(TimingDelay != 0);
  /* Disable SysTick Counter */
  LL_SYSTICK_DisableIT();
  /* Clear SysTick Counter */
  LL_SYSTICK_EnableIT();
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

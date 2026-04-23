# HealerBot ChairControl 인수인계 문서

> 작성일: 2026-04-20
> 대상: ChairControl3.1 (STM32F105 BDC 의자·로봇 제어 보드)

---

## 1. 시스템 개요

HealerBot ChairControl은 **Healerbot Chair System**의 의자·로봇 제어 보드이다. 좌측 의자(Chair1), 우측 의자(Chair2), Robot 유닛을 관리하며 총 5개의 BDC(Brushed DC) 모터를 제어하여 전후/상하 이동을 수행한다.

```
[Smart Device (APP / PIC 상위 컨트롤러)]
        │ UART  115200bps (STX 0xF1 / ETX 0xF4 패킷)
        ▼
[STM32F105VCTX]   ← ChairControl3.1 (본 프로젝트)
        │ DRV1/DRV2/DRV3 (GPIO Direction + nSLEEP)
        │ SPI 없음 — DRV 시리즈 모터 드라이버는 직접 GPIO 제어
        ▼
[5× BDC 모터]
 ├─ Motor1: Chair1 전/후 (Laser 거리센서 피드백)
 ├─ Motor2: Chair1 상/하 (Potentiometer 피드백)
 ├─ Motor3: Chair2 전/후 (Laser 거리센서 피드백)
 ├─ Motor4: Chair2 상/하 (Potentiometer 피드백)
 └─ Motor5: Robot 상/하 (Laser 거리센서 피드백)
```

**센서 구성:**
- **Laser 거리센서** 3개: Chair1 길이, Chair2 길이, Robot 높이 (ADC → voltage → mm 변환)
- **Potentiometer** 2개: Chair1 높이, Chair2 높이 (ADC → mm 변환)
- **Limit Switch** 7개: Chair1 FWD/BACK/DW, Chair2 FWD/BACK/DW, Robot UP/DW

---

## 2. 프로젝트 폴더 구조

| 폴더 | 용도 |
|------|------|
| `Core/Src/` | `main.c` (2777줄), `stm32f1xx_it.c` (ISR + 1ms ADC 평균), `system_stm32f1xx.c`, syscall 스텁 |
| `Core/Inc/` | `main.h` (핀 매핑, 매크로, 타입 정의 전체), `stm32f1xx_it.h` |
| `Core/Startup/` | STM32F105 벡터 테이블 어셈블리 (CubeIDE 생성) |
| `Drivers/STM32F1xx_HAL_Driver/` | STM32 LL (Low-Layer) 드라이버 라이브러리 |
| `Drivers/CMSIS/` | ARM Cortex-M3 CMSIS |
| `Debug/` | STM32CubeIDE 디버그 빌드 산출물 (`.elf/.bin/.hex/.list/.map`) |
| `Release/` | STM32CubeIDE 릴리즈 빌드 산출물 |
| `SI/` | Source Insight 프로젝트 파일 (코드 탐색용) |
| `docs/superpowers/` | 설계문서(specs)·구현계획(plans) |
| `ChairControl3.ioc` | STM32CubeMX 설정 (핀 할당 / 클럭 / 페리페럴) |
| `STM32F105VCTX_FLASH.ld` | GNU LD 링커 스크립트 |

---

## 3. 빌드 방법

### STM32CubeIDE (GCC `arm-none-eabi`)

| 항목 | 내용 |
|------|------|
| IDE | STM32CubeIDE |
| 프로젝트 루트 | `ChairControl3.ioc` 또는 `.project` |
| 빌드 | `Project → Build All` (Debug / Release 각 configuration 존재) |
| Debug 출력 | `Debug/ChairControl3.elf` (+ `.bin`, `.hex`, `.list`, `.map`) |
| Release 출력 | `Release/ChairControl3.elf` (+ `.list`, `.map`) |
| 링커 스크립트 | `STM32F105VCTX_FLASH.ld` (FLASH 256KB / SRAM 64KB) |
| 명령줄 빌드 | `cd Debug && make all` (또는 `Release/`) |
| 클린 | `make clean` |
| 툴체인 | `GNU Tools for STM32 (13.3.rel1)` (`arm-none-eabi-gcc`) |

### 펌웨어 플래싱

- STM32CubeIDE 내부 디버거 (`Run → Debug`) — ST-LINK 사용
- 또는 `Debug/ChairControl3.bin` / `.hex` 를 ST-LINK Utility·STM32CubeProgrammer로 직접 쓰기
- `Debug/` 폴더에 `ChairControl3_2026.1.5.binMX` 등 버전 태그된 릴리즈 바이너리도 보관됨

---

## 4. 하드웨어 구성 (MCU / 클럭 / 페리페럴)

| 항목 | 내용 |
|------|------|
| MCU | **STM32F105VCTX** (Cortex-M3, 100-LQFP) |
| 시스템 클럭 | **72 MHz** (HSE → PLLx9) |
| Flash Latency | 2 wait states |
| APB1 prescaler | /2 (36 MHz) |
| APB2 prescaler | /4 (18 MHz) |
| ADC clock source | PCLK2 / 2 |
| 드라이버 프레임워크 | STM32 **LL (Low-Layer)** — HAL 미사용 |
| 사용 페리페럴 | ADC1(10ch, DMA1_Ch1), UART4(DMA2_Ch3 Rx), USART2(printf Tx), TIM7(1ms), GPIO 전체 |

---

## 5. 보드 리비전 분기 (`BOARD_B`)

`main.h:164`의 `#define BOARD_B`로 하드웨어 리비전을 전환한다. **주석 처리 = V3.0, 활성 = V3.1.**  
영향 범위:

| 영역 | V3.0 (`BOARD_B` 주석) | V3.1 (`BOARD_B` 활성, 현재 기본) |
|------|-----------------------|-----------------------------------|
| I2C (EEPROM) SCL/SDA | GPIOB PB11/PB12 | GPIOE PE15/PE14 |
| SW_CHAIR2 FWD/BACK/UP | GPIOE PE14/PE15, GPIOB PB10 | GPIOA PA11/PA10/PA9 |
| SW_CHAIR2_DW | GPIOB PB13 | GPIOA PA8 |
| SW_ROBOT UP/DW | GPIOA PA9/PA8 | GPIOC PC9/PC8 |
| SW_CHAIR1_DW | GPIOA PA11 | GPIOC PC12 |

다른 컴파일 플래그 (`main.h:261~`):
- `MOTOR_MULTI_MOVE` — 주석 처리 (다중 모터 동시 구동 테스트용)
- `HW_TEST` — 주석 처리 (자동 왕복 HW 수명시험 모드)
- `RMC_BLACK` — 주석 처리 (리모컨 블랙 버전: SW_CHAIR_DW를 BACK+UP AND 조합으로 대체)

---

## 6. 핀 매핑 (주요)

### 모터 드라이버 (DRV1 / DRV2 / DRV3)

DRV1 / DRV2 / DRV3는 각각 H-브리지 2채널 또는 1채널 포함하여 **총 5 모터**를 구동.

| 드라이버 | 모터 | IN1/IN3 (Forward) | IN2/IN4 (Reverse) | nSLEEP | nFAULT |
|---------|------|--------------------|--------------------|--------|--------|
| DRV1 ch1 | Motor1 (Chair1 FWD/BACK) | PE0 (DRV1_IN1) | PE1 (DRV1_IN2) | PE4 | PE5 |
| DRV1 ch2 | Motor3 (Chair2 FWD/BACK) | PE2 (DRV1_IN3) | PE3 (DRV1_IN4) | PE4 | PE5 |
| DRV2 ch1 | Motor4 (Chair2 UP/DW) | PC13 (DRV2_IN1) | PC14 (DRV2_IN2) | PB0 | PB2 |
| DRV2 ch2 | Motor2 (Chair1 UP/DW) | PE7 (DRV2_IN3) | PE8 (DRV2_IN4) | PB0 | PB2 |
| DRV3 | Motor5 (Robot UP/DW) | PD9 (DRV3_IN1) | PD10 (DRV3_IN2) | PD8 | PD11 |

> **주의**: 실제 Forward/Reverse 방향 제어 매크로는 `DRV_CHAIR1_FWD_HIGH/LOW` 등이며 보드 배선상의 이유로 Chair1은 DRV1_IN1/IN2, **Chair2는 DRV1_IN3/IN4**를 쓴다. Up/Down도 Chair1은 DRV2_IN3/IN4, Chair2는 DRV2_IN1/IN2로 **교차 배선**되어 있음에 유의.

### 모터 전류 보호 / 센서 (ADC1, 10ch, DMA1_Ch1)

| Rank | ADC 채널 | GPIO | 용도 | 배열 index |
|------|---------|------|------|-----------|
| 1 | CH10 | PC0 | DRV1 IPROP1 (Motor1 전류) | `internal_adc_value[0]` |
| 2 | CH11 | PC1 | DRV1 IPROP2 (Motor3 전류) | `internal_adc_value[1]` |
| 3 | CH12 | PC2 | DRV2 IPROP1 | `internal_adc_value[2]` |
| 4 | CH13 | PC3 | DRV2 IPROP2 | `internal_adc_value[3]` |
| 5 | CH14 | PC4 | Chair2 Height Potentiometer | `internal_adc_value[4]` |
| 6 | CH0 | PA0 | DRV3 IPROP1 (Motor5 전류) | `internal_adc_value[5]` |
| 7 | CH4 | PA4 | Chair1 Laser Length 센서 | `internal_adc_value[6]` |
| 8 | CH5 | PA5 | Chair2 Laser Length 센서 | `internal_adc_value[7]` |
| 9 | CH6 | PA6 | Robot Laser Height 센서 | `internal_adc_value[8]` |
| 10 | CH7 | PA7 | Chair1 Height Potentiometer | `internal_adc_value[9]` |

ADC는 **연속 변환 + 스캔 + DMA Circular** 모드로 구동 (`ADC1_Init_LL()` in [main.c](Core/Src/main.c))

### 리밋 스위치 (Active LOW, Pull-Up)

| 스위치 | GPIO | 매크로 |
|-------|------|-------|
| LMT_CHAIR1_FRT | PB5 | `LIMIT_CHAIR1_FRONT_CHECK` |
| LMT_CHAIR1_BACK | PD5 | `LIMIT_CHAIR1_BACK_CHECK` |
| LMT_CHAIR1_DW | PD3 | `LIMIT_CHAIR1_DW_CHECK` |
| LMT_CHAIR2_FRT | PE10 | `LIMIT_CHAIR2_FRONT_CHECK` |
| LMT_CHAIR2_BACK | PE11 | `LIMIT_CHAIR2_BACK_CHECK` |
| LMT_CHAIR2_DW | PE13 | `LIMIT_CHAIR2_DW_CHECK` |
| LMT_ROBOT_UP | PC7 | `LIMIT_ROBOT_UP_CHECK` |
| LMT_ROBOT_DW | PC6 | `LIMIT_ROBOT_DW_CHECK` |

### 조작 스위치 (Active HIGH, Pull-Down) — `BOARD_B` 기준

| 스위치 | GPIO | 매크로 |
|-------|------|-------|
| SW_CHAIR1_FWD | PD2 | `SW_CHAIR1_FWD_CHECK` |
| SW_CHAIR1_BACK | PD1 | `SW_CHAIR1_BACK_CHECK` |
| SW_CHAIR1_UP | PD0 | `SW_CHAIR1_UP_CHECK` |
| SW_CHAIR1_DW | PC12 | `SW_CHAIR1_DW_CHECK` |
| SW_CHAIR1_OK | PA10 | (정의만, 현재 미사용) |
| SW_CHAIR2_FWD | PA11 | `SW_CHAIR2_FWD_CHECK` |
| SW_CHAIR2_BACK | PA10 | `SW_CHAIR2_BACK_CHECK` |
| SW_CHAIR2_UP | PA9 | `SW_CHAIR2_UP_CHECK` |
| SW_CHAIR2_DW | PA8 | `SW_CHAIR2_DW_CHECK` |
| SW_ROBOT_UP | PC9 | `SW_ROBOT_UP_CHECK` |
| SW_ROBOT_DW | PC8 | `SW_ROBOT_DW_CHECK` |

### 기타

| 신호 | GPIO | 설명 |
|------|------|------|
| CpuRun (Status LED) | PB14 | TIM7에서 500ms On/Off 하트비트 |
| UART4 TX / RX | PC10 / PC11 | 상위 장치(APP·PIC) 통신 (**115200**, DMA2_Ch3 Rx) |
| USART2 TX / RX | PA2 / PA3 | `printf` 디버그 출력 (115200) |
| I2C SCL / SDA (bit-bang) | PE15 / PE14 (BOARD_B) | EEPROM (D_WControl=0xA0 / D_RControl=0xA1) |

---

## 7. 플래시 메모리 레이아웃

```
0x08000000 ──────────────────────────
           │  Application (256KB)   │  ← 본 프로젝트의 유일한 영역 (부트로더 없음)
0x0803FFFF ──────────────────────────
```

현재 ChairControl3.1 보드에는 **별도의 부트로더 없이 애플리케이션만 단독** 탑재된다. (참고: QoL 보드는 부트로더 + 앱 분할 구조)

### EEPROM 레이아웃 (외장 I2C EEPROM, bit-bang)

| 주소 | 내용 | 정의 |
|------|------|------|
| 0 | FW Version byte 1 | `EEPROM_ADDR_FW_VERSION1` |
| 1 | FW Version byte 2 | `EEPROM_ADDR_FW_VERSION2` |
| 2~3 | Left Chair BACK Home Position (MSB/LSB) | `EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_*` |
| 4~5 | Left Chair HEIGHT Home Position | `EEPROM_ADDR_LEFT_CHAIR_HEIGHT_HOME_POSITION_*` |
| 6~7 | Right Chair BACK Home Position | `EEPROM_ADDR_RIGHT_CHAIR_BACK_HOME_POSITION_*` |
| 8~9 | Right Chair HEIGHT Home Position | `EEPROM_ADDR_RIGHT_CHAIR_HEIGHT_HOME_POSITION_*` |
| 10~11 | Robot HEIGHT Home Position | `EEPROM_ADDR_ROBOT_HEIGHT_HOME_POSITION_*` |

- 각 HomePosition은 **u16 big-endian** 으로 저장
- 초기값 `0xFF` (미초기화 플래시) 감지 시 `main()` 기동 때 전부 0으로 초기화한다 ([main.c:180](Core/Src/main.c#L180))

---

## 8. 통신 프로토콜

### 패킷 프레임 (STM32 ↔ 상위 장치, UART4, 115200 bps)

```
[STX=0xF1] [LEN_H] [LEN_L] [CMD] [ADDR] [DATA...] [CHECKSUM] [ETX=0xF4]
```

각 바이트의 역할:

| 위치 | 이름 | 값 | 설명 |
|------|------|----|------|
| [0] | STX | `0xF1` | 패킷 시작 마커 |
| [1] | LEN_H | `0x00` (보통) | 데이터 길이 상위 바이트 |
| [2] | LEN_L | 가변 | 데이터 길이 하위 바이트 (CMD+ADDR+DATA 바이트 수만 포함) |
| [3] | CMD | 가변 | 명령 코드 |
| [4] | ADDR | `0x00` (보통) | 주소 필드 (예약) |
| [5..] | DATA | 가변 | 명령 파라미터 |
| [-2] | CHECKSUM | 계산값 | `(CMD + ADDR + DATA...) & 0xFF` 하위 8비트 |
| [-1] | ETX | `0xF4` | 패킷 끝 마커 |

**체크섬**: `CMD + ADDR + DATA...` 를 단순 누산한 후 하위 8비트만 취함 (u8 오버플로).

**전체 패킷 길이** = `LEN_L + 5` (`STX + LEN_H + LEN_L + CHECKSUM + ETX` 5바이트 제외 기준).

### 주요 CMD 코드 (`main.h:323~332`)

| CMD | 방향 | 기능 |
|-----|------|------|
| `0x45` | APP→STM | 홈포지션 이동(설정) 요청 — `UART_SMART_DEVCIE_REQUEST_GO_HOME_POSITION` |
| `0x46` | APP→STM | 타겟 위치 이동 요청 — `UART_SMART_DEVCIE_REQUEST_GO_TARGET_POSITION` |
| `0x47` | STM→APP | 로봇 정지 응답 — `UART_ESP_SEND_ROBOT_STOP_RESPONSE` |
| `0x48` | APP→STM / STM→APP | 수동 조정 후 위치 전송 응답 — `UART_SMART_DEVICE_SEND_POSITION_RESPONSE` |
| `0x49` | APP→STM | 현재 위치 요청 — `UART_SMART_DEVICE_REQUEST_CURRENT_POSITION` |
| `0x4A` | APP→STM | 모델 정보 설정 — `UART_SMART_DEVICE_SEND_MODEL_NAME` |
| `0x4E` | STM→APP | 에러 코드 전송 — `UART_PIC_SEND_ERROR_CODE` |
| `0x75` | APP→STM | 로봇 정지 명령 — `UART_ESP_SEND_ROBOT_STOP` |

### CMD 필드 상세

#### 0x45 (홈포지션 이동 요청/설정)
```
Request:  F1 00 04 45 00 [ChairName] [MotorName] [CS] F4
Response: F1 00 06 45 00 [LR] [WhichOne] [LenMSB] [LenLSB] [CS] F4
```
- `ChairName`: `0x01`=LEFT_CHAIR, `0x02`=RIGHT_CHAIR, `0x04`=ROBOT, `0x10`=HM_LEFT_CHAIR, `0x20`=HM_RIGHT_CHAIR
- `MotorName`: `0x01`=CHAIR_FRONT_BACK, `0x02`=CHAIR_UP_DOWN, `0x04`=ROBOT_UP_DOWN

#### 0x46 (타겟 위치 이동)
```
Request: F1 00 09 46 00 [ChairName] [FrntBackMSB] [FrntBackLSB] [UpDwMSB] [UpDwLSB] [RobotMSB] [RobotLSB] [CS] F4
```
3축 동시 목표값 전송 (의자 전후 / 의자 상하 / 로봇 상하). 수식:
- `의자 전후 실제 타겟 = HomePosition − Offset` (레이저 센서 방향 때문에 감산)
- `의자 상하 실제 타겟 = Offset + HomePosition`
- `로봇 상하 실제 타겟 = Offset + HomePosition`

#### 0x49 (현재 위치 조회 응답)
Home 위치 대비 **차이(delta)** 를 u16 big-endian으로 돌려준다. Chair1/Chair2/Robot 각축의 diff가 아니라 `CurrentPosition - HomePosition` (음수는 0으로 클램프).

---

## 9. 메인 로직 구조

### `main()` — 기동 흐름 ([main.c:89](Core/Src/main.c#L89))

```
1. SystemClock_Config()   72MHz HSE+PLL
2. MX_GPIO_Init()         모든 GPIO 초기화
3. MX_USART2_UART_Init()  printf 디버그
4. MX_UART4_Init()        상위 통신 (DMA+IDLE)
5. MX_TIM7_Init()         1ms 타이머
6. ADC1_Init_LL()         ADC+DMA
7. EEPROM → HomePosition 복구
8. SW_ROBOT_UP == HIGH이면 → SPMode() (유지보수 모드)
9. while(1): MasterComm1(), UartResponseProcess(), 수동 스위치 폴링
```

### 메인 루프 ([main.c:270~371](Core/Src/main.c#L270))

무한 루프 내부:
- `MasterComm1()` — UART 패킷 파싱 (플래그 `Comm1.Flag==1`이면 처리)
- `UartResponseProcess()` — 각 모터 `MoveState` 폴링해서 응답 송출
- **수동 스위치 체크** — SW_CHAIR1_FWD / BACK / UP / DW, SW_CHAIR2_*, SW_ROBOT_* 모두 폴링해서 해당 방향 모터 구동 함수 호출

> 참고: **일부 스위치는 상호 잠금**. `SW_CHAIR1_BACK && SW_CHAIR2_UP` 동시 눌림 시 BACK은 무시 (SW_CHAIR2_UP 우선). 약 10ms 디바운스 후 재확인.

### 1kHz TIM7 ISR ([stm32f1xx_it.c:348~587](Core/Src/stm32f1xx_it.c#L348))

매 1ms 다음을 수행:
1. **MCU LED 하트비트** — 500ms ON / 500ms OFF (`CpuRun_Pin`)
2. **Chair1 Length ADC**: 10샘플 이동평균 → voltage(mV) → mm  
   `mm = (28200 / (voltage - 190)) * 1000 / 129` (레이저 센서 비선형 보정식)
3. **Chair2 Length ADC**: 동일 공식, `internal_adc_value[7]`
4. **Robot Height ADC**: **5샘플 미디안 필터** (버블 정렬 후 [2] 중앙값) — Chair는 단순 평균이지만 Robot만 노이즈 대응을 위해 미디안 사용
5. **Chair1/Chair2 Potentiometer**: 10샘플 평균 → `position = adc/2.5` (72~172mm 범위)
6. **BDCMotor[1..5].MotorTimer** 카운트다운 — 0되면 타임아웃 판정

### UART4 RX ISR ([stm32f1xx_it.c:247~343](Core/Src/stm32f1xx_it.c#L247))

DMA(Circular) + IDLE 인터럽트 조합:
1. IDLE 플래그 발생 → DMA write 포인터 계산
2. Wrap-around 고려해서 `Comm1.RxBuf`로 memcpy
3. STX(0xF1) 확인 → LEN_L 확인 → ETX(0xF4) 위치 확인
4. 완전한 패킷이면 `Comm1.Flag = 1` → 메인 루프가 `MasterComm1()`에서 처리

> **왜 DMA + IDLE?**: USB-UART 어댑터가 분할 전송할 수 있어 RXNE 단일 바이트 방식으로는 드롭 가능. 여러 IDLE에 걸쳐 누적 조립하도록 설계됨.

---

## 10. 주요 함수 목록 (`main.c`)

### 모터 구동 함수 (블로킹, 스위치 폴링 루프)

| 함수 | 기능 |
|------|------|
| `LeftChairGoFront(Auto, TargetPos)` | Motor1 전진. Auto=1이면 Laser 거리 < TargetPos까지, 0이면 수동 SW 해제까지 |
| `LeftChairGoBack(Auto, TargetPos)` | Motor1 후진 |
| `LeftChairGoUp(Auto, TargetPos)` | Motor2 상승 (Potentiometer 피드백) |
| `LeftChairGoDown(Auto, TargetPos)` | Motor2 하강 + **안정화 감지** (TargetPos=0일 때 3초 동안 위치 변화 없으면 홈 도달 판정) |
| `RightChairGoFront(Auto, TargetPos)` | Motor3 |
| `RightChairGoBack(Auto, TargetPos)` | Motor3 |
| `RightChairGoUp(Auto, TargetPos)` | Motor4 |
| `RightChairGoDown(Auto, TargetPos)` | Motor4 + 안정화 감지 |
| `RobotGoUp(Auto, TargetPos)` | Motor5 상승 (Laser Height 피드백) |
| `RobotGoDown(Auto, TargetPos)` | Motor5 하강 |

모든 함수 공통:
- 리턴값 `0` = 정상 완료, `1` = 타임아웃 (`time_over=1`)
- 이동 시작 시 `BDCMotor[N].MotorTimer`에 제한시간 세팅 (FWD_BACK=16000ms, UP_DOWN=24000ms, ROBOT=27000ms)
- `g_is_homing` 플래그 확인 — **홈잉 중이 아닐 때**는 기동 전 리밋 스위치 체크하여 이미 리밋이면 즉시 return

### 통신 함수

| 함수 | 기능 |
|------|------|
| `MasterComm1()` | UART4 수신 플래그 체크 → 체크섬 확인 → `UartProcess()` 호출 (메인 루프에서 호출) |
| `UartProcess()` | CMD 스위치 분기 (0x45/0x46/0x47/0x49/0x4A 등) — 1490줄 대규모 함수 |
| `UartResponseProcess()` | 5개 모터의 `MoveState`를 폴링하여 MOVE_OK / MOVE_LIMIT / MOVE_TIME_OVER 별 UART 응답 송출 |
| `UartSmartDeviceRequestGoHomePositionResponse(LR, WhichOne, Length)` | 0x45 응답 송출 |
| `UartSmartDeviceRequestGoTargetPositionResponse(Complete, Dir)` | 0x46 중간/완료 응답 |
| `UartEspRequestRobotStopResponse(WhichOne)` | 0x47 (9바이트 DATA: 3축 현재위치) |
| `UartSmartDeviceRequestCurrentPositionResponse(WhichOne)` | 0x49 — Home 대비 diff 값 송출 |
| `UartPICSendLengthDataAfterManualAdjustment(WhichOne)` | 0x48 — 수동 조작 후 PIC에 위치 전송 |
| `UartSmartDeviceSendModelNameResponse(ModelNo)` | 0x4A 응답 |
| `UartPICSendErrorCode(WhichOne, ErrorCode1, ErrorCode2)` | 0x4E — 에러 코드 전송 |

### 센서·저장·유틸

| 함수 | 기능 |
|------|------|
| `ADC1_Init_LL()` | ADC1 10채널 스캔 + DMA Circular 모드 설정 |
| `SPMode()` | 부팅 시 SW_ROBOT_UP 눌림 시 진입. 모든 스위치/센서/리밋 상태를 printf로 연속 출력. 리셋 전까지 유지 |
| `EepromRead(Raddress)` / `EepromWrite(Waddress, WData)` | I2C bit-bang EEPROM 읽기/쓰기 |
| `EEPROM_Read` / `EEPROM_Write` | 위 래퍼 (`main.c:2632~`) |
| `EEPROM_Save(Address, Buffer, Size)` / `EEPROM_Load(...)` | 블록 I/O |
| `EEPROM_Erase()` | 앞 100 바이트 0으로 삭제 |
| `I2C_Start/Stop/Ack/Write_Data/Read_Data` | bit-bang I2C 트랜잭션 |
| `I2C_SDA_in_Mode` / `I2C_SDA_out_Mode` / `I2C_SCL_out_Mode` | SDA/SCL 방향 전환 (오픈드레인) |
| `delayMsec(delayTime)` | SysTick 기반 블로킹 지연 |
| `TimingDelay_Decrement()` | SysTick_Handler에서 호출되는 1ms 감산 |
| `Error_Handler()` | 치명 오류 시 IRQ 비활성 + 무한 루프 |

---

## 11. 주요 자료구조 (`main.h`)

### `Comm_InitTypeDef` — UART 수신 상태
```c
u8  RxBuf[30];              // 조립된 패킷
u8  TxBuf[30];
u8  DMA_Rx[64];             // DMA Circular 버퍼
u32 DMA_RxHead;             // 이전 DMA 포인터 위치
u16 RxCount;                // RxBuf 누적 바이트
u8  Flag;                   // 1=패킷 수신 완료, MasterComm1()이 처리
u8  AckCheck, Retry;
u16 Timer, CommandDataLength;
```

### `BDCMotor_InitTypeDef` — 개별 모터 상태
```c
u16 AnalogCurrent;          // IPROP ADC 값
u16 MotorTimer;             // 남은 제한시간 (ms, TIM7에서 감산)
u8  MoveState;              // 0=idle, 1=MOVE_OK, 2=MOVE_LIMIT, 3=MOVE_TIME_OVER
u8  Direction;              // INCREASE=0, DECREASE=1
```

### `LaserSensor_InitTypeDef` — 레이저 센서 기반 축
```c
u32 FindPosition, voltage;
u16 TargetPosition, CurrentPosition, HomePosition, PrePosition;
u8  HomePosi[2];            // EEPROM u16 저장용 MSB/LSB
u8  Length, Leng[2];
```
사용: `LaserChair1Front`, `LaserChair1Up`, `LaserChair2Front`, `LaserChair2Up`, `LaserRobotUp`

### `Potentiometer_InitTypeDef` — 의자 높이용
`LaserSensor_InitTypeDef`와 유사하나 추가로:
```c
u16 HomeCheckTimer;         // 안정화 대기 타이머 (3000ms)
u8  HomeCheckFlag;          // 안정화 감지 상태
```
사용: `PotentioChair1Height`, `PotentioChair2Height`

### `ADC_Healerbot_InitTypeDef` — ADC 10채널 통합
5개의 모터 전류(IPROP), 3개의 레이저 거리, 2개의 Potentiometer 집계

### `AVERAGE_InitTypeDef` — 이동평균 필터 (10샘플)
사용: `Chair1LengthAvg`, `Chair2LengthAvg`, `Chair1HeightAvg`, `Chair2HeightAvg`, `RobotHeightAvg`

### `BdcSystem_InitTypeDef` — 시스템 전역 상태
```c
u16 Timer, SPMode;
u8  ChairName;              // LEFT_CHAIR / RIGHT_CHAIR / ROBOT / HM_*
u8  MotorName;              // CHAIR_FRONT_BACK / CHAIR_UP_DOWN / ROBOT_UP_DOWN
u8  ModelName;              // MODEL_HA=1, MODEL_HM=2
```

### `HWTest_InitTypeDef` — HW 수명시험 카운터 (`HW_TEST` 빌드 시)
Minute/Second 누적, 각 모터별 왕복 횟수 및 방향 전환 딜레이 타이머

---

## 12. 상수 및 특수 값

### 이동 제한 시간
- `FWD_BACK_LIMIT_TIME` = **16000 ms** (≈ 14초 실측)
- `UP_DOWN_LIMIT_TIME` = **24000 ms** (≈ 21초 실측)
- `ROBOT_UP_DOWN_LIMIT_TIME` = **27000 ms** (≈ 23초 실측)

### 거리센서 변환식 (레이저)
```c
voltage_mV = (ADC * 3300) >> 12;
mm = (u16)( (28200 / (voltage - 190)) * 1000 / 129 );
```
(`main.c:392~`, `:410~`, `:443~`)

### Potentiometer 변환
`mm = (float)(ADC>>2) / 2.5`  → 72~172mm 범위 ([stm32f1xx_it.c:456](Core/Src/stm32f1xx_it.c#L456))

### Home Position 안정화 (의자 높이만)
`LeftChairGoDown` / `RightChairGoDown`이 `Auto=1, TargetPos=0`으로 호출될 때:
- 현재위치가 이전위치와 동일하면 `HomeCheckTimer=3000` 시작
- 3초간 값 변화 없으면 안정화 완료 → 현재 위치를 HomePosition으로 저장

### SENS_MARGIN, PRE_STOP
현재 모두 **0**. 목표 위치 근방 여유값 (도달 판정 마진, 정지 전 감속 여유). 튜닝 여지 존재.

### Potentiometer 홈 기본 위치
- `POTENTIO_CHAIR1_HOME_POSITION` = `78+3` = 81 mm
- `POTENTIO_CHAIR1_MAX_POSITION` = 168 mm
- Chair2 동일

---

## 13. 동작 모드

### 일반 모드 (부팅 시 SW_ROBOT_UP이 눌려있지 않을 때)
메인 루프에서:
- UART 명령 처리 (APP/PIC로부터 받은 명령 실행)
- 수동 스위치 폴링 (물리 버튼으로 의자/로봇 직접 조작)
- 조작 후 자동으로 `UartPICSendLengthDataAfterManualAdjustment()`로 위치 전송

### SP Mode (부팅 시 SW_ROBOT_UP이 눌린 채 전원 ON)
`BdcSystem.SPMode = 1`, `SPMode()` 진입. 리셋 전까지 영구 루프:
- 100ms 주기로 모든 스위치/리밋/센서 상태를 printf(USART2)로 출력
- 서비스·점검·캘리브레이션 확인용
- 빠져나가려면 **전원 리셋 필수**

### HW_TEST 모드 (`#define HW_TEST` 활성화 빌드)
자동 왕복 수명 시험:
- 전원 ON 직후 리밋 상태에 따라 모터 기동
- TIM7이 리밋 도달 감지 → 딜레이 후 반대 방향 전환
- 각 모터별 왕복 카운트를 MM/SS 타임스탬프와 함께 printf 출력
- 별도 종료 조건 없음 — 수동 리셋

---

## 14. 현재 진행/최근 변경사항

**2026-04-16 업데이트** (`docs/superpowers/specs/2026-04-10-homing-sensor-origin-design.md`):
- **홈포지션 방식 변경**: 기존 고정 좌표(의자 전방 75, 로봇 하강 90)로 이동 → 리밋 포토센서 감지점까지 이동
- **타겟 좌표식 반전**: `Target = Home + Offset` → `Target = Home − Offset` (의자 전후방축)
- 로봇 축은 기존 덧셈 유지 (레이저 방향 때문)
- `LeftChairGoBack(1, 65535)`, `RobotGoDown(1, 0)` — 큰 더미 타겟을 넘기고 센서만이 정지 조건
- Robot Height ADC 평균 → **5샘플 미디안** 필터로 교체 (노이즈 개선)
- 의자 높이는 3초 안정화 감지 방식 (`HomeCheckTimer`) 도입

---

## 15. 개발 환경 / 디버깅

### 툴
- **STM32CubeIDE** (주 IDE, CubeMX 통합)
- **STM32CubeMX** — `.ioc` 파일 편집 (핀맵 / 클럭 변경 후 자동 코드 재생성)
- **Source Insight** — 대용량 `main.c` 네비게이션용 (`SI/` 폴더)
- **ST-LINK Utility** or **STM32CubeProgrammer** — 수동 플래싱

### 디버그 출력 (USART2, 115200 bps)
- PA2 TX / PA3 RX 핀에 USB-UART 어댑터 연결
- 모든 상태 변화·에러·위치값이 `printf`로 출력됨
- `__io_putchar()`가 USART2로 리다이렉트 ([main.c:2705](Core/Src/main.c#L2705))

### UART4 (APP 통신) 스니핑
- PC10 TX / PC11 RX (115200 bps)
- STX `F1` / ETX `F4` 단위로 분리하여 CMD 분석

---

## 16. 참고 문서 (`docs/superpowers/`)

| 파일 | 내용 |
|------|------|
| `specs/2026-04-10-homing-sensor-origin-design.md` | 센서 기반 홈포지션 방식 설계서 (핀맵 분기·필터링 포함) |
| `plans/2026-04-10-homing-sensor-origin.md` | 위 설계의 구현 계획 / 변경 위치 리스트 |

---

## 17. 알려진 특이사항 / 주의점

1. **모터 드라이버 교차 배선** — Chair1/Chair2가 DRV1·DRV2 내에서 채널 교차 (IN1/IN2 vs IN3/IN4). 매크로 (`DRV_CHAIRn_FWD_HIGH` 등) 사용하고 핀 번호를 직접 참조하지 말 것.
2. **UART4 수신은 반드시 DMA+IDLE 방식** — RXNE 방식 주석 처리됨 (`main.c:220`). 분할 전송 드롭 방지 목적.
3. **DRV3_IN3/IN4 핀 존재하지 않음** — Robot은 단일 채널 (UP/DW만). `main.c:165~166`의 DRV2_IN3/IN4 ResetOutputPin에 GPIOC가 적혀있으나 실제 DRV2 Chair UP/DW는 GPIOE. **이 코드는 잘못된 참조로 보임** — 검증 필요.
4. **I2C bit-bang의 Ack 처리 생략** — `I2C_Ack()`가 SDA=LOW로 강제하고 읽기 없음. Nack 감지 안 됨 → EEPROM 고장 시 대응 불가.
5. **while(LL_USART_IsActiveFlag_TXE(UART4))** 방식 블로킹 송신 — 수십 바이트 송신 중 다른 처리 멈춤. 고속 통신 필요 시 DMA Tx 검토 필요.
6. **인터럽트 우선순위**: NVIC Group3 기반, TIM7=0 (최우선), UART4=1, USART2=3. UART 처리 중 TIM7 ADC 샘플링은 선점.
7. **MOTOR_MULTI_MOVE** / **HW_TEST** / **RMC_BLACK** 매크로는 기본 비활성. 특정 릴리즈/시험 용도이므로 일반 빌드에서 건드리지 말 것.
8. **Home Position 0xFF 초기화 판정**은 Robot MSB 바이트 한 군데만 검사 — 다른 축이 깨져있고 Robot MSB만 멀쩡하면 놓칠 수 있음.

# ChairControl3.1 모듈 분해 리팩토링 설계서

> 작성일: 2026-04-23
> 대상: `Core/Src/main.c` (2,889줄), `Core/Inc/main.h` (655줄), `Core/Src/stm32f1xx_it.c` (591줄)
> 선행 자료: `HANDOVER.md`

---

## 1. 목표와 범위

### 1.1 목표

모놀리식 `main.c`·`main.h`·`stm32f1xx_it.c`를 **책임별 도메인 모듈 7개 + 핀맵 헤더 1개 + umbrella `main.h`** 구조로 분해하여 **유지보수성·가독성**을 확보한다.

### 1.2 범위 (In-Scope)

- 파일 분리와 헤더 분해
- 전역 변수의 소유 모듈 확정
- ISR 파일의 도메인 tick/handler 함수 위임 경계 확정
- 함수 `static` 범위 재평가 (모듈 내부 전용은 `static` 격리)

### 1.3 범위 밖 (Out-of-Scope)

- 기능 추가·동작 변경 (behavior-preserving 리팩토링)
- OCP 추상화 (함수 포인터·vtable 등)
- BSP/HAL 추상화 레이어 도입
- 서브폴더 구조 (예: `motor/`, `comm/`) — 플랫 파일 구성 유지
- 도메인 내부의 캡슐화(`static` + getter/setter) 강화
- `BOARD_B` 보드 분기 리팩 (분기 매크로는 현 위치 유지)
- `HANDOVER.md` §17에 기록된 기존 이슈(`DRV3_IN3/IN4` 오참조, I2C Ack 미검사, UART4 송신 블로킹, EEPROM 0xFF 부분검사 등) **수정 금지** — 발견 시 worklog에 "추후 검토"로 기록만 함
- `MOTOR_MULTI_MOVE`·`HW_TEST`·`RMC_BLACK` 빌드 플래그 기본 상태(비활성) 변경 금지

### 1.4 성공 기준

- 빌드 성공 (에러 0, warning 개수 증가 없음)
- 베이스라인 대비 모든 기능 동등 (§5 검증 절차 전량 통과)
- `main.c` ≤ ~250줄, 각 `hb_*.c` ≤ ~600줄
- `.bin` 크기 ±1% 이내 변동
- SP 모드 printf 출력이 베이스라인 대비 수치 상 ±1mm/±허용오차 이내

---

## 2. 아키텍처

### 2.1 레이어 / 의존성

```
Level 0 (최하):
  pinmap.h                    — 모든 GPIO 매크로. 의존성 없음.

Level 1 (독립 도메인):
  hb_storage.{c,h}            — I2C bit-bang + EEPROM_*
  hb_mode.{c,h}               — SPMode + HW_TEST

Level 2 (센서/액추에이터):
  hb_sensor.{c,h}             — Laser/Potentio 타입 + 변환식 + 이동평균/미디안
  hb_motor.{c,h}              — BDCMotor 타입 + 10개 모터 함수 + g_is_homing

Level 3 (프로토콜):
  hb_comm.{c,h}               — Comm1 + MasterComm1 + UartProcess + 응답들

Level 4 (초기화):
  hb_system.{c,h}             — SystemClock_Config + MX_*_Init + ADC1_Init_LL + 시스템 타이머/delay

Top:
  main.c (~250줄)             — main() + 메인 루프 스위치 폴링
  stm32f1xx_it.c (~200줄)     — ISR 진입점만, 본체는 도메인 함수 위임
  main.h                      — umbrella: 위 헤더들 include + BdcSystem 전역 + 빌드 플래그
```

### 2.2 include 규칙

- **기존 `.c`의 `#include "main.h"` 경로 변경하지 않는다.** `main.h`가 umbrella 역할로 모든 도메인 헤더를 포함하므로 기존 include 체인이 그대로 유효.
- 새 도메인 `.c`는 자기 모듈 헤더 + 필요한 타 모듈 헤더만 명시 include 가능.
- **도메인 `.h` 간 cross-include 금지**. 공유 타입은 `main.h`를 경유 (ISP 준수 + include cycle 방지).

### 2.3 `main.h` 최종 형태

```c
#ifndef __MAIN_H
#define __MAIN_H
#ifdef __cplusplus
extern "C" {
#endif

/* 빌드 플래그 (현 위치 유지, 범위 밖) */
#define BOARD_B
// #define MOTOR_MULTI_MOVE
// #define HW_TEST
// #define RMC_BLACK

#include "stm32f1xx.h"
/* (기존 LL 헤더 include 유지) */

/* 도메인 umbrella */
#include "pinmap.h"
#include "hb_system.h"
#include "hb_sensor.h"
#include "hb_motor.h"
#include "hb_comm.h"
#include "hb_storage.h"
#include "hb_mode.h"

/* 다수 모듈 공유 타입·전역 */
typedef struct { u16 Timer, SPMode; u8 ChairName, MotorName, ModelName; } BdcSystem_InitTypeDef;
extern BdcSystem_InitTypeDef BdcSystem;

#ifdef __cplusplus
}
#endif
#endif
```

> `BdcSystem_InitTypeDef`의 필드는 현 `main.h` 정의 그대로 복사(여기서는 예시만 요약). `u8`·`u16` 등 typedef가 별도 파일에 있다면 해당 include도 유지.

---

## 3. 전역 변수 소유권

### 3.1 매핑

| 전역 | 현재 위치 | 이동 위치 | 비고 |
|------|----------|---------|------|
| `BdcSystem` | `main.c:31` | **`main.c` 유지** | 여러 모듈 공유, umbrella `main.h`에서 `extern` |
| `BDCMotor1..BDCMotor5` | `main.c:34` | `hb_motor.c` | |
| `Chair1, Chair2, Robot` (MotorCtrl) | `main.c:39` | `hb_motor.c` | |
| `g_is_homing` | `main.c:66` | `hb_motor.c` | `volatile` 유지 |
| `ADConverter` | `main.c:35` | `hb_sensor.c` | |
| `LaserChair1Up..LaserRobotUp` (5개) | `main.c:40` | `hb_sensor.c` | |
| `PotentioChair1Height, PotentioChair2Height` | `main.c:41` | `hb_sensor.c` | |
| `Chair1LengthAvg, Chair2LengthAvg, Chair1HeightAvg, Chair2HeightAvg, RobotHeightAvg` | `main.c:36` | `hb_sensor.c` | |
| `internal_adc_value[10]` | `main.c:64` | `hb_sensor.c` | DMA 타겟. `hb_system`의 ADC init이 `extern`으로 참조 |
| `HWTest` | `main.c:46` | `hb_mode.c` | |
| `TimingDelay` (`static vu32`) | `main.c:63` | `hb_system.c` | **`static` 유지** (파일 내부 전용) |
| `Comm1` | `stm32f1xx_it.c:33` | `hb_comm.c` | |
| `Timer7Counter` | `stm32f1xx_it.c:55` | `hb_system.c` | |
| `systick_counter` | `stm32f1xx_it.c:54` | `hb_system.c` | |
| `Timer6Counter` | `stm32f1xx_it.c:57` | `hb_system.c` | 원본 보존(미사용처럼 보이나 건드리지 않음) |

### 3.2 규칙

1. 각 전역의 **정의(storage)** 는 소유 모듈 `.c`에, **`extern` 선언** 은 소유 모듈 `.h`에 둔다.
2. 기존 `.c`에 흩어진 `extern` 선언(예: `main.c:43`, `stm32f1xx_it.c:34~39` 등)은 **전부 제거**하고 헤더의 `extern` 선언 경유로 일원화한다.
3. 타입·이름·초기값은 **100% 보존**. 선언 순서는 가능한 한 유지.
4. `static` 키워드는 그대로 유지. 도메인 간 공개가 필요하면 **별도 항목으로 명시** (§4 참조).

---

## 4. ISR 경계

### 4.1 `stm32f1xx_it.c` 최종 구조 (~200줄 목표)

| ISR | 처리 | 위임 대상 |
|-----|------|----------|
| `NMI_Handler`, `HardFault_Handler`, `MemManage_Handler`, `BusFault_Handler`, `UsageFault_Handler`, `SVC_Handler`, `DebugMon_Handler`, `PendSV_Handler` | **변경 없음** | — |
| `SysTick_Handler` (main.c의 `TimingDelay_Decrement` + `systick_counter++`) | 본체 이동 | `hb_system_on_systick()` |
| `ADC1_2_IRQHandler` | 본체 이동 | `hb_system_on_adc_isr()` |
| `USART2_IRQHandler` | 본체 이동 | `hb_comm_on_usart2_isr()` |
| `UART4_IRQHandler` (247~343, ~96줄 DMA+IDLE 패킷 조립) | **통째 이동** | `hb_comm_on_uart4_isr()` (내부 로직 쪼개지 않음) |
| `TIM7_IRQHandler` (348~587, ~239줄) | **4개 함수 호출로 분할** (아래 §4.2) | 다중 |

### 4.2 TIM7 1ms hot-path 분할

원본 실행 순서를 그대로 유지:

```c
void TIM7_IRQHandler(void) {
    if (LL_TIM_IsActiveFlag_UPDATE(TIM7)) {
        LL_TIM_ClearFlag_UPDATE(TIM7);
        hb_system_heartbeat_1ms();   /* ① MCU LED 500ms on/off */
        hb_sensor_tick_1ms();         /* ② Laser×3 + Potentio×2 평균/미디안/mm 변환 */
        hb_motor_tick_1ms();          /* ③ BDCMotor1..5 MotorTimer 감산 */
        hb_mode_tick_1ms();            /* ④ HW_TEST 카운터 (빌드 플래그 비활성 시 빈 본체) */
        Timer7Counter++;              /* ⑤ 전역 1ms 카운터 */
    }
}
```

### 4.3 오버헤드 영향

- 4회 추가 함수 호출 ≈ 30~40 cycle @ 72MHz ≈ **0.5 μs** (1ms tick의 0.05%)
- `-O2` 인라인 가능성 존재
- **중요 변경 체크포인트** (§7 커밋 4·6): 오실로스코프로 TIM7 ISR 지속시간 측정해 before/after 비교

---

## 5. 공개 API

### 5.1 `pinmap.h`

- 모든 GPIO `*_Pin` `#define` (현 `main.h`에서 추출)
- 매크로 헬퍼: `DRV_CHAIR1_FWD_HIGH/LOW`, `LIMIT_*_CHECK`, `SW_*_CHECK` 등
- `BOARD_B` 분기 `#ifdef`는 **이 헤더 내부에 잔존** (`main.h`가 `#define BOARD_B` 후 include)

### 5.2 `hb_system.h`

```c
/* 초기화 (main()에서 순차 호출) */
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
void MX_UART4_Init(void);
void MX_TIM7_Init(void);
void ADC1_Init_LL(void);

/* 유틸 */
void delayMsec(u32 delayTime);
void Error_Handler(void);

/* ISR 위임 */
void hb_system_on_systick(void);
void hb_system_on_adc_isr(void);
void hb_system_heartbeat_1ms(void);

extern u32 Timer7Counter;
extern u32 Timer6Counter;
extern u32 systick_counter;
```

> 원본 `static void MX_*_Init()`의 `static` 키워드는 **제거**하여 non-static으로 공개. 링크 시 동일 심볼, scope만 TU→전역 확장. 동작 영향 없음.

### 5.3 `hb_motor.h`

```c
typedef struct { /* 기존 정의 그대로 */ } BDCMotor_InitTypeDef;
typedef struct { /* 기존 정의 그대로 */ } MotorCtrl_InitTypeDef;

u8 LeftChairGoFront (u8 Auto, u16 TargetPos);
u8 LeftChairGoBack  (u8 Auto, u16 TargetPos);
u8 LeftChairGoUp    (u8 Auto, u16 TargetPos);
u8 LeftChairGoDown  (u8 Auto, u16 TargetPos);
u8 RightChairGoFront(u8 Auto, u16 TargetPos);
u8 RightChairGoBack (u8 Auto, u16 TargetPos);
u8 RightChairGoUp   (u8 Auto, u16 TargetPos);
u8 RightChairGoDown (u8 Auto, u16 TargetPos);
u8 RobotGoUp        (u8 Auto, u16 TargetPos);
u8 RobotGoDown      (u8 Auto, u16 TargetPos);

void hb_motor_tick_1ms(void);   /* BDCMotor1..5 MotorTimer 감산 */

extern BDCMotor_InitTypeDef BDCMotor1, BDCMotor2, BDCMotor3, BDCMotor4, BDCMotor5;
extern MotorCtrl_InitTypeDef Chair1, Chair2, Robot;
extern volatile u8 g_is_homing;
```

### 5.4 `hb_sensor.h`

```c
typedef struct { /* 기존 정의 그대로 */ } ADC_Healerbot_InitTypeDef;
typedef struct { /* 기존 정의 그대로 */ } LaserSensor_InitTypeDef;
typedef struct { /* 기존 정의 그대로 */ } Potentiometer_InitTypeDef;
typedef struct { /* 기존 정의 그대로 */ } AVERAGE_InitTypeDef;

void hb_sensor_tick_1ms(void);  /* ADC 평균/미디안 + mm 변환 */

extern u16 internal_adc_value[10];
extern ADC_Healerbot_InitTypeDef ADConverter;
extern LaserSensor_InitTypeDef LaserChair1Up, LaserChair1Front,
                               LaserChair2Up, LaserChair2Front, LaserRobotUp;
extern Potentiometer_InitTypeDef PotentioChair1Height, PotentioChair2Height;
extern AVERAGE_InitTypeDef Chair1LengthAvg, Chair2LengthAvg,
                           Chair1HeightAvg, Chair2HeightAvg, RobotHeightAvg;
```

### 5.5 `hb_comm.h`

```c
typedef struct { /* 기존 정의 그대로 */ } Comm_InitTypeDef;

/* 메인 루프에서 호출 */
void MasterComm1(void);
void UartResponseProcess(void);

/* 모터·모드 모듈이 호출하는 응답 송출 */
void UartSmartDeviceRequestGoHomePositionResponse(u8 LR, u8 WhichOne, u16 Length);
void UartSmartDeviceRequestGoTargetPositionResponse(u8 Complete, u8 Dir);
void UartSmartDeviceSendModelNameResponse(u8 ModelNo);
void UartEspRequestRobotStopResponse(u8 WhichOne);
void UartSmartDeviceRequestCurrentPositionResponse(u8 WhichOne);
void UartPICSendLengthDataAfterManualAdjustment(u8 WhichOne);
void UartPICSendErrorCode(u8 WhichOne, u8 ErrorCode1, u8 ErrorCode2);

/* ISR 위임 */
void hb_comm_on_uart4_isr(void);
void hb_comm_on_usart2_isr(void);

extern Comm_InitTypeDef Comm1;
```

> `UartProcess()` (1490줄)는 `MasterComm1()`만 호출 확인(main.c:1650). **`hb_comm.c` 내부 `static`으로 격리**. 동시에 현 `main.h:513`의 `void UartProcess(void);` prototype도 **제거** (더 이상 외부 노출 안 함).

### 5.6 `hb_storage.h`

```c
void EEPROM_Write(u16 Address, u8 Data);
u8   EEPROM_Read (u16 Address);
void EEPROM_Save (u16 Address, u8 *Buffer, u16 Size);
void EEPROM_Load (u16 Address, u8 *Buffer, u16 Size);
void EEPROM_Erase(void);
```

> `hb_storage.c` 내부 `static`으로 격리할 함수 (외부 호출처 확인 완료 — I2C 계열 내부에서만 사용):
> - `I2C_Start`, `I2C_Stop`, `I2C_Ack`, `I2C_Write_Data`, `I2C_Read_Data`
> - `I2C_SDA_in_Mode`, `I2C_SDA_out_Mode`, `I2C_SCL_out_Mode`
> - `EepromRead(u16)`, `EepromWrite(u16, u8)` (raw 래퍼 아래층)
> - `nop_op(void)` (I2C bit-bang 타이밍 지연, 2579~2676 범위에서만 호출됨)
> - `delay(void)` (I2C 내부 지연 함수, line 2829)

### 5.7 `hb_mode.h`

```c
typedef struct { /* 기존 정의 그대로 */ } HWTest_InitTypeDef;

void SPMode(void);              /* main()에서 조건부 호출 */
void hb_mode_tick_1ms(void);    /* HW_TEST 카운터 (플래그 비활성 시 빈 본체) */

extern HWTest_InitTypeDef HWTest;
```

### 5.8 API 설계 공통 원칙

1. **함수 시그니처 100% 보존** — 리턴타입·인자·이름·순서 불변.
2. **`static` 격리 기준** — 한 도메인 내부에서만 호출되는 함수는 `static`. 타 모듈/ISR이 호출하면 public.
3. **타입은 소유 모듈 헤더에** 정의. 다른 모듈은 umbrella `main.h`를 통해 간접 접근.
4. **`extern` 선언 중복 금지** — 각 전역은 소유 헤더에 한 번만.

---

## 6. 검증·롤백

### 6.1 베이스라인 캡처 (리팩토링 직전, 1회)

- `Debug/` 디렉터리 그대로 백업 → `baseline/Debug_YYYYMMDD/`
- SP 모드 부팅(전원 ON + SW_ROBOT_UP 누름) 후 USART2 printf 30초 캡처 → `baseline/sp_mode_YYYYMMDD.log`
- 수동 기능 스모크 기록:
  - Chair1 FWD/BACK/UP/DW, Chair2 동일, Robot UP/DW
  - UART 0x45/0x46/0x47/0x48/0x49/0x4A/0x4E/0x75 각 왕복 로그
  - EEPROM Home Position 재부팅 후 유지 확인

### 6.2 매 커밋 공통 검증

| 단계 | 항목 | 판정 |
|------|------|------|
| ① | Debug 빌드 | 에러 0, warning 개수 증가 없음 |
| ② | `.bin` 크기 diff | ±1% 이내 |
| ③ | `.map` 심볼 스폿 체크 | 주요 함수·전역 심볼 존재 |
| ④ | 부팅 | CpuRun LED 하트비트 정상 |
| ⑤ | SP 모드 | printf 출력 정상 |
| ⑥ | 해당 모듈 기능 | §6.3 체크리스트 |
| ⑦ | 회귀 스모크 | 변경 없던 영역 원 기능 유지 |

### 6.3 커밋별 기능 체크

| 커밋 | 위험도 | 중점 검증 |
|------|-------|----------|
| 1. pinmap.h | 🟢 | 빌드 성공 = 검증 완료 |
| 2. hb_storage | 🟢 | 재부팅 후 Home EEPROM 유지. `EEPROM_Erase` 후 재기록 왕복 |
| 3. hb_mode | 🟢 | SP 모드 진입/이탈. HW_TEST 플래그 off 유지 |
| 4. hb_sensor | 🟡 | **SP 모드 로그 정량 비교** Laser×3 + Pot×2 ±1mm. 오실로 TIM7 ISR 시간 ±10% |
| 5. hb_motor | 🟡 | 10개 함수 전부 실기: 수동·리밋·타임아웃. `g_is_homing` 경로 확인 |
| 6. hb_comm | 🔴 | UART 8개 CMD 왕복. `UartProcess` static 격리 후에도 호출 경로 동일. 오실로 UART4 프레임 |
| 7. hb_system | 🟡 | 72MHz 클럭(LED 주기로 간접), ADC DMA 순환, Timer7Counter 증가 |
| 8. 정리 | — | §6.1 베이스라인 절차 전량 재수행 |

### 6.4 이상 신호 (즉시 중단·롤백)

- `.bin` 크기 ±5% 이상 변동
- SP 모드 Laser/Pot 값이 베이스라인 대비 ±2mm 이상
- 부팅 후 3초 내 CpuRun LED 미시작
- 스위치 이벤트 로그 누락
- UART 명령 응답 누락/지연(>100ms)

### 6.5 버전 관리

`git init` 로컬 저장소 (커밋 1 직전 수행). `.gitignore`:

```gitignore
Debug/**/*.o
Debug/**/*.d
Debug/**/*.su
Debug/*.list
Debug/*.map
Release/**/*.o
Release/**/*.d
Release/**/*.su
Release/*.list
Release/*.map
*.bak
baseline/
```

> `.bin`·`.elf`·`.hex`는 **추적**(버전별 바이너리 비교용). 커밋 1 직전에 현재 `Debug/*.bin` 등을 최초 커밋으로 고정해 베이스라인 삼는다.

커밋 메시지 규칙:

```
refactor(pinmap): extract pin macros from main.h
refactor(storage): extract I2C/EEPROM from main.c
refactor(mode): extract SPMode and HW_TEST
refactor(sensor): extract Laser/Potentio conversion and filters
refactor(motor): extract 10 motor drive functions
refactor(comm): extract UART frame/process/responses
refactor(system): extract clock/GPIO/peripheral init
chore(main): finalize main.c cleanup
```

### 6.6 롤백 절차

```
문제 발견 → git reset --hard HEAD~1
         → 재플래싱으로 복귀 확인
         → 원인 분석 후 재작업 커밋
```

---

## 7. 추출 순서 (커밋 1~8)

| # | 모듈 | 예상 라인 규모 | 위험도 | 주요 동반 이동 |
|---|------|-------------|-------|--------------|
| 1 | `pinmap.h` | -200 / +205 | 🟢 | 핀 `#define`·매크로 헬퍼 |
| 2 | `hb_storage.{c,h}` | ~150 | 🟢 | I2C 8함수 + EEPROM 5함수 |
| 3 | `hb_mode.{c,h}` | ~100 | 🟢 | `SPMode()`, `HWTest` 전역 |
| 4 | `hb_sensor.{c,h}` | ~250 | 🟡 | 타입 4종 + 전역 13종 + `hb_sensor_tick_1ms` (TIM7 ISR 본체 일부) |
| 5 | `hb_motor.{c,h}` | ~600 | 🟡 | 10 함수 + 전역 9종 + `hb_motor_tick_1ms` |
| 6 | `hb_comm.{c,h}` | ~1500 | 🔴 | `MasterComm1`, `UartProcess`(static), `UartResponse*`(7종), `Comm1`, UART4/USART2 ISR 본체 |
| 7 | `hb_system.{c,h}` | ~400 | 🟡 | `SystemClock_Config`, `MX_*_Init`(4종), `ADC1_Init_LL`, `delayMsec`, `TimingDelay*`, 시스템 타이머 3종 |
| 8 | 정리 | — | 🟢 | `main.c` 최종 ~250줄 확인. 커밋 1~7 과정에서 헤더 이전으로 **중복·불필요해진 `extern` 선언**과 **죽은 주석** 제거만 수행. 함수 본체·동작 로직은 건드리지 않음 |

각 커밋은 **§6.2·§6.3 검증 통과 후에만 다음 단계로 진행**.

---

## 8. Action Items (본 설계 승인 직후)

1. `git init` 실행 + `.gitignore` 생성 + 현재 상태(`Core/`, `Debug/`, `docs/` 포함) 초기 커밋
2. 베이스라인 캡처 (§6.1)
3. 구현 계획(writing-plans) 작성 — 커밋 단위별 파일 변경 목록·grep 재확인 체크리스트 포함
4. 커밋 1 (pinmap.h) 착수

---

## 9. 결정 사항 요약 (§1~§5 승인)

| 항목 | 결정 |
|------|------|
| 목표 | B. 유지보수성·가독성만 (동작 보존) |
| 세분성 | 옵션 ① Coarse 7-file split |
| 서브폴더 | 미사용 (플랫 구성) |
| 타입 정의 | 도메인 헤더로 분산, umbrella 경유 |
| umbrella `main.h` | 유지 — 기존 include 체인 보존 |
| `static` 격리 | `UartProcess` 외 다수 함수 (§5 참조) |
| ISR 경계 | `stm32f1xx_it.c`는 진입점만, TIM7 4분할 위임 |
| 검증 수준 | 옵션 D (스모크 + SP 모드 로그 + 바이너리 크기·map 감시) |
| 버전 관리 | `git init` 로컬 저장소 |
| 추출 순서 | A (저위험 → 고위험 bottom-up) |

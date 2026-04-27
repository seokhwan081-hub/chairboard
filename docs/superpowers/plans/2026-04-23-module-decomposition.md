# ChairControl3.1 모듈 분해 리팩토링 구현 플랜

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 모놀리식 `main.c`(2,889줄) / `main.h`(655줄) / `stm32f1xx_it.c`(591줄)를 책임별 도메인 모듈 7개 + 핀맵 헤더 1개 + umbrella `main.h` 구조로 분해하여 유지보수성·가독성을 확보한다. 동작(behavior)은 100% 보존.

**Architecture:** 옵션 ① Coarse 7-file split. 순서 A bottom-up(저위험 → 고위험). 베이스라인 대비 기능·센서값·타이밍 동등성을 매 커밋마다 실기 검증. 완전한 설계는 `docs/superpowers/specs/2026-04-23-module-decomposition-design.md` 참조.

**Tech Stack:** STM32 embedded C (LL 드라이버), STM32CubeIDE (GCC arm-none-eabi), USART2 printf 디버그, UART4 패킷 통신, git local.

---

## Prerequisites

### 환경 체크 (플랜 착수 전 1회)

- [ ] **git 저장소 확인**

  ```bash
  cd /c/PR25/Healerbot/Sourcecode/ChairControl3_1
  git log --oneline -1
  ```
  Expected: `59db561 chore: baseline snapshot before module decomposition refactor`

- [ ] **베이스라인 산출물 존재 확인**

  ```bash
  ls Debug/ChairControl3.{bin,elf,hex,map}
  ```
  Expected: 4개 파일 모두 존재.

- [ ] **베이스라인 Debug 빌드 크기 기록**

  ```bash
  stat -c '%n %s' Debug/ChairControl3.bin Debug/ChairControl3.elf
  ```
  이 값을 `worklog.md` 2026-04-23 섹션에 기록. 이후 커밋마다 비교 기준.

---

## Pre-Work: 베이스라인 기능 캡처

### Task 0: SP 모드 로그 + 기능 스모크 (사용자 수행)

**Files:**
- Create: `baseline/sp_mode_baseline.log`
- Create: `baseline/smoke_test_baseline.md`

이 작업은 **실기 하드웨어가 필요**하며 사용자가 수동으로 수행한다.

- [ ] **Step 1: SP 모드 로그 캡처**

  1. USB-UART 어댑터를 USART2(PA2/PA3)에 연결, 터미널 115200bps 설정
  2. `SW_ROBOT_UP` 버튼 누른 채 전원 인가 → SP 모드 진입
  3. 터미널 로그 30초 수집 → `baseline/sp_mode_baseline.log` 저장
  4. 로그에 Laser×3(Chair1/Chair2/Robot), Pot×2(Chair1/Chair2 Height) 정지값이 포함되는지 확인

- [ ] **Step 2: 수동 기능 스모크**

  다음 항목을 실기로 수행하고 `baseline/smoke_test_baseline.md`에 pass/fail 기록:
  - Chair1 FWD / BACK / UP / DW 수동 버튼 각 동작
  - Chair2 FWD / BACK / UP / DW 수동 버튼 각 동작
  - Robot UP / DW 수동 버튼 각 동작
  - 리밋 스위치 도달 시 자동 정지 확인 (각 축)

- [ ] **Step 3: UART 명령 왕복**

  상위 장치 에뮬레이터 또는 UART4 스니퍼로 다음 명령 각 1회 왕복:
  - `0x45` (홈포지션 이동) — Chair1/Chair2/Robot
  - `0x46` (타겟 위치 이동)
  - `0x49` (현재 위치 조회)
  - `0x4A` (모델 정보 설정)

  각 명령의 응답 패킷 원본을 `baseline/smoke_test_baseline.md`에 기록.

- [ ] **Step 4: EEPROM 지속성**

  1. 수동으로 Chair1/Chair2 Back 홈 위치, Height 홈 위치, Robot 홈 위치를 설정
  2. 전원 OFF → ON
  3. 부팅 시 USART2 printf로 저장된 홈 위치 값이 재로드되는지 확인
  4. `baseline/smoke_test_baseline.md`에 홈 위치 값 4쌍 기록

- [ ] **Step 5: 베이스라인 커밋**

  ```bash
  cd /c/PR25/Healerbot/Sourcecode/ChairControl3_1
  # baseline/ 폴더는 .gitignore에 포함되어 있으므로 추적 안 됨
  # 대신 요약 파일만 추가로 커밋
  git add worklog.md
  git commit -m "docs: record baseline measurements before module decomposition"
  ```

---

## Conventions Used Throughout

- **파일 생성 위치:**
  - 새 헤더 파일 → `Core/Inc/`
  - 새 소스 파일 → `Core/Src/`
- **헤더 가드 형식:** `#ifndef HB_<MODULE>_H` / `#define HB_<MODULE>_H` / `#endif`
- **파일 상단 주석 블록:** ST 기본 주석 블록 복사 (`@file`, `@brief`, Copyright). 각 모듈의 `@brief`는 해당 모듈 책임 한 줄로.
- **`#include "main.h"`** 은 새 `.c` 파일마다 상단에 포함 (기존 관례 유지, umbrella로 LL 드라이버·공유 타입 접근).
- **함수/전역 이동 시:** 원본에서 **코드 블록을 그대로 복사**한 후 원본에서 제거. 리턴타입·인자명·줄바꿈 위치까지 보존.
- **`extern` 선언은 소유 모듈 헤더에 한 번만.** 타 `.c`에 흩어진 `extern`은 이동 시 반드시 제거.
- **빌드 검증:** STM32CubeIDE에서 Debug configuration → Build All. 에러 0 + warning 개수 베이스라인 이상 증가 없음.
- **실기 검증은 사용자가 수행**하고 결과를 대화로 전달받는 방식. 빌드 + grep 확인은 작업자가 수행.
- **커밋 메시지 접두:** `refactor(<module>):` 형식. 하드코딩된 본문은 각 Task에서 제공.

---

### Task 1: `pinmap.h` 추출 — GPIO 핀 매크로 분리

**Files:**
- Create: `Core/Inc/pinmap.h`
- Modify: `Core/Inc/main.h:117-245` (삭제), `Core/Inc/main.h` 최상단에 `#include "pinmap.h"` 추가

**위험도:** 🟢 최저 — 순수 매크로 이동, 컴파일 통과 = 검증 완료.

- [ ] **Step 1: 이동 범위 확정**

  `main.h:116~245` 사이의 GPIO 핀 `#define` 구간을 이동. 이 범위에는 BOARD_B 조건부 분기(`#ifdef BOARD_B ... #else ... #endif`)가 3곳 포함되어 있다:
  - `main.h:166~180` (SW_CHAIR2_FWD/BACK/UP)
  - `main.h:181~195` (I2C_SCL/SDA, SW_CHAIR2_DW)
  - `main.h:210~220` (SW_ROBOT_DW/UP)
  - `main.h:223~229` (SW_CHAIR1_DW)

  `#define BOARD_B` (`main.h:164`)는 **pinmap.h로 이동하지 않는다** — main.h에 잔존. pinmap.h는 include될 시점에 BOARD_B가 이미 정의되어 있다고 가정한다.

- [ ] **Step 2: `Core/Inc/pinmap.h` 생성**

  ```c
  /**
    ******************************************************************************
    * @file    pinmap.h
    * @brief   GPIO pin and port macros (extracted from main.h).
    *          BOARD_B must be defined (or not) before including this file.
    ******************************************************************************
    */
  #ifndef HB_PINMAP_H
  #define HB_PINMAP_H

  #ifdef __cplusplus
  extern "C" {
  #endif

  #include "stm32f1xx_ll_gpio.h"

  /* (여기에 main.h:117~245 내용 그대로 붙여넣기) */

  #ifdef __cplusplus
  }
  #endif
  #endif /* HB_PINMAP_H */
  ```

  구체적 붙여넣기 대상: `main.h:117`의 `#define DRV1_IN3_Pin LL_GPIO_PIN_2` 부터 `main.h:245`의 `#define DRV1_IN2_GPIO_Port GPIOE`까지 연속 구간. BOARD_B 조건부 분기 3곳 구조 그대로 유지.

  > 주의: `main.h:246~257`의 `#ifndef NVIC_PRIORITYGROUP_0 ... #endif` 블록은 GPIO 핀이 아니므로 **이동하지 않는다** — main.h에 남긴다.

- [ ] **Step 3: `main.h`에서 이동한 블록 삭제**

  `main.h:117~245` 구간 전체를 삭제. 삭제 후 `main.h:246`의 `#ifndef NVIC_PRIORITYGROUP_0`이 이전 `main.h:116`(Private defines 주석 줄) 바로 다음에 오도록 한다.

- [ ] **Step 4: `main.h` 최상단에 umbrella include 추가**

  `main.h`의 "Private includes" 섹션(현 `main.h:47`의 `/* USER CODE BEGIN Includes */` 직후)에 다음 한 줄 추가:

  ```c
  #include "pinmap.h"
  ```

  이 위치는 `BOARD_B` 정의(`main.h:164`)보다 **위**에 있지만, 그 시점엔 pinmap.h가 BOARD_B에 의존하는 매크로를 사용하지 않는다. **단, pinmap.h 내부의 `#ifdef BOARD_B` 분기가 평가되려면 BOARD_B가 먼저 정의되어야 한다** → 해결책: main.h의 `#define BOARD_B` 줄(`main.h:164`)을 **pinmap.h include보다 앞**으로 이동시킨다. 즉:

  ```c
  /* 최종 main.h 상단 구조 */
  #include "stm32f1xx_ll_*.h" ...       /* LL 드라이버 (기존 유지) */

  /* 보드 분기 플래그 — pinmap.h include 전에 정의되어야 함 */
  #define BOARD_B
  //#define MOTOR_MULTI_MOVE
  //#define HW_TEST
  //#define RMC_BLACK

  /* 도메인 umbrella */
  #include "pinmap.h"

  /* 이후 기존 typedef / 상수 / prototype / BdcSystem 선언 등 유지 */
  ```

  `#define BOARD_B`를 `main.h:164`에서 상단으로 옮기면서 주변 주석(`//해당 코드 주석 처리로 V3.0에 쓸지 V3.1에 쓸지 선택`)도 함께 이동.

- [ ] **Step 5: STM32CubeIDE Debug 빌드**

  STM32CubeIDE에서 `Project → Build All` (Debug configuration).

  Expected: 에러 0. warning 개수는 베이스라인(커밋 `59db561`)과 동일.

  문제 발생 시 대응:
  - `'LL_GPIO_PIN_N' undeclared` → pinmap.h의 `#include "stm32f1xx_ll_gpio.h"` 누락 확인
  - `macro redefinition` → main.h에서 동일 매크로가 삭제됐는지 확인

- [ ] **Step 6: `.bin` 크기 diff**

  ```bash
  cd /c/PR25/Healerbot/Sourcecode/ChairControl3_1
  ls -l Debug/ChairControl3.bin
  git show HEAD:Debug/ChairControl3.bin | wc -c   # 이전 커밋 크기
  ```

  Expected: 차이 0 바이트 (매크로 이동만이므로 컴파일 결과 동일해야 함).
  차이 > 0이면 → 매크로 정의 변경 또는 include 순서 영향 가능. 원인 분석 후 진행.

- [ ] **Step 7: 실기 스모크 (사용자)**

  플래싱 후 부팅만 확인(CpuRun LED 하트비트 정상). SP 모드 불필요 — 기능 영향 없는 리팩토링 단계.

- [ ] **Step 8: Commit**

  ```bash
  git add Core/Inc/pinmap.h Core/Inc/main.h
  git commit -m "refactor(pinmap): extract GPIO pin macros from main.h

  Move the GPIO pin and GPIO_Port #defines (main.h:117-245) to a
  dedicated header. BOARD_B conditional branches are preserved verbatim.
  main.h becomes an umbrella that includes pinmap.h after BOARD_B is
  defined. No behavior change."
  ```

- [ ] **Step 9: `worklog.md` 업데이트**

  `worklog.md` 2026-04-23 섹션에 아래 bullet 추가:
  - `커밋 1 완료: pinmap.h 추출. .bin 크기 <SIZE> bytes (변화 <DELTA>).`

---

### Task 2: `hb_storage.{c,h}` 추출 — I2C bit-bang + EEPROM

**Files:**
- Create: `Core/Inc/hb_storage.h`
- Create: `Core/Src/hb_storage.c`
- Modify: `Core/Inc/main.h` (EEPROM 주소/컨트롤 바이트 매크로 이동 + umbrella include 추가)
- Modify: `Core/Src/main.c` (I2C·EEPROM 함수 삭제, `delay()`·`nop_op()` 삭제)

**위험도:** 🟢 낮음 — 부팅 시 홈 위치 복원 외에는 런타임 영향 최소.

- [ ] **Step 1: 이동 대상 확정 (grep)**

  다음 함수들이 `hb_storage.c`로 이동한다 (main.c 라인 번호):
  - `void nop_op(void)` — main.c:2579
  - `void I2C_Start(void)` — main.c:2584
  - `void I2C_Stop(void)` — main.c:2605
  - `void I2C_Ack(void)` — main.c:2616
  - `void I2C_Write_Data(u8 data)` — main.c:2626
  - `u8 I2C_Read_Data(void)` — main.c:2651
  - `void EepromWrite(u16 Waddress, u8 WData)` — main.c:2682
  - `u8 EepromRead(u16 Raddress)` — main.c:2712
  - `void EEPROM_Write(u16 Address, u8 Data)` — main.c:2744
  - `u8 EEPROM_Read(u16 Address)` — main.c:2749
  - `void EEPROM_Save(u16 Address, u8 *Buffer, u16 Size)` — main.c:2754
  - `void EEPROM_Erase()` — main.c:2761
  - `void EEPROM_Load(u16 Address, u8 *Buffer, u16 Size)` — main.c:2769
  - `void I2C_SDA_in_Mode()` — main.c:2778
  - `void I2C_SCL_out_Mode()` — main.c:2789
  - `void I2C_SDA_out_Mode()` — main.c:2802
  - `void delay()` — main.c:2829

  외부 호출처 재확인:
  ```bash
  grep -n '\bEEPROM_\(Read\|Write\|Save\|Load\|Erase\)\b' Core/Src/main.c Core/Inc/main.h
  grep -n '\b\(I2C_\|Eeprom\|nop_op\|^delay(\)' Core/Src/
  ```

  Expected: `EEPROM_*` 5개 함수만 외부(`main.c` 내 타 함수)에서 호출. 나머지 `I2C_*`, `EepromRead/Write`, `nop_op`, `delay`는 자기들끼리만 호출.

- [ ] **Step 2: 이동할 매크로 확정**

  `main.h`에서 이동:
  - `main.h:299~300`: `D_WControl`, `D_RControl`
  - `main.h:265~266`, `main.h:302~303`: `EEPROM_ADDR_FW_VERSION1/2` (**중복 정의 존재** — Step 4에서 처리)
  - `main.h:305~316`: 홈 위치 EEPROM 주소 10개 (`EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_MSB` 등)
  - `main.h:364~376`: I2C 비트뱅 GPIO 매크로 (BOARD_B 분기 포함):
    ```c
    #ifdef BOARD_B
    #define I2C_SCL_HIGH    LL_GPIO_SetOutputPin(GPIOE, I2C_SCL_Pin)
    #define I2C_SCL_LOW     LL_GPIO_ResetOutputPin(GPIOE, I2C_SCL_Pin)
    #define I2C_SDA_HIGH    LL_GPIO_SetOutputPin(GPIOE, I2C_SDA_Pin)
    #define I2C_SDA_LOW     LL_GPIO_ResetOutputPin(GPIOE, I2C_SDA_Pin)
    #define I2C_SDA_READ    LL_GPIO_IsInputPinSet(GPIOE, I2C_SDA_Pin)
    #else
    /* GPIOB variants */
    #endif
    ```
    이 매크로들은 hb_storage 내부 함수만 사용하므로 **`hb_storage.c` 파일 상단에 private (헤더 노출 안 함)** 으로 이동.

- [ ] **Step 3: `Core/Inc/hb_storage.h` 생성**

  ```c
  /**
    ******************************************************************************
    * @file    hb_storage.h
    * @brief   EEPROM public API (I2C bit-bang backend is internal).
    ******************************************************************************
    */
  #ifndef HB_STORAGE_H
  #define HB_STORAGE_H

  #ifdef __cplusplus
  extern "C" {
  #endif

  #include "main.h"

  /* EEPROM control bytes (external 24Cxx I2C EEPROM) */
  #define D_WControl  0xa0  /* 0b10100000 */
  #define D_RControl  0xa1  /* 0b10100001 */

  /* EEPROM address map */
  #define EEPROM_ADDR_FW_VERSION1                             0
  #define EEPROM_ADDR_FW_VERSION2                             1

  #define EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_MSB       2
  #define EEPROM_ADDR_LEFT_CHAIR_BACK_HOME_POSITION_LSB       3
  #define EEPROM_ADDR_LEFT_CHAIR_HEIGHT_HOME_POSITION_MSB     4
  #define EEPROM_ADDR_LEFT_CHAIR_HEIGHT_HOME_POSITION_LSB     5

  #define EEPROM_ADDR_RIGHT_CHAIR_BACK_HOME_POSITION_MSB      6
  #define EEPROM_ADDR_RIGHT_CHAIR_BACK_HOME_POSITION_LSB      7
  #define EEPROM_ADDR_RIGHT_CHAIR_HEIGHT_HOME_POSITION_MSB    8
  #define EEPROM_ADDR_RIGHT_CHAIR_HEIGHT_HOME_POSITION_LSB    9

  #define EEPROM_ADDR_ROBOT_HEIGHT_HOME_POSITION_MSB          10
  #define EEPROM_ADDR_ROBOT_HEIGHT_HOME_POSITION_LSB          11

  /* Public API */
  void EEPROM_Write(u16 Address, u8 Data);
  u8   EEPROM_Read (u16 Address);
  void EEPROM_Save (u16 Address, u8 *Buffer, u16 Size);
  void EEPROM_Load (u16 Address, u8 *Buffer, u16 Size);
  void EEPROM_Erase(void);

  #ifdef __cplusplus
  }
  #endif
  #endif /* HB_STORAGE_H */
  ```

  > 주의: 원본 `main.h:302~303`의 `EEPROM_ADDR_FW_VERSION1/2` 는 이미 `main.h:265~266`에 동일하게 정의되어 있어 **중복**이다. hb_storage.h에는 한 번만 기재.

- [ ] **Step 4: `main.h`에서 해당 매크로 삭제**

  삭제 대상 줄:
  - `main.h:265~266` (또는 `main.h:302~303` 중 하나 — 먼저 나타나는 것을 남기고 다른 것 삭제. 판단: Step 2에서 확인한 `main.h:265~266`은 이미 Private defines 영역 초입이고 `main.h:302~303`이 중복이므로 **`main.h:302~303` 쪽을 삭제**)
  - `main.h:299~316` 중 매크로만 해당(주소 상수) 전부 삭제. `D_WControl`·`D_RControl` 포함.

  `main.h` 줄 번호는 Step 3 후 재조정되어 있으므로 **실제 편집 시 패턴 기반 삭제** 권장:
  ```bash
  grep -n 'EEPROM_ADDR_\|D_WControl\|D_RControl' Core/Inc/main.h
  ```
  위 grep이 0줄 반환할 때까지 해당 정의 삭제.

- [ ] **Step 5: `Core/Src/hb_storage.c` 생성**

  파일 skeleton:
  ```c
  /**
    ******************************************************************************
    * @file    hb_storage.c
    * @brief   External I2C EEPROM access (bit-bang I2C).
    ******************************************************************************
    */
  #include "main.h"
  #include "hb_storage.h"

  /* --- Private I2C GPIO macros (BOARD_B-conditional) --- */
  /* main.h:364~376에서 이동 — 그대로 붙여넣기 */
  #ifdef BOARD_B
  #define I2C_SCL_HIGH    LL_GPIO_SetOutputPin  (GPIOE, I2C_SCL_Pin)
  #define I2C_SCL_LOW     LL_GPIO_ResetOutputPin(GPIOE, I2C_SCL_Pin)
  #define I2C_SDA_HIGH    LL_GPIO_SetOutputPin  (GPIOE, I2C_SDA_Pin)
  #define I2C_SDA_LOW     LL_GPIO_ResetOutputPin(GPIOE, I2C_SDA_Pin)
  #define I2C_SDA_READ    LL_GPIO_IsInputPinSet (GPIOE, I2C_SDA_Pin)
  #else
  #define I2C_SCL_HIGH    LL_GPIO_SetOutputPin  (GPIOB, I2C_SCL_Pin)
  #define I2C_SCL_LOW     LL_GPIO_ResetOutputPin(GPIOB, I2C_SCL_Pin)
  #define I2C_SDA_HIGH    LL_GPIO_SetOutputPin  (GPIOB, I2C_SDA_Pin)
  #define I2C_SDA_LOW     LL_GPIO_ResetOutputPin(GPIOB, I2C_SDA_Pin)
  #define I2C_SDA_READ    LL_GPIO_IsInputPinSet (GPIOB, I2C_SDA_Pin)
  #endif

  /* --- Private helpers (I2C bit-bang timing) --- */

  static void delay(void);
  static void nop_op(void);
  static void I2C_SDA_in_Mode(void);
  static void I2C_SDA_out_Mode(void);
  static void I2C_SCL_out_Mode(void);
  static void I2C_Start(void);
  static void I2C_Stop(void);
  static void I2C_Ack(void);
  static void I2C_Write_Data(u8 data);
  static u8   I2C_Read_Data(void);
  static void EepromWrite(u16 Waddress, u8 WData);
  static u8   EepromRead(u16 Raddress);

  /* --- 여기부터 아래에 함수 본체 붙여넣기 --- */
  ```

  그 다음 main.c에서 17개 함수 본체를 **line-range 그대로 복사**해 붙여넣는다:
  - main.c:2579~2583 (nop_op)
  - main.c:2584~2604 (I2C_Start)
  - main.c:2605~2615 (I2C_Stop)
  - main.c:2616~2625 (I2C_Ack)
  - main.c:2626~2650 (I2C_Write_Data)
  - main.c:2651~2681 (I2C_Read_Data)
  - main.c:2682~2711 (EepromWrite)
  - main.c:2712~2743 (EepromRead)
  - main.c:2744~2748 (EEPROM_Write)
  - main.c:2749~2753 (EEPROM_Read)
  - main.c:2754~2760 (EEPROM_Save)
  - main.c:2761~2768 (EEPROM_Erase)
  - main.c:2769~2777 (EEPROM_Load)
  - main.c:2778~2788 (I2C_SDA_in_Mode)
  - main.c:2789~2801 (I2C_SCL_out_Mode)
  - main.c:2802~2828 (I2C_SDA_out_Mode)
  - main.c:2829~2836 (delay)

  붙여넣은 후 **public API 5개(`EEPROM_Write/Read/Save/Load/Erase`)를 제외한 12개 함수의 반환/인자 앞에 `static` 키워드 추가**. 예:
  ```c
  static void I2C_Start(void) { ... }
  static u8 EepromRead(u16 Raddress) { ... }
  ```

- [ ] **Step 6: `main.c`에서 해당 함수 17개 제거**

  Step 5에서 복사한 main.c:2579~2836 전체를 **삭제**. Step 3 전 원본 기준.

- [ ] **Step 7: `main.h`에 umbrella include 추가**

  Step 4에서 매크로 삭제한 자리 또는 파일 상단의 도메인 include 섹션에 다음 줄 추가:
  ```c
  #include "hb_storage.h"
  ```

  위치: 기존 `#include "pinmap.h"` 바로 아래.

- [ ] **Step 8: 빌드**

  STM32CubeIDE Debug build.
  Expected: 에러 0, warning 증가 없음.

  문제 발생 시:
  - `'EepromRead' undeclared` in main.c → main.c에서 기존 `EepromRead` 호출이 있는데 이제 static이므로 외부 접근 불가. 호출자가 `EEPROM_Read` 공개 래퍼를 쓰도록 되어 있는지 재확인. 만약 raw `EepromRead/Write`를 직접 부르는 외부 호출자가 있으면 → 해당 호출 스팟을 `EEPROM_Read/Write`로 교체하거나 `EepromRead/Write`를 public으로 승격.
  - `multiple definition of 'delay'` → LL_TIM 또는 stm32f1xx_hal에 동명 함수가 있을 수 있음. static 추가로 해결될 것.

- [ ] **Step 9: `.bin` 크기 diff**

  ```bash
  ls -l Debug/ChairControl3.bin
  git show HEAD:Debug/ChairControl3.bin | wc -c
  ```

  Expected: ±1% 이내.

- [ ] **Step 10: 실기 스모크 (사용자)**

  1. 플래싱 후 정상 부팅 (CpuRun LED 하트비트)
  2. USART2 printf 로그에서 부팅 시 "Home Position Loaded" 또는 유사 메시지의 값이 베이스라인과 일치하는지 확인
  3. 임의의 홈 위치 재설정 → 전원 OFF/ON → 값 유지 확인
  4. 결과를 대화로 보고

- [ ] **Step 11: Commit**

  ```bash
  git add Core/Inc/hb_storage.h Core/Src/hb_storage.c Core/Inc/main.h Core/Src/main.c
  git commit -m "refactor(storage): extract I2C bit-bang and EEPROM from main.c

  Move EEPROM_* (5 public functions), EepromRead/Write, I2C_* (8 helpers),
  nop_op, and delay() to a new hb_storage module. I2C/Eeprom/timing
  helpers become file-local (static). main.h loses the EEPROM address
  and control-byte macros; hb_storage.h owns them. No behavior change."
  ```

- [ ] **Step 12: worklog.md 업데이트**

  `worklog.md`에 커밋 2 완료 + 이동 함수 수 + .bin 크기 변동 기록.

---

### Task 3: `hb_mode.{c,h}` 추출 — SP 모드 + HW_TEST

**Files:**
- Create: `Core/Inc/hb_mode.h`
- Create: `Core/Src/hb_mode.c`
- Modify: `Core/Inc/main.h` (`HWTest_InitTypeDef` typedef 이동, umbrella include 추가)
- Modify: `Core/Src/main.c` (`SPMode()` 함수, `HWTest` 전역 이동)

**위험도:** 🟢 낮음.

- [ ] **Step 1: 이동 대상 확정**

  함수:
  - `void SPMode(void)` — main.c:1466

  전역:
  - `HWTest_InitTypeDef HWTest;` — main.c:46

  타입 정의:
  - `typedef struct { ... } HWTest_InitTypeDef;` — main.h에서 grep으로 위치 찾기:
  ```bash
  grep -n 'HWTest_InitTypeDef' Core/Inc/main.h
  ```

  HW_TEST 매크로 블록:
  - `main.h:262`: `//#define HW_TEST` (주석 상태 유지)

  외부 참조:
  ```bash
  grep -n 'SPMode\|HWTest\b\|HW_TEST' Core/Src/main.c Core/Src/stm32f1xx_it.c
  ```

  Expected: `SPMode()`는 main()에서 1회 호출 (main.c의 boot 분기), `HWTest` 전역은 stm32f1xx_it.c의 TIM7 ISR에서 `HW_TEST` 매크로 보호 하에 참조.

- [ ] **Step 2: `hb_mode.h` 생성**

  ```c
  /**
    ******************************************************************************
    * @file    hb_mode.h
    * @brief   Operating modes: SP (service) mode and HW_TEST lifetime endurance counters.
    ******************************************************************************
    */
  #ifndef HB_MODE_H
  #define HB_MODE_H

  #ifdef __cplusplus
  extern "C" {
  #endif

  #include "main.h"

  /* (main.h에서 이동한 HWTest_InitTypeDef 정의 붙여넣기) */
  typedef struct {
      /* 기존 필드 전부 */
  } HWTest_InitTypeDef;

  extern HWTest_InitTypeDef HWTest;

  /* Public API */
  void SPMode(void);                /* main()에서 조건부 호출 */
  void hb_mode_tick_1ms(void);      /* HW_TEST 비활성 시 빈 함수 */

  #ifdef __cplusplus
  }
  #endif
  #endif /* HB_MODE_H */
  ```

  `HWTest_InitTypeDef` 실제 필드는 main.h에서 그대로 복사. (HANDOVER §11 참조: Minute/Second + 각 모터별 왕복 카운터와 딜레이 타이머)

- [ ] **Step 3: `hb_mode.c` 생성**

  ```c
  /**
    ******************************************************************************
    * @file    hb_mode.c
    * @brief   Operating mode handlers.
    ******************************************************************************
    */
  #include "main.h"
  #include "hb_mode.h"

  HWTest_InitTypeDef HWTest;   /* main.c:46에서 이동 */

  /* (main.c:1466~의 SPMode 본체 붙여넣기) */
  void SPMode(void) {
      /* ... */
  }

  /* HW_TEST tick handler — 빌드 플래그 비활성 시 빈 본체 */
  void hb_mode_tick_1ms(void) {
  #ifdef HW_TEST
      /* TIM7 ISR이 원래 수행하던 HW_TEST 카운터/모터 제어 코드는 Task 4에서
       * stm32f1xx_it.c의 TIM7_IRQHandler 본체에서 이 함수로 이전된다.
       * 이 Task 3 단계에서는 뼈대만 준비한다.
       */
  #endif
  }
  ```

  > **주의**: `hb_mode_tick_1ms`의 실제 HW_TEST 카운터 로직은 Task 4(sensor) 수행 후 Task 5 직전에 stm32f1xx_it.c의 TIM7_IRQHandler에서 분리되며, 본 Task 3 시점에는 **빈 본체로 둔다**. TIM7 ISR은 아직 기존 구조로 동작 중이므로 HW_TEST 로직도 거기서 계속 실행된다.

- [ ] **Step 4: `main.c`에서 이동**

  - `main.c:46`의 `HWTest_InitTypeDef HWTest;` 줄 삭제
  - `main.c:1466~`의 `SPMode()` 함수 본체 삭제 (함수 끝까지, 다음 함수 시작 전까지)

- [ ] **Step 5: `main.h`에서 typedef 삭제**

  `main.h`에서 `typedef struct { ... } HWTest_InitTypeDef;` 블록 삭제.

  ```bash
  grep -n 'HWTest' Core/Inc/main.h
  ```

  Expected: 0 hit (typedef 완전 제거).

- [ ] **Step 6: `main.h`에 umbrella include 추가**

  ```c
  #include "hb_mode.h"
  ```

  위치: 기존 `#include "hb_storage.h"` 아래.

- [ ] **Step 7: 빌드 + 스모크**

  Debug build → 에러 0.
  실기: 부팅 시 SW_ROBOT_UP 누른 채 전원 ON → SP 모드 진입, 기존 printf 출력과 동일.

- [ ] **Step 8: Commit**

  ```bash
  git add Core/Inc/hb_mode.h Core/Src/hb_mode.c Core/Inc/main.h Core/Src/main.c
  git commit -m "refactor(mode): extract SPMode and HW_TEST skeleton

  Move SPMode() body and HWTest global to a new hb_mode module.
  HWTest_InitTypeDef migrates from main.h to hb_mode.h. hb_mode_tick_1ms
  is declared with an empty body for now; the HW_TEST counter logic
  stays in stm32f1xx_it.c:TIM7_IRQHandler until Task 5. No behavior change."
  ```

- [ ] **Step 9: worklog.md 업데이트**

---

### Task 4: `hb_sensor.{c,h}` 추출 — Laser/Pot + ADC 필터 + TIM7 sensor hook

**Files:**
- Create: `Core/Inc/hb_sensor.h`
- Create: `Core/Src/hb_sensor.c`
- Modify: `Core/Inc/main.h` (센서 typedef 4종 이동, umbrella include)
- Modify: `Core/Src/main.c` (센서 전역·`internal_adc_value` 이동)
- Modify: `Core/Src/stm32f1xx_it.c` (TIM7 ISR의 ADC/필터 블록을 `hb_sensor_tick_1ms` 호출로 대체)

**위험도:** 🟡 중 — ISR 경계 첫 진입. 센서 값 정량 비교 필수.

- [ ] **Step 1: 이동 대상 확정**

  **전역 (main.c:35~41, 64):**
  - `ADConverter` (ADC_Healerbot_InitTypeDef) — main.c:35
  - `Chair1LengthAvg`, `Chair2LengthAvg`, `Chair1HeightAvg`, `Chair2HeightAvg`, `RobotHeightAvg` (AVERAGE_InitTypeDef) — main.c:36
  - `LaserChair1Up`, `LaserChair1Front`, `LaserChair2Up`, `LaserChair2Front`, `LaserRobotUp` (LaserSensor_InitTypeDef) — main.c:40
  - `PotentioChair1Height`, `PotentioChair2Height` (Potentiometer_InitTypeDef) — main.c:41
  - `internal_adc_value[10]` — main.c:64

  **typedef (main.h):**
  ```bash
  grep -n 'typedef struct' Core/Inc/main.h
  ```
  위에서 확인된 line 중 다음 4개:
  - `ADC_Healerbot_InitTypeDef`
  - `LaserSensor_InitTypeDef`
  - `Potentiometer_InitTypeDef`
  - `AVERAGE_InitTypeDef`

  **ISR 이동 대상 (stm32f1xx_it.c:348~587 내부):**
  - Chair1 Length ADC 10-sample 이동평균 + voltage + mm 변환 블록
  - Chair2 Length ADC 동일
  - Robot Height ADC 5-sample 미디안 필터 + mm 변환
  - Chair1/Chair2 Potentiometer 10-sample 이동평균 + mm 변환
  - (BDCMotor.MotorTimer 감산과 LED 하트비트는 **제외** — 이들은 Task 5·7에서 각각 이동)

- [ ] **Step 2: `hb_sensor.h` 생성**

  ```c
  /**
    ******************************************************************************
    * @file    hb_sensor.h
    * @brief   Laser/Potentiometer sensors, ADC averaging/median filters.
    ******************************************************************************
    */
  #ifndef HB_SENSOR_H
  #define HB_SENSOR_H

  #ifdef __cplusplus
  extern "C" {
  #endif

  #include "main.h"

  /* (main.h에서 이동한 4개 typedef를 여기에 그대로 붙여넣기)
   * ADC_Healerbot_InitTypeDef
   * LaserSensor_InitTypeDef
   * Potentiometer_InitTypeDef
   * AVERAGE_InitTypeDef
   */

  /* ADC DMA target buffer */
  extern u16 internal_adc_value[10];

  /* Sensor state */
  extern ADC_Healerbot_InitTypeDef ADConverter;
  extern LaserSensor_InitTypeDef LaserChair1Up, LaserChair1Front,
                                 LaserChair2Up, LaserChair2Front, LaserRobotUp;
  extern Potentiometer_InitTypeDef PotentioChair1Height, PotentioChair2Height;
  extern AVERAGE_InitTypeDef Chair1LengthAvg, Chair2LengthAvg,
                             Chair1HeightAvg, Chair2HeightAvg, RobotHeightAvg;

  /* 1ms tick — called from TIM7_IRQHandler */
  void hb_sensor_tick_1ms(void);

  #ifdef __cplusplus
  }
  #endif
  #endif /* HB_SENSOR_H */
  ```

- [ ] **Step 3: `main.h`에서 typedef 4개 제거**

  `grep -n 'ADC_Healerbot_InitTypeDef\|LaserSensor_InitTypeDef\|Potentiometer_InitTypeDef\|AVERAGE_InitTypeDef' Core/Inc/main.h` 로 위치 확인 후 각 `typedef struct { ... } Xxx;` 블록 전체 삭제.

  Expected: 삭제 후 grep 결과 0줄.

- [ ] **Step 4: `main.h`에 umbrella include 추가**

  ```c
  #include "hb_sensor.h"
  ```
  기존 `#include "hb_mode.h"` 아래 위치.

- [ ] **Step 5: `hb_sensor.c` 생성**

  ```c
  /**
    ******************************************************************************
    * @file    hb_sensor.c
    * @brief   Sensor state, ADC averaging / median filtering, mm conversion.
    ******************************************************************************
    */
  #include "main.h"
  #include "hb_sensor.h"

  /* Storage */
  u16 internal_adc_value[10];
  ADC_Healerbot_InitTypeDef ADConverter;
  LaserSensor_InitTypeDef LaserChair1Up, LaserChair1Front,
                          LaserChair2Up, LaserChair2Front, LaserRobotUp;
  Potentiometer_InitTypeDef PotentioChair1Height, PotentioChair2Height;
  AVERAGE_InitTypeDef Chair1LengthAvg, Chair2LengthAvg,
                      Chair1HeightAvg, Chair2HeightAvg, RobotHeightAvg;

  void hb_sensor_tick_1ms(void) {
      /* stm32f1xx_it.c의 TIM7_IRQHandler 내부에서 이동한 블록:
       *   - Chair1 Length 10-sample moving average + voltage + mm
       *   - Chair2 Length 동일
       *   - Robot Height 5-sample median + mm
       *   - Chair1/Chair2 Potentiometer 10-sample avg + mm
       * 원본 실행 순서를 그대로 보존한다.
       */
  }
  ```

- [ ] **Step 6: `main.c`에서 전역 이동**

  main.c:35~41, main.c:64 줄에서 다음 전역 정의 삭제 (hb_sensor.c로 이미 이전):
  - `ADConverter`
  - `Chair1LengthAvg ... RobotHeightAvg` (5개)
  - `LaserChair1Up ... LaserRobotUp` (5개)
  - `PotentioChair1Height`, `PotentioChair2Height`
  - `internal_adc_value[10]`

- [ ] **Step 7: `stm32f1xx_it.c`에서 `extern` 선언 제거**

  stm32f1xx_it.c:34~37, :56 줄 삭제:
  ```c
  extern ADC_Healerbot_InitTypeDef ADConverter;
  extern AVERAGE_InitTypeDef Chair1LengthAvg, ...;
  extern LaserSensor_InitTypeDef ...;
  extern Potentiometer_InitTypeDef ...;
  extern u16 internal_adc_value[10];
  ```

  이제 `main.h` umbrella가 `hb_sensor.h`를 include하고 거기서 `extern` 선언되므로, `stm32f1xx_it.c`는 자동으로 접근 가능.

- [ ] **Step 8: TIM7 ISR 본체 분리**

  `stm32f1xx_it.c:348~587`의 `TIM7_IRQHandler` 본체에서:
  - Chair1 Length ADC 블록 (대략 line 388~430 구간 — 실제 편집 전 파일 확인)
  - Chair2 Length ADC 블록
  - Robot Height ADC 미디안 블록
  - Chair1 Potentio 블록
  - Chair2 Potentio 블록

  를 **잘라내어** `hb_sensor_tick_1ms`의 본체로 붙여넣는다. 붙여넣는 순서는 원본 실행 순서와 동일.

  `TIM7_IRQHandler` 본체에는 다음 한 줄이 삽입된다 (위에서 잘라낸 자리):
  ```c
  hb_sensor_tick_1ms();
  ```

  실제 편집 절차:
  1. stm32f1xx_it.c를 백업(`cp stm32f1xx_it.c stm32f1xx_it.c.task4.bak`)
  2. TIM7_IRQHandler 본체에서 LED 하트비트 블록은 그대로 두고, BDCMotor.MotorTimer 감산 블록도 그대로 두고, **중간의 ADC/필터 블록만** 잘라낸다
  3. 잘라낸 내용을 `hb_sensor.c`의 `hb_sensor_tick_1ms` 본체 `{` 직후에 순서 그대로 붙여넣는다
  4. 잘라낸 자리에 `hb_sensor_tick_1ms();` 한 줄 삽입
  5. 백업 파일과 diff 해서 **없어진 줄 = 추가된 줄(+ 한 함수 호출)** 인지 확인:
     ```bash
     diff Core/Src/stm32f1xx_it.c.task4.bak Core/Src/stm32f1xx_it.c
     ```

- [ ] **Step 9: 빌드**

  Debug build. 에러 0.

  예상 오류:
  - `'LL_ADC_*' undeclared` in hb_sensor.c → `#include "main.h"`가 umbrella 역할로 LL 드라이버까지 포함하는지 확인. 안되면 `#include "stm32f1xx_ll_adc.h"` 추가.

- [ ] **Step 10: `.bin` 크기 diff + 실기 정량 검증**

  ```bash
  ls -l Debug/ChairControl3.bin
  ```
  ±1% 이내.

  **사용자 실기 검증:**
  1. 플래싱 후 SP 모드 부팅 → 30초 USART2 로그 캡처 → `baseline/sp_mode_task4.log`
  2. `baseline/sp_mode_baseline.log` 와 Laser/Pot 정지 값 비교
  3. 각 센서 값이 ±1mm 이내인지 확인
  4. 오실로스코프로 TIM7 ISR 지속시간 캡처 (예: CpuRun_Pin 대신 임시 GPIO 토글 삽입은 하지 않음 — TIM7 ISR 진입/이탈 시각 측정으로 대체 가능)
  5. 결과를 대화로 보고

- [ ] **Step 11: Commit**

  ```bash
  git add Core/Inc/hb_sensor.h Core/Src/hb_sensor.c Core/Inc/main.h Core/Src/main.c Core/Src/stm32f1xx_it.c
  git commit -m "refactor(sensor): extract Laser/Potentiometer state and ADC filters

  Move sensor typedefs (main.h), sensor globals (main.c), and the 1 ms
  ADC averaging / median / mm-conversion block out of TIM7_IRQHandler
  into hb_sensor_tick_1ms. Original execution order and values preserved.
  Oscilloscope verification of TIM7 ISR duration within baseline ±10%."
  ```

- [ ] **Step 12: worklog.md 업데이트**

  Laser/Pot before/after 값과 TIM7 ISR 측정값 기록.

---

### Task 5: `hb_motor.{c,h}` 추출 — 10개 모터 구동 함수 + MotorTimer tick

**Files:**
- Create: `Core/Inc/hb_motor.h`
- Create: `Core/Src/hb_motor.c`
- Modify: `Core/Inc/main.h` (모터 typedef 2종 + 모터 관련 상수 이동)
- Modify: `Core/Src/main.c` (10개 모터 함수 + `BDCMotor1..5` + `Chair1/Chair2/Robot` + `g_is_homing` 이동)
- Modify: `Core/Src/stm32f1xx_it.c` (TIM7 ISR의 MotorTimer 감산 블록을 `hb_motor_tick_1ms`로 이전)

**위험도:** 🟡 중 — 10개 함수 + 전역 9개. 수동 버튼·리밋·타임아웃 모두 실기 확인 필요.

- [ ] **Step 1: 이동 대상 확정**

  **함수 (main.c):**
  - `u8 LeftChairGoFront(u8 Auto, u16 TargetPos)` — main.c:762
  - `u8 LeftChairGoBack(u8 Auto, u16 TargetPos)` — main.c:820
  - `u8 LeftChairGoUp(u8 Auto, u16 TargetPos)` — main.c:878
  - `u8 LeftChairGoDown(u8 Auto, u16 TargetPos)` — main.c:920
  - `u8 RightChairGoFront(u8 Auto, u16 TargetPos)` — main.c:1058
  - `u8 RightChairGoBack(u8 Auto, u16 TargetPos)` — main.c:1111
  - `u8 RightChairGoUp(u8 Auto, u16 TargetPos)` — main.c:1166
  - `u8 RightChairGoDown(u8 Auto, u16 TargetPos)` — main.c:1211
  - `u8 RobotGoUp(u8 Auto, u16 TargetPos)` — main.c:1348
  - `u8 RobotGoDown(u8 Auto, u16 TargetPos)` — main.c:1408

  **전역:**
  - `BDCMotor1, BDCMotor2, BDCMotor3, BDCMotor4, BDCMotor5` — main.c:34
  - `Chair1, Chair2, Robot` (MotorCtrl_InitTypeDef) — main.c:39
  - `g_is_homing` — main.c:66

  **typedef (main.h):**
  - `BDCMotor_InitTypeDef`
  - `MotorCtrl_InitTypeDef`

  **상수 (main.h):**
  - main.h:268~272: `ACTIVE_LOW`, `INACTIVE_HIGH`, `HIGH_ACTIVE`, `LOW_INACTIVE` → **main.h에 잔존** (다수 도메인 공유)
  - main.h:277~282: `INCREASE`, `DECREASE`, `MOVE_OK`, `MOVE_LIMIT`, `MOVE_TIME_OVER` → hb_motor.h로 이동
  - main.h:284~297: `ERROR_CHAIR1_FORWARD ... ERROR_ROBOT_DOWN`, `WARNING_HIGH_CURRENT_*`, `WARNING_MOTOR_DRIVER_FAULT` → hb_motor.h로 이동
  - main.h:318~320: `FWD_BACK_LIMIT_TIME`, `UP_DOWN_LIMIT_TIME`, `ROBOT_UP_DOWN_LIMIT_TIME` → hb_motor.h
  - main.h:343~345: `CHAIR_FRONT_BACK`, `CHAIR_UP_DOWN`, `ROBOT_UP_DOWN` → hb_motor.h
  - main.h:274~275: `SENS_MARGIN`, `PRE_STOP` → hb_motor.h (모터 정지 마진)

  **ISR (stm32f1xx_it.c TIM7_IRQHandler):**
  - BDCMotor1..5 MotorTimer 감산 블록 → `hb_motor_tick_1ms`로 이전

- [ ] **Step 2: `hb_motor.h` 생성**

  ```c
  /**
    ******************************************************************************
    * @file    hb_motor.h
    * @brief   BDC motor drive functions, motor state, timer countdown.
    ******************************************************************************
    */
  #ifndef HB_MOTOR_H
  #define HB_MOTOR_H

  #ifdef __cplusplus
  extern "C" {
  #endif

  #include "main.h"

  /* Motor state / direction constants */
  #define INCREASE   0
  #define DECREASE   1

  #define MOVE_OK         1
  #define MOVE_LIMIT      2
  #define MOVE_TIME_OVER  3

  /* Margins (currently 0, tuning hooks preserved) */
  #define SENS_MARGIN  0
  #define PRE_STOP     0

  /* Error codes (reported over UART 0x4E) */
  #define ERROR_CHAIR1_FORWARD    1
  #define ERROR_CHAIR1_BACKWARD   2
  #define ERROR_CHAIR1_UP         3
  #define ERROR_CHAIR1_DOWN       4
  #define ERROR_CHAIR2_FORWARD    5
  #define ERROR_CHAIR2_BACKWARD   6
  #define ERROR_CHAIR2_UP         7
  #define ERROR_CHAIR2_DOWN       8
  #define ERROR_ROBOT_UP          9
  #define ERROR_ROBOT_DOWN        10

  #define WARNING_HIGH_CURRENT_MOTOR1   1
  #define WARNING_HIGH_CURRENT_MOTOR2   2
  #define WARNING_MOTOR_DRIVER_FAULT    4

  /* Movement timeouts (ms) */
  #define FWD_BACK_LIMIT_TIME        16000  /* ≈14 s measured */
  #define UP_DOWN_LIMIT_TIME         24000  /* ≈21 s */
  #define ROBOT_UP_DOWN_LIMIT_TIME   27000  /* ≈23 s */

  /* Motor identifiers within a Chair command */
  #define CHAIR_FRONT_BACK  1
  #define CHAIR_UP_DOWN     2
  #define ROBOT_UP_DOWN     4

  /* (main.h에서 이동한 typedef) */
  typedef struct { /* 기존 BDCMotor_InitTypeDef 필드 그대로 */ } BDCMotor_InitTypeDef;
  typedef struct { /* 기존 MotorCtrl_InitTypeDef 필드 그대로 */ } MotorCtrl_InitTypeDef;

  /* Globals */
  extern BDCMotor_InitTypeDef BDCMotor1, BDCMotor2, BDCMotor3, BDCMotor4, BDCMotor5;
  extern MotorCtrl_InitTypeDef Chair1, Chair2, Robot;
  extern volatile u8 g_is_homing;

  /* Drive functions (blocking) */
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

  /* 1ms tick — called from TIM7_IRQHandler */
  void hb_motor_tick_1ms(void);

  #ifdef __cplusplus
  }
  #endif
  #endif /* HB_MOTOR_H */
  ```

- [ ] **Step 3: `main.h`에서 typedef와 상수 제거**

  grep으로 확인 후 삭제:
  ```bash
  grep -n 'BDCMotor_InitTypeDef\|MotorCtrl_InitTypeDef\|MOVE_OK\|MOVE_LIMIT\|MOVE_TIME_OVER\|INCREASE\|DECREASE\|FWD_BACK_LIMIT_TIME\|UP_DOWN_LIMIT_TIME\|ROBOT_UP_DOWN_LIMIT_TIME\|CHAIR_FRONT_BACK\|CHAIR_UP_DOWN\|ROBOT_UP_DOWN\|ERROR_CHAIR\|ERROR_ROBOT\|WARNING_HIGH_CURRENT\|WARNING_MOTOR_DRIVER\|SENS_MARGIN\|PRE_STOP' Core/Inc/main.h
  ```
  삭제 후 Expected: 0줄.

- [ ] **Step 4: `main.h`에 umbrella include 추가**

  ```c
  #include "hb_motor.h"
  ```
  기존 `#include "hb_sensor.h"` 아래.

- [ ] **Step 5: `hb_motor.c` 생성**

  ```c
  /**
    ******************************************************************************
    * @file    hb_motor.c
    * @brief   BDC motor driving routines.
    ******************************************************************************
    */
  #include "main.h"
  #include "hb_motor.h"

  BDCMotor_InitTypeDef BDCMotor1, BDCMotor2, BDCMotor3, BDCMotor4, BDCMotor5;
  MotorCtrl_InitTypeDef Chair1, Chair2, Robot;
  volatile u8 g_is_homing = 0;  /* 원점잡기 동작 중 여부 플래그 */

  /* (main.c에서 10개 모터 함수 본체 복사) */
  ```

  main.c:762~1464 범위의 10개 함수 본체를 순서대로 복사. 단, 함수 간 여백·주석도 유지.

- [ ] **Step 6: `main.c`에서 이동**

  main.c:34 (`BDCMotor1..5`), main.c:39 (`Chair1, Chair2, Robot`), main.c:66 (`g_is_homing`), main.c:762~1464 (10개 함수 본체) 삭제.

- [ ] **Step 7: TIM7 ISR에서 MotorTimer 블록 이전**

  Task 4와 동일 패턴:
  1. stm32f1xx_it.c 백업
  2. TIM7_IRQHandler 본체에서 `BDCMotor1.MotorTimer`, `BDCMotor2.MotorTimer` ... 5개 감산 블록 잘라냄 (보통 단일 블록 또는 순차 배치)
  3. `hb_motor_tick_1ms()` 본체에 붙여넣기
  4. 잘라낸 자리(`hb_sensor_tick_1ms()` 직후 라인)에 `hb_motor_tick_1ms();` 삽입
  5. stm32f1xx_it.c:38 줄 `extern BDCMotor_InitTypeDef ...` 삭제 (umbrella로 접근)

- [ ] **Step 8: 빌드**

  Debug build → 에러 0.

- [ ] **Step 9: 실기 검증 (사용자)**

  1. 플래싱 후 10개 동작 모두 실행:
     - Chair1 FWD/BACK/UP/DW (수동 버튼)
     - Chair2 FWD/BACK/UP/DW
     - Robot UP/DW
  2. 각 축 리밋 도달 시 자동 정지 확인
  3. 타임아웃 테스트 (예: 리밋 스위치를 차단하여 16초/24초/27초 경과 후 `MOVE_TIME_OVER` 응답 발생 확인)
  4. UART 0x45(홈) / 0x46(타겟) 왕복 확인
  5. `g_is_homing` 플래그 경로: 홈 설정 요청(0x45) 처리 중 수동 버튼 눌렀을 때 동작(플래그 활성 시 일반 리밋 체크 우회 경로 등 확인)

- [ ] **Step 10: Commit**

  ```bash
  git add Core/Inc/hb_motor.h Core/Src/hb_motor.c Core/Inc/main.h Core/Src/main.c Core/Src/stm32f1xx_it.c
  git commit -m "refactor(motor): extract motor drive functions and state

  Move 10 motor drive functions (Left/Right/Robot × Front/Back/Up/Down),
  BDCMotor1..5, Chair1/Chair2/Robot MotorCtrl, g_is_homing, and related
  constants out of main.c into hb_motor. TIM7_IRQHandler motor timer
  countdown moves to hb_motor_tick_1ms preserving execution order."
  ```

- [ ] **Step 11: worklog.md 업데이트**

---

### Task 6: `hb_comm.{c,h}` 추출 — UART 프레임 + UartProcess + 응답 송출

**Files:**
- Create: `Core/Inc/hb_comm.h`
- Create: `Core/Src/hb_comm.c`
- Modify: `Core/Inc/main.h` (`Comm_InitTypeDef` typedef 이동, UART CMD 상수 이동, `UartProcess` prototype **삭제**)
- Modify: `Core/Src/main.c` (9개 UART 함수 본체 이동)
- Modify: `Core/Src/stm32f1xx_it.c` (UART4/USART2 ISR 본체 이전, `Comm1` 정의 이동)

**위험도:** 🔴 높음 — 1490줄 `UartProcess` + DMA+IDLE 조합 ISR. UART 명령 전체 회귀 필수.

- [ ] **Step 1: 이동 대상 확정**

  **함수 (main.c):**
  - `void MasterComm1()` — main.c:1633 (public)
  - `void UartResponseProcess(void)` — main.c:1680 (public)
  - `void UartProcess()` — main.c:1745 (**static으로 격리**)
  - `void UartSmartDeviceRequestGoHomePositionResponse(u8 LR, u8 WhichOne, u16 Length)` — main.c:2213
  - `void UartSmartDeviceRequestGoTargetPositionResponse(u8 Complete, u8 Dir)` — main.c:2246
  - `void UartSmartDeviceSendModelNameResponse(u8 ModelNo)` — main.c:2275
  - `void UartEspRequestRobotStopResponse(u8 WhichOne)` — main.c:2304
  - `void UartSmartDeviceRequestCurrentPositionResponse(u8 WhichOne)` — main.c:2362
  - `void UartPICSendLengthDataAfterManualAdjustment(u8 WhichOne)` — main.c:2460
  - `void UartPICSendErrorCode(u8 WhichOne, u8 ErrorCode1, u8 ErrorCode2)` — main.c:2549

  **전역:**
  - `Comm1` (Comm_InitTypeDef) — stm32f1xx_it.c:33

  **typedef (main.h):** `Comm_InitTypeDef`

  **상수 (main.h):**
  - main.h:323~331: UART CMD 코드 8개 (`UART_SMART_DEVCIE_REQUEST_GO_HOME_POSITION` 등)
  - main.h:352~355: `STX`, `ETX`, `ACK`, `NACK`
  - main.h:334~341: `LEFT_CHAIR`, `RIGHT_CHAIR`, `ROBOT`, `HM_LEFT_CHAIR`, `HM_RIGHT_CHAIR`, `MODEL_HA`, `MODEL_HM`  
    → **판단**: 이들은 모터·모드·통신 모두가 참조하는 **공용 도메인 값**. `main.h` 에 **잔존**시키고 hb_comm.h에 중복 정의하지 않는다.

  **Prototype 제거:** main.h:513 `void UartProcess(void);` 삭제 (static 격리 목적).

  **printf 리다이렉트:** `__io_putchar(int ch)` 함수가 main.c:2817의 `PUTCHAR_PROTOTYPE { ... }` 블록에 정의되어 있다. USART2 송신 경로이므로 hb_comm 소속.
  - main.c:2817~ (`PUTCHAR_PROTOTYPE` 블록 = `int __io_putchar(int ch) { ... }`) → `hb_comm.c`로 이전
  - main.h:357~363 의 `#ifdef __GNUC__ ... #define PUTCHAR_PROTOTYPE ... #endif` 매크로 정의 → **삭제**. hb_comm.c 안에서 `int __io_putchar(int ch)` 시그니처를 직접 사용 (매크로 추상화 제거 — 1회용 매크로).

  **ISR:**
  - stm32f1xx_it.c:234~246 (USART2_IRQHandler 본체) → `hb_comm_on_usart2_isr`
  - stm32f1xx_it.c:247~343 (UART4_IRQHandler 본체) → `hb_comm_on_uart4_isr`

  외부 참조 재확인:
  ```bash
  grep -n 'UartProcess\|MasterComm1\|UartResponseProcess' Core/
  ```
  Expected:
  - `UartProcess`: 호출처는 `MasterComm1()` 하나(main.c:1650). main.h:513 prototype만 추가 정리.
  - `MasterComm1`: main loop (main.c:275)
  - `UartResponseProcess`: main loop

- [ ] **Step 2: `hb_comm.h` 생성**

  ```c
  /**
    ******************************************************************************
    * @file    hb_comm.h
    * @brief   UART4 protocol (frame assembly, dispatch, responses).
    ******************************************************************************
    */
  #ifndef HB_COMM_H
  #define HB_COMM_H

  #ifdef __cplusplus
  extern "C" {
  #endif

  #include "main.h"

  /* Frame markers */
  #define STX   0xF1
  #define ETX   0xF4
  #define ACK   0x06
  #define NACK  0xFF

  /* CMD codes (APP/PIC ↔ STM) */
  #define UART_SMART_DEVCIE_REQUEST_GO_HOME_POSITION        0x45
  #define UART_SMART_DEVCIE_REQUEST_GO_TARGET_POSITION      0x46
  #define UART_ESP_SEND_ROBOT_STOP_RESPONSE                 0x47
  #define UART_SMART_DEVICE_SEND_POSITION_RESPONSE          0x48
  #define UART_SMART_DEVICE_REQUEST_CURRENT_POSITION        0x49
  #define UART_SMART_DEVICE_SEND_MODEL_NAME                 0x4A
  #define UART_PIC_SEND_ERROR_CODE                          0x4E
  #define UART_ESP_SEND_ROBOT_STOP                          0x75

  /* (main.h에서 이동한 Comm_InitTypeDef) */
  typedef struct { /* 기존 필드 전부 */ } Comm_InitTypeDef;

  extern Comm_InitTypeDef Comm1;

  /* Main-loop entry points */
  void MasterComm1(void);
  void UartResponseProcess(void);

  /* Responses (called from motor/mode domains) */
  void UartSmartDeviceRequestGoHomePositionResponse(u8 LR, u8 WhichOne, u16 Length);
  void UartSmartDeviceRequestGoTargetPositionResponse(u8 Complete, u8 Dir);
  void UartSmartDeviceSendModelNameResponse(u8 ModelNo);
  void UartEspRequestRobotStopResponse(u8 WhichOne);
  void UartSmartDeviceRequestCurrentPositionResponse(u8 WhichOne);
  void UartPICSendLengthDataAfterManualAdjustment(u8 WhichOne);
  void UartPICSendErrorCode(u8 WhichOne, u8 ErrorCode1, u8 ErrorCode2);

  /* ISR delegates */
  void hb_comm_on_uart4_isr(void);
  void hb_comm_on_usart2_isr(void);

  #ifdef __cplusplus
  }
  #endif
  #endif /* HB_COMM_H */
  ```

- [ ] **Step 3: `main.h`에서 typedef·UART 상수·`UartProcess`·PUTCHAR_PROTOTYPE 제거**

  삭제 대상:
  - `typedef struct { ... } Comm_InitTypeDef;` 블록
  - main.h:323~331 (UART CMD 8개)
  - main.h:352~355 (STX/ETX/ACK/NACK)
  - main.h:357~363 (PUTCHAR_PROTOTYPE `#ifdef __GNUC__` 블록 — 1회용, 인라인 처리)
  - main.h:513 `void UartProcess(void);`
  - main.h:512 `void MasterComm1(void);` (같이 hb_comm.h로 일원화)

  main.h에서 `MasterComm1` prototype도 제거하는 이유: 이제 hb_comm.h가 소유. umbrella에서 접근 가능.

  grep 확인:
  ```bash
  grep -n 'Comm_InitTypeDef\|UART_SMART\|UART_ESP\|UART_PIC\|\bSTX\b\|\bETX\b\|UartProcess\|MasterComm1' Core/Inc/main.h
  ```
  Expected: 0줄.

- [ ] **Step 4: `main.h`에 umbrella include 추가**

  ```c
  #include "hb_comm.h"
  ```

- [ ] **Step 5: `hb_comm.c` 생성**

  ```c
  /**
    ******************************************************************************
    * @file    hb_comm.c
    * @brief   UART4 protocol implementation.
    ******************************************************************************
    */
  #include "main.h"
  #include "hb_comm.h"
  #include <string.h>

  Comm_InitTypeDef Comm1;  /* stm32f1xx_it.c:33에서 이동 */

  /* Forward declaration for static dispatcher */
  static void UartProcess(void);

  /* (main.c:1633~의 MasterComm1 본체 복사 — UartProcess 호출은 그대로) */
  /* (main.c:1680~의 UartResponseProcess 본체 복사) */
  /* (main.c:1745~의 UartProcess 본체 복사 — 여기선 'static void UartProcess(void)' 로 선언) */
  /* (main.c:2213~의 7개 응답 함수 본체 순서대로 복사) */

  /* printf 리다이렉트 (main.c:2817의 PUTCHAR_PROTOTYPE 블록 이전) */
  int __io_putchar(int ch) {
      /* 원본 본체 그대로: USART2 TXE 대기 후 LL_USART_TransmitData8 등 */
  }

  /* ISR delegates */
  void hb_comm_on_uart4_isr(void) {
      /* stm32f1xx_it.c:247~343 본체 이동 */
  }

  void hb_comm_on_usart2_isr(void) {
      /* stm32f1xx_it.c:234~246 본체 이동 */
  }
  ```

  **중요**: `UartProcess` 본체의 시그니처를 복사할 때 `void UartProcess()` → `static void UartProcess(void)` 로 변경. 인자 리스트 공백 `()` 도 명시적 `(void)` 로 맞춰 K&R warning 회피.

- [ ] **Step 6: `main.c`에서 이동**

  main.c:1633~2577 범위의 UART 관련 함수 9개 본체 삭제. 구체 범위:
  - 1633~1679 (MasterComm1)
  - 1680~1744 (UartResponseProcess)
  - 1745~2212 (UartProcess)
  - 2213~2274 (GoHomePositionResponse + GoTargetPositionResponse)
  - 2275~2303 (SendModelNameResponse)
  - 2304~2361 (EspRequestRobotStopResponse)
  - 2362~2459 (RequestCurrentPositionResponse)
  - 2460~2548 (SendLengthDataAfterManualAdjustment)
  - 2549~2578 (SendErrorCode)
  - main.c:2817~ 의 `PUTCHAR_PROTOTYPE { ... }` 블록 (`__io_putchar` 본체)

  그리고 main.c:43의 `extern Comm_InitTypeDef Comm1;` 삭제 (umbrella로 접근).

- [ ] **Step 7: `stm32f1xx_it.c`에서 Comm1과 ISR 본체 이전**

  1. 백업
  2. stm32f1xx_it.c:33 `Comm_InitTypeDef Comm1;` 삭제
  3. UART4_IRQHandler (247~343) 본체를 잘라내어 `hb_comm.c`의 `hb_comm_on_uart4_isr` 본체로 이전. 잘라낸 자리에 `hb_comm_on_uart4_isr();` 삽입.
  4. USART2_IRQHandler (234~246) 동일 패턴으로 `hb_comm_on_usart2_isr` 로 이전.
  5. `extern` 선언 (있다면) 삭제.

- [ ] **Step 8: 빌드**

  Debug build → 에러 0.

  예상 경고:
  - `'UartProcess' declared 'static' but never defined` → Step 5의 forward declaration과 본체 정의 위치 확인
  - `implicit declaration of 'LL_USART_*'` → `#include "stm32f1xx_ll_usart.h"` 이 main.h umbrella에 있는지 확인

- [ ] **Step 9: 실기 검증 (사용자)**

  1. 플래싱 후 UART4 명령 전체 왕복:
     - 0x45 (홈포지션 이동) — Chair1/Chair2/Robot 각 축
     - 0x46 (타겟 위치 이동) — 3축 동시
     - 0x49 (현재 위치 조회) — 각 축
     - 0x4A (모델 설정)
     - 0x47 (로봇 정지 응답 수신 확인)
     - 0x48 (수동 조정 후 위치 전송)
     - 0x4E (에러 코드 — 강제로 리밋 차단하여 발생)
     - 0x75 (로봇 정지 명령)
  2. 각 명령의 응답 패킷을 베이스라인과 바이트 수준 비교
  3. USART2 printf 출력 정상 (putchar 경로 확인)
  4. 오실로스코프로 UART4 TX 프레임 타이밍 캡처 — STX/ETX 마커 위치와 체크섬 바이트 값이 원본과 동일한지 확인

- [ ] **Step 10: Commit**

  ```bash
  git add Core/Inc/hb_comm.h Core/Src/hb_comm.c Core/Inc/main.h Core/Src/main.c Core/Src/stm32f1xx_it.c
  git commit -m "refactor(comm): extract UART protocol stack

  Move MasterComm1, UartResponseProcess, and 7 Uart*Response functions
  from main.c into hb_comm. UartProcess becomes file-local (static);
  the main.h prototype is dropped. UART4/USART2 IRQ bodies move to
  hb_comm_on_uart4_isr / hb_comm_on_usart2_isr. Comm1 migrates out of
  stm32f1xx_it.c."
  ```

- [ ] **Step 11: worklog.md 업데이트**

---

### Task 7: `hb_system.{c,h}` 추출 — 클럭/GPIO/TIM/UART/ADC init + SysTick/TIM7 heartbeat

**Files:**
- Create: `Core/Inc/hb_system.h`
- Create: `Core/Src/hb_system.c`
- Modify: `Core/Inc/main.h` (NVIC_PRIORITYGROUP_* 이동, `Error_Handler` prototype 이동)
- Modify: `Core/Src/main.c` (6개 init 함수 + `TimingDelay_Decrement` + `delayMsec` + `Error_Handler` + `assert_failed` 이동)
- Modify: `Core/Src/stm32f1xx_it.c` (SysTick/ADC ISR 본체 + TIM7 LED 블록 이전, `Timer7Counter`/`Timer6Counter`/`systick_counter` 이동)

**위험도:** 🟡 중 — 클럭·DMA·TIM init 경로. 부팅 실패 가능성 존재.

- [ ] **Step 1: 이동 대상 확정**

  **함수 (main.c):**
  - `void SystemClock_Config(void)` — main.c:379
  - `static void MX_TIM7_Init(void)` — main.c:420
  - `static void MX_UART4_Init(void)` — main.c:457
  - `static void MX_USART2_UART_Init(void)` — main.c:533
  - `static void MX_GPIO_Init(void)` — main.c:590
  - `void ADC1_Init_LL(void)` — main.c:1538
  - `void TimingDelay_Decrement(void)` — main.c:2837
  - `void delayMsec(u32 delayTime)` — main.c:2845
  - `void Error_Handler(void)` — main.c:2864
  - `void assert_failed(uint8_t *file, uint32_t line)` — main.c:2882

  **Static → Public 승격**: `MX_TIM7_Init`, `MX_UART4_Init`, `MX_USART2_UART_Init`, `MX_GPIO_Init` 4개 함수의 `static` 키워드 제거.

  **전역 (main.c):**
  - `static vu32 TimingDelay;` — main.c:63 (static 유지, hb_system.c 내부)

  **전역 (stm32f1xx_it.c):**
  - `u32 systick_counter;` — :54
  - `u32 Timer7Counter;` — :55
  - `u32 Timer6Counter;` — :57

  **상수 (main.h):**
  - main.h:246~257: NVIC_PRIORITYGROUP_0~4 → hb_system.h

  **Prototype (main.h):**
  - main.h:110 `void Error_Handler(void);` → hb_system.h

  **ISR (stm32f1xx_it.c):**
  - SysTick_Handler 본체 → `hb_system_on_systick`
  - ADC1_2_IRQHandler 본체 → `hb_system_on_adc_isr`
  - TIM7_IRQHandler 내 LED 하트비트 블록 → `hb_system_heartbeat_1ms` (TIM7 본체 마지막 4분할 완성)

- [ ] **Step 2: `hb_system.h` 생성**

  ```c
  /**
    ******************************************************************************
    * @file    hb_system.h
    * @brief   System init (clock, GPIO, peripherals), delay, error handlers, ISR delegates.
    ******************************************************************************
    */
  #ifndef HB_SYSTEM_H
  #define HB_SYSTEM_H

  #ifdef __cplusplus
  extern "C" {
  #endif

  #include "main.h"

  #ifndef NVIC_PRIORITYGROUP_0
  #define NVIC_PRIORITYGROUP_0  ((uint32_t)0x00000007)
  #define NVIC_PRIORITYGROUP_1  ((uint32_t)0x00000006)
  #define NVIC_PRIORITYGROUP_2  ((uint32_t)0x00000005)
  #define NVIC_PRIORITYGROUP_3  ((uint32_t)0x00000004)
  #define NVIC_PRIORITYGROUP_4  ((uint32_t)0x00000003)
  #endif

  /* Init (called from main()) */
  void SystemClock_Config(void);
  void MX_GPIO_Init(void);
  void MX_USART2_UART_Init(void);
  void MX_UART4_Init(void);
  void MX_TIM7_Init(void);
  void ADC1_Init_LL(void);

  /* Utilities */
  void delayMsec(u32 delayTime);
  void Error_Handler(void);

  /* ISR delegates */
  void hb_system_on_systick(void);
  void hb_system_on_adc_isr(void);
  void hb_system_heartbeat_1ms(void);

  /* System timers (defined in hb_system.c) */
  extern u32 Timer7Counter, Timer6Counter, systick_counter;

  #ifdef __cplusplus
  }
  #endif
  #endif /* HB_SYSTEM_H */
  ```

- [ ] **Step 3: `main.h`에서 해당 요소 제거**

  - `main.h:110`의 `void Error_Handler(void);` 삭제
  - `main.h:246~257`의 NVIC 매크로 블록 삭제

  grep:
  ```bash
  grep -n 'NVIC_PRIORITYGROUP\|Error_Handler' Core/Inc/main.h
  ```
  Expected: 0줄.

- [ ] **Step 4: `main.h`에 umbrella include 추가**

  ```c
  #include "hb_system.h"
  ```

- [ ] **Step 5: `hb_system.c` 생성**

  ```c
  /**
    ******************************************************************************
    * @file    hb_system.c
    * @brief   Clock, GPIO, peripheral init; SysTick/ADC/TIM7 LED delegates.
    ******************************************************************************
    */
  #include "main.h"
  #include "hb_system.h"

  /* Private */
  static vu32 TimingDelay;       /* main.c:63에서 이동 */

  /* Shared system timers */
  u32 systick_counter;           /* stm32f1xx_it.c:54에서 이동 */
  u32 Timer7Counter;             /* stm32f1xx_it.c:55에서 이동 */
  u32 Timer6Counter;             /* stm32f1xx_it.c:57에서 이동 */

  /* (main.c 6개 init 함수 본체 복사 — MX_*_Init 4개는 static 제거) */
  void SystemClock_Config(void) { /* main.c:379~ */ }
  void MX_GPIO_Init(void)       { /* main.c:590~ */ }
  void MX_USART2_UART_Init(void){ /* main.c:533~ */ }
  void MX_UART4_Init(void)      { /* main.c:457~ */ }
  void MX_TIM7_Init(void)       { /* main.c:420~ */ }
  void ADC1_Init_LL(void)       { /* main.c:1538~ */ }

  /* (main.c delay/error 본체 복사) */
  void TimingDelay_Decrement(void) { /* main.c:2837~ */ }
  void delayMsec(u32 delayTime)    { /* main.c:2845~ */ }
  void Error_Handler(void)         { /* main.c:2864~ */ }

  /* assert_failed는 __io_putchar 경로처럼 약한 심볼 오버라이드 */
  void assert_failed(uint8_t *file, uint32_t line) {
      /* main.c:2882~ 본체 */
  }

  /* ISR delegates */
  void hb_system_on_systick(void) {
      TimingDelay_Decrement();
      systick_counter++;
  }

  void hb_system_on_adc_isr(void) {
      /* stm32f1xx_it.c:221~233 본체 이동 */
  }

  void hb_system_heartbeat_1ms(void) {
      /* stm32f1xx_it.c TIM7_IRQHandler 내 CpuRun_Pin 500 ms on/off 블록 이동 */
  }
  ```

  > `TimingDelay_Decrement`는 기존 main.c에 있던 그대로 `static` 없이 정의하되, 외부 호출은 `hb_system_on_systick()` 내부에서만 일어나도록 hb_system.h에 **노출하지 않는다**. 소스 파일 내부 link scope 유지. (C에서는 파일 내 정의된 non-static 함수도 실질적으로 외부에서 prototype을 선언해야 호출 가능하므로, prototype을 공개 헤더에 두지 않으면 의도한 격리 달성.)

- [ ] **Step 6: `main.c`에서 이동**

  - main.c:63 `static vu32 TimingDelay;` 삭제
  - main.c:70의 `void SystemClock_Config(void);` forward declaration 삭제 (hb_system.h로 일원화)
  - main.c:71~74의 `static void MX_*_Init(void);` 4개 forward declaration 삭제
  - main.c:77의 `void delayMsec(u32 delayTime);` 삭제
  - main.c:379~ (SystemClock_Config) 삭제
  - main.c:420~ (MX_TIM7_Init) 삭제
  - main.c:457~ (MX_UART4_Init) 삭제
  - main.c:533~ (MX_USART2_UART_Init) 삭제
  - main.c:590~ (MX_GPIO_Init) 삭제
  - main.c:1538~ (ADC1_Init_LL) 삭제
  - main.c:2837~ (TimingDelay_Decrement) 삭제
  - main.c:2845~ (delayMsec) 삭제
  - main.c:2864~ (Error_Handler) 삭제
  - main.c:2882~ (assert_failed) 삭제
  - main.c:44의 `extern u32 Timer7Counter;` 삭제 (umbrella 접근)

  main()의 init 호출 시퀀스(`SystemClock_Config()`, `MX_GPIO_Init()`, `MX_USART2_UART_Init()`, `MX_UART4_Init()`, `MX_TIM7_Init()`, `ADC1_Init_LL()`)는 **건드리지 않는다**. 호출 위치 그대로, 함수 정의만 hb_system.c에 있음.

- [ ] **Step 7: `stm32f1xx_it.c`에서 이전**

  1. 백업
  2. `stm32f1xx_it.c:54~57` 전역 3개 (`systick_counter`, `Timer7Counter`, `Timer6Counter`) 삭제
  3. SysTick_Handler 본체 → `hb_system_on_systick()` 호출 한 줄로 대체
  4. ADC1_2_IRQHandler 본체 → `hb_system_on_adc_isr()` 호출 한 줄로 대체
  5. TIM7_IRQHandler 내부의 CpuRun LED 하트비트 블록 (`CpuRun_Pin` 토글 부분, grep으로 위치 확인:
     ```bash
     grep -n 'CpuRun_Pin' Core/Src/stm32f1xx_it.c
     ```
     ) → `hb_system_heartbeat_1ms()` 호출로 대체
  6. TIM7_IRQHandler 내부의 두 `#ifdef HW_TEST` 블록 (stm32f1xx_it.c:364, 501 인근) → 두 블록을 모두 잘라내어 `hb_mode_tick_1ms()` 본체에 순서 그대로 붙여넣고, 잘라낸 자리에 **현재 이미 있는 `hb_mode_tick_1ms();` 호출만 남도록** 정리. (Task 5에서 이미 `hb_motor_tick_1ms()` 호출이 삽입되어 있고, 그 직후 `hb_mode_tick_1ms()` 호출 한 줄을 같은 시점에 추가했어야 한다 — Task 5에서 누락됐다면 Task 7에서 추가.)
  7. TIM7_IRQHandler의 `Timer7Counter++;` 는 그대로 유지(or 호출 끝에서 수행)

  최종 TIM7_IRQHandler 구조:
  ```c
  void TIM7_IRQHandler(void) {
      if (LL_TIM_IsActiveFlag_UPDATE(TIM7)) {
          LL_TIM_ClearFlag_UPDATE(TIM7);
          hb_system_heartbeat_1ms();   /* LED */
          hb_sensor_tick_1ms();         /* ADC avg/median/mm */
          hb_motor_tick_1ms();          /* MotorTimer countdown */
          hb_mode_tick_1ms();            /* HW_TEST (empty if flag off) */
          Timer7Counter++;
      }
  }
  ```

- [ ] **Step 8: 빌드**

  Debug build → 에러 0.

  위험 오류 대응:
  - `'SystemClock_Config' implicit declaration` in main.c → hb_system.h가 main.h umbrella에 포함됐는지 확인
  - `multiple definition of 'MX_GPIO_Init'` → main.c에서 함수 본체가 완전 삭제됐는지 확인
  - `'TimingDelay' undeclared` in hb_system.c → 파일 상단 static 선언 확인
  - TIM7 ISR 누락 호출 → TIM7_IRQHandler 본체의 5줄(heartbeat/sensor/motor/mode/Timer7Counter++) 순서 확인

- [ ] **Step 9: 실기 검증 (사용자)**

  1. 플래싱 후 부팅 (최대 위험: 클럭 초기화 실패 → CpuRun LED 미동작)
  2. CpuRun LED 500ms on/off 주기 확인 (오실로스코프로 측정 가능 — 1Hz ±0.1%)
  3. ADC DMA 순환 정상 (SP 모드 로그에서 Laser/Pot 값 실시간 변동 확인)
  4. `Timer7Counter` 증가 추적 (printf에 노출되어 있다면 확인)
  5. UART4/USART2 이미 Task 6에서 검증됐으나 재부팅 후 한 번 더 확인
  6. 전체 SP 모드 로그 30초 캡처 → 베이스라인과 전반 비교

- [ ] **Step 10: Commit**

  ```bash
  git add Core/Inc/hb_system.h Core/Src/hb_system.c Core/Inc/main.h Core/Src/main.c Core/Src/stm32f1xx_it.c
  git commit -m "refactor(system): extract init, delay, and ISR dispatch glue

  Move SystemClock_Config, MX_*_Init (static removed), ADC1_Init_LL,
  delayMsec, TimingDelay_Decrement, Error_Handler, and assert_failed
  from main.c to hb_system. SysTick / ADC1_2 / TIM7-heartbeat bodies
  move into hb_system ISR delegates. Timer7Counter, Timer6Counter,
  systick_counter migrate out of stm32f1xx_it.c. TIM7_IRQHandler is
  now a 4-call dispatcher in original execution order."
  ```

- [ ] **Step 11: worklog.md 업데이트**

---

### Task 8: 마무리 정리 — `main.c` 최종 형태 확인

**Files:**
- Modify: `Core/Src/main.c` (죽은 주석 / 중복 `extern` 정리)
- Modify: `Core/Src/stm32f1xx_it.c` (죽은 `extern` / 주석 정리)
- Modify: `Core/Inc/main.h` (umbrella 형태 깔끔하게 정리)

**위험도:** 🟢 — 함수 본체·동작 로직 건드리지 않음.

- [ ] **Step 1: `main.c` 상태 점검**

  ```bash
  wc -l Core/Src/main.c
  ```
  Expected: ≤ 300줄 수준.

  ```bash
  grep -n '^extern ' Core/Src/main.c
  ```
  Expected: 거의 0줄 (umbrella로 대부분 접근). 남아 있다면 조사.

  ```bash
  # main() 외 다른 비-static 함수 정의가 main.c에 잔존하는지 검사
  grep -nE '^(void|u8|u16|u32|int|static\s+\w+)\s+\w+\s*\(' Core/Src/main.c
  ```
  Expected: `int main(void)` 한 줄만 매치. 다른 함수가 잡히면 적절한 hb_*.c로 마저 이동하고 본 Task 8 내에서 함께 커밋.

- [ ] **Step 2: `main.h` 상태 점검**

  ```bash
  wc -l Core/Inc/main.h
  cat Core/Inc/main.h | head -80
  ```

  Expected 구조 (대략):
  - 헤더 가드
  - `extern "C"` 블록
  - STM LL includes
  - 기본 typedef (u8/s8/u16/s16/u32/s32 + volatile/const 변형)
  - U8_MAX 등 기본 매크로
  - 빌드 플래그: BOARD_B (+ 주석 플래그들)
  - 공용 상수: ACTIVE_LOW/INACTIVE_HIGH, HIGH_ACTIVE/LOW_INACTIVE, MODEL_HA/HM, LEFT_CHAIR/RIGHT_CHAIR/ROBOT, HM_LEFT_CHAIR/HM_RIGHT_CHAIR
  - 도메인 umbrella includes (pinmap/system/sensor/motor/comm/storage/mode)
  - `BdcSystem_InitTypeDef` + `extern BdcSystem;`

- [ ] **Step 3: stm32f1xx_it.c 최종 검사**

  ```bash
  wc -l Core/Src/stm32f1xx_it.c
  grep -n '^extern ' Core/Src/stm32f1xx_it.c
  ```

  Expected: ~200줄, extern 0줄.

- [ ] **Step 4: 경고 없이 빌드**

  STM32CubeIDE Debug + Release 양쪽 모두 빌드 → 에러 0, warning 증가 없음.

- [ ] **Step 5: 전수 실기 검증 (사용자)**

  베이스라인 Task 0과 동일한 절차를 **전량 재수행**하여 `baseline/post_refactor_smoke.md` 작성:
  - SP 모드 30초 로그 + 베이스라인 diff
  - 10개 모터 동작 수동 + 리밋 + 타임아웃
  - UART 8개 CMD 왕복
  - EEPROM 홈 위치 저장/복원
  - TIM7 ISR 주기 (오실로스코프로 1 kHz 확인)
  - CpuRun LED 500 ms on/off

- [ ] **Step 6: 최종 Commit**

  ```bash
  git add Core/Src/main.c Core/Src/stm32f1xx_it.c Core/Inc/main.h
  git commit -m "chore(main): finalize main.c cleanup after module decomposition

  Remove dead extern declarations and stale comments introduced by
  the extraction process. main.c is now a boot + main-loop file
  (<300 lines); stm32f1xx_it.c is an ISR dispatch file (~200 lines);
  main.h is an umbrella with shared types, build flags, and domain
  header includes. No behavior change verified end-to-end against
  the pre-refactor baseline."
  ```

- [ ] **Step 7: worklog.md 최종 업데이트**

  "리팩토링 완료 2026-04-23" 섹션 작성 (또는 해당 작업일 날짜). 각 모듈별 최종 줄 수 + 베이스라인 대비 `.bin` 크기 변동 요약 + 실기 전수 검증 결과.

---

## Post-Refactor Verification Checklist

모든 커밋 완료 후 다음 최종 체크 (Task 8 Step 5와 별도 최종 점검):

- [ ] `wc -l Core/Src/main.c` ≤ 300
- [ ] `wc -l Core/Src/stm32f1xx_it.c` ≤ 250
- [ ] `wc -l Core/Inc/main.h` ≤ 200
- [ ] 각 `Core/Src/hb_*.c` ≤ 700
- [ ] 각 `Core/Inc/hb_*.h` ≤ 150
- [ ] `grep -n '^extern ' Core/Src/*.c` 결과 모든 파일에서 0
- [ ] `git log --oneline` 커밋 개수 9개 (베이스라인 + 리팩토링 8개)
- [ ] Debug·Release 빌드 모두 에러·warning 증가 없음
- [ ] 베이스라인 기능 스모크 전량 재현 성공
- [ ] SP 모드 로그 센서값 ±1mm 이내
- [ ] UART 응답 바이트 단위 동등
- [ ] EEPROM 홈 위치 저장·복원 동등
- [ ] TIM7 ISR 주기 1kHz ±0.1%
- [ ] `.bin` 크기 베이스라인 대비 ±2% 이내

---

## 참고

- 설계서: `docs/superpowers/specs/2026-04-23-module-decomposition-design.md`
- 인수인계: `HANDOVER.md`
- 베이스라인 커밋: `59db561`
- 주의: HANDOVER §17의 알려진 이슈(`DRV3_IN3/IN4` 오참조 등)는 **본 리팩토링에서 수정하지 않는다**. 발견 시 `worklog.md`의 "추후 검토" 섹션에 기록만 함.

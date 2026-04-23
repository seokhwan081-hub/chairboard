# 설계 문서: 원점 설정, 센서 필터링, 핀맵 분기

**날짜**: 2026-04-16 (최종 업데이트)  
**파일**: `Core/Src/main.c`, `Core/Src/stm32f1xx_it.c`, `Core/Inc/main.h`

---

## 1. 의자 전후방 / 로봇 높이 — 센서 기반 원점 설정

### 배경

기존에는 홈포지션 설정 시 지정된 좌표값(의자 전방 75, 로봇 하강 90)으로 이동한 뒤 그 위치를 원점으로 저장했다.  
변경 후에는 후면/하강 리밋 포토센서가 감지되는 지점까지 이동 후 멈추고, 그 위치를 원점으로 저장한다.

적용 축: **의자 전후방(CHAIR_FRONT_BACK)**, **로봇 높이(ROBOT)**

### 좌표계

| 축 | 센서 위치 | 레이저 방향 | 새 홈 위치 | 타겟 오프셋 의미 |
|---|---|---|---|---|
| 의자 전후방 | 전면 설치, 후면까지 거리 측정 | 전방이동 = 값 감소, 후방이동 = 값 증가 | 후면 리밋 (큰 값) | "홈에서 전방으로 얼마나" |
| 로봇 높이 | 하부 기준 높이 측정 | 상승 = 값 증가, 하강 = 값 감소 | 하강 리밋 (작은 값) | "홈에서 위로 얼마나" |

### 변경 1-1: 홈설정 이동 방식

`case UART_SMART_DEVCIE_REQUEST_GO_HOME_POSITION` 내부 3곳.

| 위치 | 기존 | 변경 후 | 정지 조건 |
|---|---|---|---|
| main.c:1583 (LEFT_CHAIR) | `LeftChairGoFront(1, 75)` | `LeftChairGoBack(1, 65535)` | `LIMIT_CHAIR1_BACK_CHECK == ACTIVE_LOW` |
| main.c:1620 (RIGHT_CHAIR) | `RightChairGoFront(1, 75)` | `RightChairGoBack(1, 65535)` | `LIMIT_CHAIR2_BACK_CHECK == ACTIVE_LOW` |
| main.c:1657 (ROBOT) | `RobotGoDown(1, 90)` | `RobotGoDown(1, 0)` | `LIMIT_ROBOT_DW_CHECK == ACTIVE_LOW` |

`65535`는 u16 최대값으로, 현실적으로 도달 불가능한 더미 타겟. 오직 센서만이 정지 조건.  
`0`은 로봇 레이저값이 0 이하로 내려갈 수 없으므로 동일하게 센서만이 정지 조건.

동작 흐름:
```
커맨드 수신
→ GoBack(1, 65535) / RobotGoDown(1, 0) 호출
→ 모터 구동 시작
→ 리밋 센서 감지(ACTIVE_LOW) → break
→ return 0 (time_over = 0, 성공)
→ CurrentPosition → HomePosition → EEPROM 저장
→ UartSmartDeviceRequestGoHomePositionResponse 응답
```

센서 미감지 시 타임아웃(`FWD_BACK_LIMIT_TIME` / `ROBOT_UP_DOWN_LIMIT_TIME`) 후 `time_over = 1` → `UartPICSendErrorCode` 기존 에러 처리 유지.

### 변경 1-2: 타겟포지션 수식 (의자 전후방만)

의자 홈이 후면(큰 레이저값)으로 바뀌었으므로, 오프셋 방향이 반전된다.

**기존**: `ActualTarget = Home + Offset` (홈에서 후방으로 offset)  
**변경**: `ActualTarget = Home - Offset` (홈에서 전방으로 offset)

변경 대상 4곳:

| 라인 | 케이스 | 변수 |
|---|---|---|
| main.c:1773 | LEFT_CHAIR | `LaserChair1Front` |
| main.c:1859 | RIGHT_CHAIR | `LaserChair2Front` |
| main.c:1931 | HM_LEFT_CHAIR | `LaserChair1Front` |
| main.c:1965 | HM_RIGHT_CHAIR | `LaserChair2Front` |

```c
// 변경 전
LaserChair1Front.TargetPosition = LaserChair1Front.TargetPosition + LaserChair1Front.HomePosition;

// 변경 후
LaserChair1Front.TargetPosition = LaserChair1Front.HomePosition - LaserChair1Front.TargetPosition;
```

**로봇(`LaserRobotUp`)은 변경 없음**: 홈이 하단(작은 값), 오프셋이 상승 방향(값 증가) → 덧셈 유지.

이동 방향 판단 로직(`< CurrentPosition` → GoFront, `> CurrentPosition` → GoBack)은 수식 변경 후에도 자동으로 올바르게 동작하므로 수정 불필요.

---

## 2. 의자 높이(UP/DOWN) — 안정화 기반 원점 설정

### 배경

기존에는 의자 높이 원점 설정 시 `POTENTIO_CHAIR1_HOME_POSITION(81)` 고정 임계값을 기준으로 사용했다.  
포텐셔미터가 제품마다 균일하지 않아, 최솟값이 81 이상인 제품에서는 조건 미충족으로 타임아웃이 발생했다.

변경 후에는 고정 임계값 없이, 의자가 **실제로 하강한 후** 포텐셔미터 값이 **±1 범위에서 3초간 안정화**되면 해당 값을 원점으로 저장한다.

적용 함수: `LeftChairGoDown`, `RightChairGoDown` 내 `TargetPos == 0` 케이스

### 기존 동작

```
하강 중 → CurrentPosition < 81(고정 임계값) → 5초 타이머 시작 → 타이머 만료 시 정지
문제: 포텐셔미터 최솟값이 81 이상이면 조건 진입 불가 → 타임아웃
```

### 중간 버전 (단순 3초 안정화 — 오판 문제 발견)

```
하강 시작 (lastPosition = 현재값)
→ 값 변화 없음 → 3000ms 타이머 카운트다운
→ 값 변화 있음 → lastPosition 갱신, 타이머 리셋
문제:
  - 모터 반응 지연 + ADC 필터 지연으로 시작 직후 CurrentPosition==lastPosition
    → 3초 카운트다운 후 시작점을 홈으로 오판
  - 하강 도중 정체 구간(관성, ADC 평탄화 등)을 최저점으로 오인
  - 포텐셔미터 ±1 노이즈로 3초 유지 불가 → 타임아웃
```

### 최종 버전 (2단계 검사 + "이미 최하점" 분기)

```
[1단계] 하강 확인 OR "이미 최하점" 판정
  startPosition 기록 → 모터 구동
  분기 A (하강 감지):
    CurrentPosition이 startPosition보다 2 이상 감소 → descentConfirmed = 1 → [2단계]로
  분기 B (이미 최하점):
    |CurrentPosition - startPosition| <= 1 에서 3000ms 유지
      → 타이머 만료 시 break → 현재 위치를 홈으로 저장
    벗어나면 타이머 리셋

[2단계] 안정화 검사 (descentConfirmed == 1 일 때만)
  |CurrentPosition - lastPosition| <= 1 (±1 허용)
    → 타이머 3000ms 카운트다운
    → 3초 유지 시 break → 해당 값을 홈으로 저장
  변화 > 1 이면 lastPosition 갱신 + 타이머 리셋

UP_DOWN_LIMIT_TIME 타임아웃은 안전장치로 유지
```

### 엣지 케이스: 의자가 이미 최하점에 있는 경우

**증상**: 의자가 이미 최하점에 있는 상태에서 원점 조정 요청 시, 홈 설정이 안되고 타임아웃 에러 발생.

**원인**: 이미 최하점이라 모터 구동해도 `CurrentPosition`이 변하지 않음 → `descentConfirmed`가 영원히 0 → 2단계 안정화 검사 진입 불가 → `UP_DOWN_LIMIT_TIME` 타임아웃.

**해결**: 1단계에서 하강 감지와 병행하여 "이미 최하점" 판정 로직 추가. `startPosition ±1` 범위에서 3초간 머무르면 현재 위치를 홈으로 저장하고 break.

### 변경 내용

| 함수 | 위치 | 내용 |
|---|---|---|
| `LeftChairGoDown` | main.c:920 | `startPosition`, `descentConfirmed`, `diff` 변수 추가. 하강 확인 2 이상, 안정화 ±1 오차 3초 |
| `RightChairGoDown` | main.c:1191 | 동일 패턴 적용 |

추가 변수:
- `startPosition` — 함수 진입 시점의 포텐셔미터 값
- `descentConfirmed` — 실제 하강(2 이상) 확인 플래그
- `diff` (s16) — 언더플로우 방지용 부호 있는 차이값

### 임계값 선택 이유

- **하강 확인 = 2**: ±1 노이즈와 확실히 구분되는 최소값 (1은 노이즈일 가능성)
- **허용 오차 ±1**: 포텐셔미터 일반적 노이즈 수준

### 관련 이슈: 진입 가드로 인한 조기 종료

**증상**: 의자가 리밋 스위치(`LIMIT_CHAIR1_DW_CHECK` / `LIMIT_CHAIR2_DW_CHECK`) 활성 위치(약 90 근처) 이하에서 원점 조정 시, 더 내려갈 수 있음에도 현재 위치가 홈으로 저장됨.

**원인**: [main.c:927](Core/Src/main.c#L927), [main.c:1197](Core/Src/main.c#L1197) 의 진입 가드
```c
if((LIMIT_CHAIR1_DW_CHECK == ACTIVE_LOW) && (g_is_homing == 0))
    return 0;  // 모터 구동 전 즉시 성공 반환
```
리턴값 0(성공) → 호출자가 `CurrentPosition`을 `HomePosition`에 저장.

**해결 방안**: `TargetPos == 0` (홈 설정)일 때는 진입 가드 우회하도록 조건 추가:
```c
if((LIMIT_CHAIR1_DW_CHECK == ACTIVE_LOW) && (g_is_homing == 0) && (TargetPos != 0))
    return 0;
```
또는 홈 설정 호출 전후로 `g_is_homing = 1/0` 관리.

---

## 3. 로봇 높이 ADC — 미디안 5샘플 필터

### 배경

로봇 높이 센서의 ADC 값이 필터 없이 raw로 사용되고 있어, 스파이크 발생 시 위치값이 크게 튀어 목표 위치 전에 멈추는 현상이 발생했다.

적용 파일: `Core/Src/stm32f1xx_it.c`

### 기존 동작

```c
// raw ADC 직접 사용 (필터 없음)
ADConverter.RobotHeightSensor = internal_adc_value[8];
LaserRobotUp.voltage = (((u32)ADConverter.RobotHeightSensor * 3300) >> 12);
LaserRobotUp.FindPosition = ...;
LaserRobotUp.CurrentPosition = (u16)LaserRobotUp.FindPosition;
```

### 변경 후 동작

기존 raw 코드를 주석 처리(`#if 0`)하고, 5샘플 미디안 필터를 적용:

```c
// 5샘플 수집 → 버블 정렬 → 중앙값(tmp[2]) 추출 → voltage/position 변환
RobotHeightAvg.AdcBuf[RobotHeightAvg.Cnt] = internal_adc_value[8];
RobotHeightAvg.Cnt++;
if(RobotHeightAvg.Cnt >= 5){
    // 버블 정렬 후 중앙값 사용
    ADConverter.RobotHeightSensor = tmp[2]; // 미디안
    LaserRobotUp.voltage = (((u32)ADConverter.RobotHeightSensor * 3300) >> 12);
    LaserRobotUp.FindPosition = ...;
    LaserRobotUp.CurrentPosition = (u16)LaserRobotUp.FindPosition;
}
```

### 추가 수정

| 위치 | 기존 | 변경 | 이유 |
|---|---|---|---|
| stm32f1xx_it.c:432 | `AdcSum/2` | `AdcSum/10` | 10샘플 평균이므로 /10이 올바름 (사용자가 직접 수정) |
| stm32f1xx_it.c:434 | `>> 10` | `>> 12` | 12비트 ADC 정규화는 /4096(>>12)이 올바름, 의자 센서와 동일하게 통일 |

---

## 4. Board A/B 핀맵 분기 (`#ifdef BOARD_B`)

### 배경

STM32F105 보드가 V3.0(Board A)에서 V3.1(Board B)로 변경되면서 일부 핀맵이 달라졌다.  
`#ifdef BOARD_B` 전처리 분기를 사용하여 동일 소스코드에서 양쪽 보드를 지원한다.

### 변경된 핀맵

| 신호 | Board A (기존) | Board B (변경) |
|---|---|---|
| I2C_SDA (EEPROM) | PB12 | PE14 |
| I2C_SCL (EEPROM) | PB11 | PE15 |
| SW_CHAIR1_DW | PA11 | PC12 |
| SW_CHAIR2_FWD | PE14 | PA11 |
| SW_CHAIR2_BACK | PE15 | PA10 |
| SW_CHAIR2_UP | PB10 | PA9 |
| SW_CHAIR2_DW | PB13 | PA8 |
| SW_ROBOT_UP | PA9 | PC9 |
| SW_ROBOT_DW | PA8 | PC8 |

### 변경 파일 및 위치

#### main.h — Pin 정의 분기

| 항목 | 내용 |
|---|---|
| `I2C_SCL_Pin/Port`, `I2C_SDA_Pin/Port` | GPIOB→GPIOE |
| `SW_CHAIR1_DW_Pin/Port` | GPIOA→GPIOC |
| `SW_CHAIR2_FWD/BACK/UP_Pin/Port` | GPIOE,GPIOB→GPIOA |
| `SW_CHAIR2_DW_Pin/Port` | GPIOB→GPIOA |
| `SW_ROBOT_UP/DW_Pin/Port` | GPIOA→GPIOC |

#### main.h — CHECK 매크로 분기

GPIO 포트가 하드코딩되어 있으므로 별도 `#ifdef BOARD_B` 분기 필요:

| 매크로 | Board A 포트 | Board B 포트 |
|---|---|---|
| `SW_CHAIR2_FWD_CHECK` | GPIOE | GPIOA |
| `SW_CHAIR2_BACK_CHECK` | GPIOE | GPIOA |
| `SW_CHAIR2_UP_CHECK` | GPIOB | GPIOA |
| `SW_CHAIR1_DW_CHECK` (non-RMC_BLACK) | GPIOA | GPIOC |
| `SW_CHAIR2_DW_CHECK` (non-RMC_BLACK) | GPIOB | GPIOA |
| `SW_ROBOT_UP_CHECK` | GPIOA | GPIOC |
| `SW_ROBOT_DW_CHECK` | GPIOA | GPIOC |

#### main.h — I2C 매크로 분기

`I2C_SCL_HIGH/LOW`, `I2C_SDA_HIGH/LOW/READ` — GPIOB→GPIOE

#### main.c — GPIO 하드코딩 수정 6곳

| # | 위치 | 내용 |
|---|---|---|
| 1 | main.c MX_GPIO_Init 내 ResetOutputPin | `I2C_SCL_Pin`을 GPIOB에서 분리, Board B는 GPIOE로 |
| 2 | main.c 초기화 SetOutputPin | `GPIOB` → `I2C_SCL_GPIO_Port` / `I2C_SDA_GPIO_Port` 매크로 사용 |
| 3 | main.c MX_GPIO_Init SW_CHAIR2 | `GPIOE`/`GPIOB` → Board B는 `GPIOA`로 통합 |
| 4 | main.c MX_GPIO_Init I2C | `GPIOB` → `I2C_SCL_GPIO_Port` 매크로 사용 |
| 5 | main.c MX_GPIO_Init SW_ROBOT/CHAIR1_DW | `GPIOA` → Board B는 GPIOC/GPIOC로 분리 |
| 6 | main.c I2C Mode 함수 3개 | `LL_GPIO_Init(GPIOB, ...)` → `LL_GPIO_Init(I2C_xxx_GPIO_Port, ...)` |

### 빌드 방법

- **Board A (V3.0)**: `BOARD_B` 미정의 (기존 동작)
- **Board B (V3.1)**: `#define BOARD_B` 추가 (main.h 상단 또는 프로젝트 Preprocessor 설정)

현재 main.h:164에 `#define BOARD_B` 가 직접 정의되어 있음. Board A로 전환 시 해당 줄을 주석 처리.

---

## 변경되지 않는 것

- EEPROM 저장 로직 (HomePosition → HomePosi[] → EEPROM_Write)
- 에러 처리 (time_over == 1 → UartPICSendErrorCode)
- 타임아웃 안전장치 (`FWD_BACK_LIMIT_TIME`, `UP_DOWN_LIMIT_TIME`, `ROBOT_UP_DOWN_LIMIT_TIME`)
- LaserRobotUp 타겟 수식 (덧셈 유지)
- 이동 방향 판단 조건문
- 의자 전후방 / 높이 ADC 필터링 (기존 10샘플 SMA 유지)
- Board A/B 공통 핀 (모터 드라이버, 리밋 스위치, ADC 센서 등)

---

## 후속 작업 권고사항 (미구현)

- **u16 언더플로우 방어**: 의자 타겟 수식 `HomePosition - Offset`에서 Offset > HomePosition 시 입력값 검증 추가
- **printf 레이블 수정**: `"Target + home"` → 실제 동작 반영하는 문구로 변경
- **CurrentPosition 전송 수식**: `UartPICSendLengthDataAfterManualAdjustment`, `UartSmartDeviceRequestCurrentPositionResponse` 내 diff 계산이 구 좌표계 기준 → 의자 전후방(`diff[0]`, `diff[2]`)은 `Home - Current`로 반전 필요

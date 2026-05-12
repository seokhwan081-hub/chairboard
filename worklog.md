# ChairControl3.1 Worklog

> 프로젝트 작업 내역을 날짜별로 누적 기록한다. 새 항목은 상단(최신 → 과거) 순서로 추가한다.

---

## 2026-05-12

### 리프트 칼럼 높이 센서 ADC 변환 수식 비선형 보정

- **문제 파악**: `stm32f1xx_it.c` TIM7 ISR 내 Chair1/Chair2 높이 포텐셔미터 변환식(`/6.7`, `/2.5`)이 선형 가정 기반으로, 실측 결과 비선형(지수 포화) 특성과 불일치함을 확인.
- **실측 데이터 분석**: `5.8 리프트 칼럼 측정.xlsx` (LC-8/9/10/16 4개 유닛 평균) 검토.
  - 높이 범위: 0~10cm(0~100mm), 전압 범위: 0.95V~2.82V
  - 엑셀 제시 모델: 지수 포화 `V = 1.8881×(1-e^(-0.3550×h)) + 0.9569` (R²=0.9987, RMSE=0.021V)
  - 역방향: `h = -ln(1-(V-0.9569)/1.8881) / 0.3550`
- **구현 방식 결정**: STM32F1(FPU 없음) 특성상 `logf()` 대신 **LUT + 구간 선형 보간** 채택.
  - 10mm 단위 11개 기준점 → 구간 내 `(off×10)/span` 정수 보간으로 1mm 분해능 확보
  - ADC 범위 초과(노이즈 포함) 시 클램핑 처리
- **코드 변경 내용** (`Core/Src/stm32f1xx_it.c`):
  - `kLiftColAdcLut[11]` 추가 (USER CODE BEGIN PV): ADC_10bit 기준점 배열 `{295, 469, 604, 688, 738, 776, 804, 828, 849, 863, 875}`
  - `LiftColAdcToMm(u16 adc)` 추가 (USER CODE BEGIN 0): 보간 변환 함수
  - Chair1 변환식 교체: `(float)ADConverter.Chair1HeightPot/6.7` → `LiftColAdcToMm(ADConverter.Chair1HeightPot)`
  - Chair2 변환식 교체: `(float)ADConverter.Chair2HeightPot/2.5` → `LiftColAdcToMm(ADConverter.Chair2HeightPot)`
  - 높이 범위 주석 `//0~80mm` → `//0~100mm` 업데이트
- **후속 검토 권고**: HWTest 분기(`stm32f1xx_it.c` line 528, 534)의 높이 임계값(`>170`, `<78`)이 이전 스케일 기준이므로 실기 검증 후 수정 필요.

---

## 2026-04-27

### 리팩토링 구현 플랜 작성 + Self-review 보강

- **베이스라인 커밋 생성 완료**: `59db561 chore: baseline snapshot before module decomposition refactor` (94 files, 80,528 insertions). git 저장소 로컬 초기화. `.gitignore`로 `.o/.d/.su/.list/.map/makefile` 제외, `.bin/.elf/.hex` 추적.
- **사용자 git identity (repo-local)**: `Seokhwan Hong <seokhwan081@gmail.com>` 설정 (`git config` repo-scope).
- **구현 플랜 문서 작성 완료**: `docs/superpowers/plans/2026-04-23-module-decomposition.md`
  - 9개 Task: Pre-Work(베이스라인 캡처) + Task 1~8(모듈 추출 7개 + 정리)
  - 각 Task에 Files / Steps / 빌드·실기 검증 / Commit 포함
  - 총 ~700줄
- **Self-review 추가 보강 (4개 누락 발견)**:
  1. Task 2: `main.h:364~376`의 I2C_SCL/SDA HIGH/LOW/READ 매크로(BOARD_B 분기) 이동 누락 → `hb_storage.c` 파일-private 매크로로 이동 명시 추가
  2. Task 6: `__io_putchar` 함수(main.c:2817 `PUTCHAR_PROTOTYPE` 블록) 이전 + `main.h:357~363`의 `PUTCHAR_PROTOTYPE` 매크로 제거 단계 추가 (printf USART2 리다이렉트는 hb_comm 소속)
  3. Task 7: `stm32f1xx_it.c:364, 501`의 두 `#ifdef HW_TEST` 블록 → `hb_mode_tick_1ms` 본체로 이전 단계 추가
  4. Task 8: "main.c에 main() 외 함수 잔존 없음" 확인용 grep 명령 추가
- **다음 단계**: 사용자 플랜 검토 → 승인 후 베이스라인 캡처(Pre-Work Task 0) → Task 1(pinmap.h) 착수
- **실행 모드 선택 대기**: subagent-driven (per-task 분리 실행) vs inline (이 세션에서 순차 실행)
- **2026-04-27 PAUSE**: 사용자 요청으로 일시 중단. 기획/설계/플랜 모두 확정 상태에서 정지. 다음 세션 재개 시 Pre-Work Task 0(베이스라인 실기 캡처) 또는 Task 1부터 진입. 결정사항·산출물 모두 기록 완료, 재합의 불필요.

---

## 2026-04-23

### 리팩토링 브레인스토밍 진행

- **목표 확정**: 리팩토링 1차 목적은 **B. 유지보수성·가독성** 단일로 한정
  - 범위 제외: A(신규 기능 용이성/OCP), C(테스트 가능성/DIP), D(알려진 버그 정리), E(보드 분기 리팩)
  - 원칙: **동작 보존 (behavior-preserving) 순수 파일 분리**
  - 회귀 검증: 실기 테스트 즉시 가능
- **분리 세분성 확정**: 옵션 ① **Coarse (7-file split)**
  - 기존 `main.c` (2889줄) + `main.h` (655줄) + `stm32f1xx_it.c` (591줄)을 7개 도메인 모듈 + umbrella 헤더 구조로 분해
  - STM32CubeIDE 프로젝트 파일(`.cproject`)·링커 스크립트 변경 없음 목표
  - 서브폴더/캡슐화는 도입하지 않음 (옵션 ②/③ 보류)
- **예정 모듈 맵** (변경 시 본 항목 업데이트)
  - `hb_system.{c,h}` — 시스템 클럭/GPIO/TIM7/UART/ADC 초기화
  - `hb_motor.{c,h}` — 10개 모터 구동 함수 + `BDCMotor[]`
  - `hb_sensor.{c,h}` — Laser/Potentio 변환식 + 이동평균/미디안 필터
  - `hb_comm.{c,h}` — UART 프레임 파싱 + `UartProcess` + 응답 송출
  - `hb_storage.{c,h}` — I2C bit-bang + `EEPROM_*`
  - `hb_mode.{c,h}` — `SPMode` + `HW_TEST`
  - `pinmap.h` — 모든 GPIO 핀 매핑
  - `main.h` — umbrella header로 유지 (기존 include 체인 보존)
- **사용 스킬/프로세스**: superpowers `brainstorming` → design spec → `writing-plans`(또는 `bkit:plan-plus`/`bkit:pdca`) 순
- **검증 수준 확정**: 옵션 D (스모크 + SP 모드 USART2 로그 before/after + `.bin`/`.map` 크기 감시, 중요 커밋은 오실로스코프 TIM7 ISR 시간 측정)
- **추출 순서 확정**: 순서 A (저위험→고위험 bottom-up, 8 커밋)
  1. `pinmap.h` → 2. `hb_storage` → 3. `hb_mode` → 4. `hb_sensor` → 5. `hb_motor` → 6. `hb_comm` → 7. `hb_system` → 8. 정리
- **ISR 경계 확정**: `stm32f1xx_it.c` ~591줄 → ~200줄. `TIM7_IRQHandler`는 `hb_system_heartbeat_1ms` → `hb_sensor_tick_1ms` → `hb_motor_tick_1ms` → `hb_mode_tick_1ms` → `Timer7Counter++` 순서로 4분할 위임. `UART4_IRQHandler` 본체는 `hb_comm_on_uart4_isr()` 로 통째 이동.
- **버전 관리**: 현재 git 저장소 아님. `git init` 로컬 저장소 채택. `.gitignore`로 `Debug/Release` 산출물 중 오브젝트/의존성(`.o`/`.d`/`.su`)만 제외하고 `.bin`/`.elf`/`.hex`는 추적(버전별 바이너리 diff용)
- **설계 스펙 문서 작성 완료**: `docs/superpowers/specs/2026-04-23-module-decomposition-design.md`
- **Self-review 수정 사항**:
  - `nop_op(void)`·`delay(void)`는 I2C 전용 함수임을 grep으로 확인 → `hb_storage.c` 내부 `static`으로 이관 (기존 초안에서 `hb_system.h` 공개로 잘못 분류했던 것 정정)
  - `UartProcess()` 는 `MasterComm1()`만 호출 확인(main.c:1650) → `hb_comm.c` static 격리 확정. 현 `main.h:513`의 prototype 제거 동반 필요 명기
  - 커밋 8 (정리) 범위를 "함수 본체·동작 로직 건드리지 않음, 중복 `extern`과 죽은 주석만 제거"로 명시화
- **다음 단계**: 사용자 스펙 검토 → 승인 후 `git init` + 베이스라인 캡처 → 구현 계획(writing-plans 또는 bkit:plan-plus) → 커밋 1 착수

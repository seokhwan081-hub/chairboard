# ChairControl3.1 Worklog

> 프로젝트 작업 내역을 날짜별로 누적 기록한다. 새 항목은 상단(최신 → 과거) 순서로 추가한다.

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

# Homing Sensor Origin 구현 플랜

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 홈포지션 설정 시 지정 좌표 이동 대신, 후면/하강 리밋 포토센서가 감지되는 지점을 원점으로 설정한다.

**Architecture:** `UART_SMART_DEVCIE_REQUEST_GO_HOME_POSITION` 커맨드 핸들러에서 GoFront→GoBack, RobotGoDown 타겟값을 변경하고, 타겟포지션 수식을 `Home + Offset` → `Home - Offset`으로 반전한다. 로봇 높이 수식은 좌표 방향이 의자와 반대이므로 변경하지 않는다.

**Tech Stack:** STM32 embedded C, STM32CubeIDE, UART 프로토콜

---

## 파일 구조

| 파일 | 변경 내용 |
|---|---|
| `Core/Src/main.c` | 홈설정 이동 함수 3곳 + 타겟포지션 수식 4곳 변경 |

---

### Task 1: 홈설정 이동 방식 변경 (LEFT_CHAIR)

**Files:**
- Modify: `Core/Src/main.c:1583`

- [ ] **Step 1: 변경 전 동작 확인**

  `main.c:1583` 주변 코드 확인:
  ```c
  if(BdcSystem.MotorName == CHAIR_FRONT_BACK){
      if(LeftChairGoFront(1, 75)){   // ← 이 줄
          UartPICSendErrorCode(BdcSystem.ChairName, 4, 0);
      }
  ```

- [ ] **Step 2: LeftChairGoFront → LeftChairGoBack으로 변경**

  `main.c:1583`을 다음으로 수정:
  ```c
  if(LeftChairGoBack(1, 65535)){
  ```

  > `65535`(u16 최대값)는 현실적으로 도달 불가능한 더미 타겟.  
  > `LIMIT_CHAIR1_BACK_CHECK == ACTIVE_LOW` 센서 감지만이 실제 정지 조건.

- [ ] **Step 3: 빌드 확인**

  STM32CubeIDE에서 빌드 실행 → 에러 없음 확인.

- [ ] **Step 4: Commit**

  ```
  git add Core/Src/main.c
  git commit -m "fix: left chair homing to rear limit sensor instead of fixed position"
  ```

---

### Task 2: 홈설정 이동 방식 변경 (RIGHT_CHAIR)

**Files:**
- Modify: `Core/Src/main.c:1620`

- [ ] **Step 1: 변경 전 동작 확인**

  `main.c:1620` 주변 코드 확인:
  ```c
  if(BdcSystem.MotorName == CHAIR_FRONT_BACK){
      if(RightChairGoFront(1, 75)){   // ← 이 줄
          UartPICSendErrorCode(RIGHT_CHAIR, 4, 0);
      }
  ```

- [ ] **Step 2: RightChairGoFront → RightChairGoBack으로 변경**

  `main.c:1620`을 다음으로 수정:
  ```c
  if(RightChairGoBack(1, 65535)){
  ```

  > `LIMIT_CHAIR2_BACK_CHECK == ACTIVE_LOW` 감지 시 정지.

- [ ] **Step 3: 빌드 확인**

  STM32CubeIDE에서 빌드 실행 → 에러 없음 확인.

- [ ] **Step 4: Commit**

  ```
  git add Core/Src/main.c
  git commit -m "fix: right chair homing to rear limit sensor instead of fixed position"
  ```

---

### Task 3: 홈설정 이동 방식 변경 (ROBOT)

**Files:**
- Modify: `Core/Src/main.c:1657`

- [ ] **Step 1: 변경 전 동작 확인**

  `main.c:1657` 주변 코드 확인:
  ```c
  else if(BdcSystem.ChairName == ROBOT){
      if(RobotGoDown(1, 90)){   // ← 이 줄
          UartPICSendErrorCode(LEFT_CHAIR, 4, 0);
      }
  ```

- [ ] **Step 2: TargetPos 90 → 0으로 변경**

  `main.c:1657`을 다음으로 수정:
  ```c
  if(RobotGoDown(1, 0)){
  ```

  > `LaserRobotUp.CurrentPosition <= 0`은 현실적으로 불가 → `LIMIT_ROBOT_DW_CHECK == ACTIVE_LOW`만이 정지 조건.

- [ ] **Step 3: 빌드 확인**

  STM32CubeIDE에서 빌드 실행 → 에러 없음 확인.

- [ ] **Step 4: Commit**

  ```
  git add Core/Src/main.c
  git commit -m "fix: robot homing to down limit sensor instead of fixed position"
  ```

---

### Task 4: 타겟포지션 수식 변경 — LEFT_CHAIR / RIGHT_CHAIR 케이스

**Files:**
- Modify: `Core/Src/main.c:1773`, `Core/Src/main.c:1859`

배경: 의자 홈이 후면(큰 레이저값)으로 바뀌어 오프셋 방향이 반전됨.  
- 기존: `ActualTarget = Home + Offset` (홈에서 후방으로)  
- 변경: `ActualTarget = Home - Offset` (홈에서 전방으로)

- [ ] **Step 1: main.c:1773 수정 (LEFT_CHAIR)**

  변경 전:
  ```c
  LaserChair1Front.TargetPosition = LaserChair1Front.TargetPosition + LaserChair1Front.HomePosition;
  ```
  변경 후:
  ```c
  LaserChair1Front.TargetPosition = LaserChair1Front.HomePosition - LaserChair1Front.TargetPosition;
  ```

- [ ] **Step 2: main.c:1859 수정 (RIGHT_CHAIR)**

  변경 전:
  ```c
  LaserChair2Front.TargetPosition = LaserChair2Front.TargetPosition + LaserChair2Front.HomePosition;
  ```
  변경 후:
  ```c
  LaserChair2Front.TargetPosition = LaserChair2Front.HomePosition - LaserChair2Front.TargetPosition;
  ```

- [ ] **Step 3: 빌드 확인**

  STM32CubeIDE에서 빌드 실행 → 에러 없음 확인.

- [ ] **Step 4: Commit**

  ```
  git add Core/Src/main.c
  git commit -m "fix: invert chair target position formula for rear-based home origin"
  ```

---

### Task 5: 타겟포지션 수식 변경 — HM_LEFT_CHAIR / HM_RIGHT_CHAIR 케이스

**Files:**
- Modify: `Core/Src/main.c:1931`, `Core/Src/main.c:1965`

- [ ] **Step 1: main.c:1931 수정 (HM_LEFT_CHAIR)**

  변경 전:
  ```c
  LaserChair1Front.TargetPosition = LaserChair1Front.TargetPosition + LaserChair1Front.HomePosition;
  ```
  변경 후:
  ```c
  LaserChair1Front.TargetPosition = LaserChair1Front.HomePosition - LaserChair1Front.TargetPosition;
  ```

- [ ] **Step 2: main.c:1965 수정 (HM_RIGHT_CHAIR)**

  변경 전:
  ```c
  LaserChair2Front.TargetPosition = LaserChair2Front.TargetPosition + LaserChair2Front.HomePosition;
  ```
  변경 후:
  ```c
  LaserChair2Front.TargetPosition = LaserChair2Front.HomePosition - LaserChair2Front.TargetPosition;
  ```

- [ ] **Step 3: 빌드 확인**

  STM32CubeIDE에서 빌드 실행 → 에러 없음 확인.

- [ ] **Step 4: Commit**

  ```
  git add Core/Src/main.c
  git commit -m "fix: invert HM_CHAIR target position formula for rear-based home origin"
  ```

---

### Task 6: 통합 동작 검증

- [ ] **Step 1: 홈설정 시퀀스 검증 (의자)**

  1. 홈설정 커맨드(`UART_SMART_DEVCIE_REQUEST_GO_HOME_POSITION`) 전송 — LEFT_CHAIR, CHAIR_FRONT_BACK
  2. 의자가 후방으로 이동하는지 확인
  3. 후면 포토센서 감지 시 정지하는지 확인
  4. printf 출력에서 저장된 HomePosition 값이 센서 감지 시점의 레이저값인지 확인:
     ```
     Left Chair Back Home Position: <value>
     ```
  5. 동일 검증을 RIGHT_CHAIR로 반복

- [ ] **Step 2: 홈설정 시퀀스 검증 (로봇)**

  1. 홈설정 커맨드 전송 — ROBOT
  2. 로봇이 하강하는지 확인
  3. 하강 리밋 센서 감지 시 정지하는지 확인
  4. printf 출력에서 저장된 HomePosition 확인:
     ```
     Robot Down Home Position: <value>
     ```

- [ ] **Step 3: 타겟이동 검증**

  1. 홈설정 완료 후 타겟 포지션 커맨드 전송 (오프셋 값 전송)
  2. 의자가 홈(후면)에서 전방으로 이동하는지 확인
  3. 오프셋 0 전송 시 의자가 홈(후면)으로 복귀하는지 확인
  4. 로봇은 홈(하단)에서 상승 방향으로 이동하는지 확인

- [ ] **Step 4: 에러 처리 검증**

  센서 연결 해제 상태에서 홈설정 커맨드 전송 → 타임아웃 후 `UartPICSendErrorCode` 호출 확인

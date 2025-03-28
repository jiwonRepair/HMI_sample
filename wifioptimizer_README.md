
# 📘 WiFiOptimizerProject

**실시간 Wi-Fi 신호 품질 시각화, ONNX 기반 예측, FastAPI 통신, 자동 최적화**를 통합한 Qt/QML 프로젝트입니다.

---

## 🚀 주요 기능

| 기능 | 설명 |
|------|------|
| 📶 신호 측정 | OS에 따라 현재 Wi-Fi SSID 및 신호 세기 측정 |
| 📊 그래프 시각화 | 실시간 신호 세기를 QML 그래프로 시각화 |
| 🧠 ONNX 예측 | 최근 신호 이력 기반 품질 예측 (PyTorch→ONNX 모델 사용) |
| ⚙ 자동 최적화 | 예측 결과가 낮으면 자동으로 설정 최적화 수행 |
| 🌐 FastAPI 연동 | REST API로 Wi-Fi 상태 전송, 서버에서 평가 및 피드백 수신 |
| 💾 CSV 저장 | 신호 이력을 `wifi_signal_log.csv`로 저장 |

---

## 🛠️ 기술 스택

- Qt 6 (C++, QML, Qt Charts)
- ONNX Runtime (CPU-only, C API)
- FastAPI (Python 3)
- PyTorch (ONNX 모델 내보내기용)

---

## 📂 파일 구조

```
WiFiOptimizerProject/
├── CMakeLists.txt              # Qt 빌드 설정
├── main.qml                    # UI (그래프, 버튼, 예측 결과 표시)
├── wifioptimizer.h/.cpp        # 핵심 로직 (신호 측정, 예측, API 연동)
├── wifi_predictor.onnx         # ONNX 예측 모델 (10개 신호 입력 → 품질 예측)
├── fastapi_server.py           # FastAPI 서버 (상태 분석 피드백 제공)
└── export_wifi_predictor.py    # PyTorch → ONNX 모델 변환 스크립트
```

---

## ▶️ 실행 방법

### 1. ONNX 모델 생성 (1회)

```bash
python export_wifi_predictor.py
```

### 2. FastAPI 서버 실행

```bash
python fastapi_server.py
# → http://localhost:8000/wifi/update 에서 응답
```

### 3. Qt 프로젝트 빌드 & 실행

```bash
cmake -B build
cmake --build build
./build/WiFiOptimizerProject
```

---

## 📈 모델 구조 (ONNX)

- 입력: `signal_history` — shape: `[1, 10]`
- 출력: `predicted_quality` — shape: `[1]` (0~100)
- 예측 모델은 PyTorch에서 간단한 MLP 구조로 생성됨

---

## ✅ 지원 플랫폼

- Windows (netsh 기반 Wi-Fi 측정)
- Linux (iwconfig 기반)
- macOS (airport 명령 기반)

---

## 📦 외부 라이브러리

| 라이브러리 | 설치 |
|-----------|------|
| `onnxruntime` | CPU-only 라이브러리 필요 (`onnxruntime-lite/include`, `lib`) |
| `fastapi`, `uvicorn` | `pip install fastapi uvicorn` |
| `torch` (PyTorch) | `pip install torch` (ONNX 변환용만 사용) |

---

## 📝 향후 확장 아이디어

- WebSocket으로 FastAPI 실시간 연결
- ONNX 모델을 TFLite로도 병행 지원
- GPU ONNX Runtime 연동
- 신호 지도 기반 자동 이동 최적화

---

## 📜 라이선스

MIT License.



## 📶 하드웨어 없이, 소프트웨어만으로 Wi-Fi 품질을 강화하는 방법 통합 정리 (Windows / Linux / macOS)

| # | 항목 | 설명 | Windows | Linux | macOS | 효과 |
|--|------|------|---------|--------|--------|------|
| 1 | 송신 전력 조절 (`txpower`) | Wi-Fi 모듈의 송신 전력을 높여 실제 전파 강도 증가 (`iwconfig txpower`) | ❌ 미지원 | ✅ 지원 (`iwconfig`, `iw`) | ❌ 미지원 | 🔥 실제 신호 강도 향상 |
| 2 | 규제 국가 변경 (`regdom`) | 더 높은 전송 출력을 허용하는 국가로 전환 (`iw reg set BO`) | ❌ 미지원 | ✅ 지원 (`crda`) | ❌ 미지원 | 🟡 송신 출력 제한 해제 |
| 3 | 전송 속도 고정 (MCS/bitrate) | 낮은 속도로 고정해 끊김 없는 연결 유지 (`iw dev set bitrates`) | ❌ 미지원 | ✅ 지원 | ❌ 미지원 | ✅ 불안정 환경에서 효과적 |
| 4 | QoS/WMM 우선순위 설정 | 멀티미디어 트래픽 우선 처리로 실시간 성능 강화 | ⚠️ 일부 고급 드라이버만 | ✅ `wpa_supplicant`, `tc` 등 | ❌ 불가능 | ✅ 딜레이 최소화 |
| 5 | MTU 조정 | 전송 단위 크기를 줄여 지연/손실 최소화 (`netsh`, `ifconfig`) | ✅ 가능 | ✅ 가능 | ❌ 제한적 | ✅ 체감 속도 향상 |
| 6 | 전원 관리 최적화 | 무선 어댑터 절전모드 해제, 고성능 설정 | ✅ 고급 전원 설정 | ✅ `iw`, `powertop`, `tlp` | ❌ 제한적 | ✅ 연결 안정성 향상 |
| 7 | Wi-Fi 드라이버 최신화 | 칩셋 최적화 및 성능 개선 | ✅ 장치 관리자 | ✅ `modprobe`, `dkms` 등 | ✅ 자동 업데이트 | ✅ 전반적 성능 향상 |
| 8 | DSP/딥러닝 필터링 | 노이즈 제거 → SNR 향상 | ✅ Python | ✅ Python/ONNX/TensorRT | ✅ 가능 | ✅ 체감 품질 향상 |
| 9 | 패킷 복구 최적화 | 손실된 패킷 자동 복원 (FEC 등) | ⚠️ 제한적 | ✅ 가능 (socket 레벨) | ⚠️ 제한적 | ✅ 연결 안정성 강화 |
| 10 | Wi-Fi 진단 유틸 | RSSI, 노이즈 등 측정 도구 | ⚠️ UI만 제공 | ✅ `iwconfig`, `nmcli` 등 | ✅ `airport` CLI | ✅ 수동 튜닝 |
| 11 | FastAPI 품질 API | 외부 API로 Wi-Fi 상태 제공 | ✅ 가능 | ✅ 가능 | ✅ 가능 | ✅ 상태 공유 및 대응 |
| 12 | QML 시각화 | 신호 세기/지연을 UI로 시각화 | ✅ 가능 | ✅ 가능 | ✅ 가능 | ✅ UX 개선 |
| 13 | 채널 폭 설정 | 20/40MHz 고정 → 간섭 최소화 | ⚠️ 일부 UI | ✅ `iw` | ❌ 불가능 | ✅ 안정성 향상 |
| 14 | 딥러닝 예측 기반 제어 | 품질 예측 → 자동 최적화 | ✅ 가능 | ✅ 가능 | ✅ 가능 | ✅ 사전 대응 가능 |
| 15 | 채널 변경 요청 | AP에 특정 채널 요청 | ❌ 불가 | ❌ 불가 | ❌ 불가 | ❌ AP만 가능 |

---

## 🧩 기능별 테스트 메서드 요약

| 순번 | 기능 | 메서드 | 설명 |
|------|------|--------|------|
| 1️⃣ | 📊 신호 이력 저장 | `testGraphUpdate()` | `QVector<int>`에 기록 → 그래프용 |
| 2️⃣ | 🌐 FastAPI 연동 | `testFastApiIntegration()` + `sendToFastApi()` | `http://localhost:8000/wifi/update`로 상태 전송 |
| 3️⃣ | 🧠 ONNX 예측 기반 판단 | `testPredictiveEnhancement()` | 최근 신호로 품질 예측 |
| 4️⃣ | 📝 신호 이력 로그 출력 | `testSignalHistoryLog()` | 전체 이력을 콘솔에 출력 |

---

## 📦 프로젝트 전체 기능 요약 (요청 기반 정리)

WiFiOptimizerProject는 사용자의 요구에 따라 아래와 같은 기능들이 단계별로 구현되었습니다.

### ✅ 1. QML 기반 UI 및 C++ 클래스 구조
- `WifiOptimizer` C++ 클래스 생성
- QML UI에서 Wi-Fi 상태, 예측 결과, 그래프를 실시간 표시
- `Q_PROPERTY`, `Q_INVOKABLE`로 QML 연동 완비

### ✅ 2. Wi-Fi 신호 측정
- Windows: `netsh wlan show interfaces`
- Linux: `iwconfig`
- macOS: `airport -I`
- 결과: `ssid`, `signalStrength` 추출

### ✅ 3. 신호 이력 그래프 구현
- `QVector<int> signalHistory` 저장
- QML `ChartView`로 시각화
- `testGraphUpdate()` → 신호 이력 수집 & 갱신

### ✅ 4. FastAPI 연동
- `testFastApiIntegration()`에서 현재 Wi-Fi 상태 JSON 전송
- FastAPI 서버에서 품질 평가 후 응답
- `onPredictionResult` 시그널로 결과 UI에 출력

### ✅ 5. ONNX 예측 연동 (CPU-only, Lite API)
- `export_wifi_predictor.py`로 PyTorch 모델 → ONNX 변환
- `runOnnxPrediction()`에서 ONNX Runtime C API 사용
- 최근 10개 신호를 기반으로 품질 예측

### ✅ 6. 자동 최적화 트리거
- `evaluatePrediction()` 내부에서 예측값 < 50 → 자동 `applyAllOptimizations()` 실행
- 예측 결과에 따라 자동으로 Wi-Fi 성능 강화 시도

### ✅ 7. 신호 이력 저장 기능
- `wifi_signal_log.csv`로 신호 강도 로그 저장
- 버튼 클릭으로 내보내기 실행 (`exportSignalHistoryToFile()`)

### ✅ 8. 시스템 통합 및 테스트 분리
- 모든 주요 기능은 QML 버튼과 독립된 `testX()` 메서드로 분리
- 각 기능 단독 테스트 가능

### ✅ 9. 최적화 전략 테이블 포함
- 운영체제별로 가능한 Wi-Fi 품질 강화 기법 15개 정리
- ONNX 예측, MTU 조절, QoS 설정, 드라이버 최신화 등 포함

### ✅ 10. README, CMake, .onnx, FastAPI 포함 전체 구성
- `README.md` 자동 생성
- `CMakeLists.txt`에 ONNX + Qt 연동 포함
- FastAPI 서버 (`fastapi_server.py`) 완성
- 전체 프로젝트 통합 `.zip` 또는 개별 파일 제공

---

📌 이 정리는 테스트 케이스 작성, 기능 리뷰, 리팩토링, 모듈 분리에 바로 활용 가능합니다.


---

## 🔧 최종 기능 요약 (2025년 기준)

| 기능 | 설명 |
|------|------|
| 📡 `refreshSignalStrength()` | 현재 Wi-Fi SSID 및 신호 강도 수집 |
| 📈 `updateSignalHistory()` | 신호 이력 저장 및 그래프 갱신 |
| 🧠 `runOnnxPrediction()` | 최근 신호 이력 기반 예측 (ONNX 또는 평균 기반) |
| 🤖 `evaluatePrediction()` | 예측값에 따라 보정 함수 자동 호출 |
| 🛠 자동 보정 함수 | `adjustMTU()`, `adjustRTO()`, `applySelectiveACK()`, `enableFEC()` 등 |
| 📤 `sendToFastApi()` | FastAPI로 SSID/신호 강도 전송 |
| 🧪 테스트 함수 | `testGraphUpdate()`, `testFastApiIntegration()` 등 |
| 💾 `exportSignalHistoryToFile()` | 신호 이력을 텍스트 파일로 저장 |
| ⏱️ `startAutoOptimization(intervalMs)` | 타이머 기반 자동 보정 시작 |
| 🛑 `stopAutoOptimization()` | 자동 보정 타이머 종료 |

---

## ⚙️ 자동 보정 흐름

1. 타이머 또는 수동으로 `refreshSignalStrength()` 호출  
2. → `updateSignalHistory()` 저장  
3. → `runOnnxPrediction()`  
4. → `evaluatePrediction()` 내에서 예측값 조건에 따라 자동 보정 수행:

| 예측값 | 동작 |
|--------|------|
| > 80 | 연결 양호, 보정 없음 |
| > 50 | `adjustRTO()` |
| ≤ 50 | `adjustRTO()`, `applySelectiveACK()`, `enableFEC()` |
| 매우 낮음 (확장 예정) | 추가 보정 예정 (MTU, 윈도우 크기 등) |

---

## 🧩 보정 함수 설명

| 함수 | 설명 |
|------|------|
| `adjustMTU()` | MTU 값을 1400으로 고정하여 데이터 조각화를 줄이고, 패킷 손실 확률 감소 (1회 설정으로 충분) |
| `adjustRTO()` | TCP 재전송 타이머를 줄여 손실 시 빠르게 재시도하게 함 (Linux `sysctl` 적용) |
| `applySelectiveACK()` | SACK 설정을 보장하여, 손실된 부분만 재전송하게 최적화 (Linux에서만 직접 설정 가능) |
| `enableFEC()` | 신호 이력 데이터에 패리티 기반 FEC 시뮬레이션 적용 → 예측 기반 보정 효과 |

---

## ⛑️ 참고

- 모든 보정은 플랫폼(OS)에 따라 적용 범위가 다릅니다.
- Qt 6 기반이며, QML과의 연동 구조도 완비되어 있습니다.
- `onnxruntime` 라이브러리는 사용하지 않으며, 예측은 평균값 기반 또는 FastAPI/ONNX로 대체됩니다.

## 🧠 QTimer::setInterval() 과 동적할당 관계 설명

### 🔧 개념 요약

`QTimer::setInterval(int msec)` 은 **기존 QTimer 객체의 주기만 설정**하는 함수입니다.  
즉, **객체를 새로 만들거나 삭제하지 않으며**, 동적할당과 직접적인 관련이 없습니다.

---

### ✅ 추천 방식: 멤버 변수 기반 QTimer 사용

```cpp
// wifioptimizer.h
QTimer m_autoTimer;

// wifioptimizer.cpp (생성자 또는 초기화 함수)
connect(&m_autoTimer, &QTimer::timeout, this, &WifiOptimizer::refreshSignalStrength);
m_autoTimer.setInterval(5000);  // 5초 간격
m_autoTimer.start();
```

- ✔️ `delete` 필요 없음
- ✔️ Qt가 수명 주기 자동 관리
- ✔️ 타이머 자동 시작 가능

---

### ⚠️ 주의: 정적 함수가 아님!

```cpp
QTimer::setInterval(5000);  // ❌ 오류 - 인스턴스 필요!
```

`setInterval()`은 정적(static)이 아니라,  
**QTimer 객체를 생성한 후 호출해야만** 동작합니다.

---

### ❗ 동적할당 방식 (구식 또는 필요할 때 사용)

```cpp
QTimer* timer = new QTimer(this);
connect(timer, &QTimer::timeout, this, &WifiOptimizer::refreshSignalStrength);
timer->setInterval(5000);
timer->start();
```

- ⚠️ 이 방식은 `new`를 썼기 때문에 수동 해제 (`delete`) 또는 `parent` 설정이 필요함

---

### ✅ 결론

| 방식 | 설명 | delete 필요 |
|------|------|--------------|
| `QTimer m_autoTimer` + `setInterval()` | 자동 메모리 관리 | ❌ |
| `new QTimer(...)` | 수동 해제 필요 | ✅ (또는 `parent`) |

동적할당을 피하고 싶다면  
👉 **멤버 변수 기반 QTimer + setInterval() 사용**이 가장 안전하고 효율적입니다. 🚀


✅ Libuv 기반 신호 이력 저장 기능 개선
QTimer나 동적 할당 없이 uv_run() 한 줄로 libuv 이벤트 루프 처리

writeCsvAsync() 호출 시 필요한 순간에만 libuv 처리 → CPU 낭비 없음

파일 저장 완료 시 onClose() 내부에서 _context.reset() 자동 실행

중복 실행 방지 상태(exporter busy)가 무한 지속되는 문제 해결

writeCsvAsync()는 동기적으로 동작하지만 저장 시간이 매우 짧아 UI는 영향 없음

.txt 포맷으로 signalHistory 저장 → 실제 포맷은 줄 단위 텍스트

Qt/QML과의 연결은 유지되며, 저장 완료 후 saveFinished 시그널로 QML UI 업데이트

📌 libuv의 비동기 구조는 유지하면서도, Qt 구조에 최적화된 경량 동기 처리 방식으로 통합


✅ 완전한 비동기 + 취소 가능한 signalHistory 저장 기능 추가
전체 저장 작업이 QtConcurrent::run()을 통해 백그라운드 스레드에서 완전히 비동기적으로 실행되도록 개선

UI 스레드와 분리되어 저장 중에도 UI 반응성 완전 유지

저장 작업 상태를 추적하기 위해 QFuture 및 QFutureWatcher 도입

cancelExport() 함수 추가로 저장 중 중간 취소 요청을 안전하게 처리

취소 시에는 저장을 즉시 중단하고, saveCancelled 시그널을 통해 QML에 알려줌

진행률(progressChanged), 완료(saveFinished), 취소(saveCancelled) 등 모든 시그널은 UI 메인 스레드에서 안전하게 emit

내부적으로 std::atomic_bool을 사용해 멀티스레드 환경에서도 데이터 충돌 없이 중단 가능

✔ 이로써 사용자 인터랙션 중에도 끊김 없이 작동하는, 완전한 비동기 저장 기능이 완성되었습니다.
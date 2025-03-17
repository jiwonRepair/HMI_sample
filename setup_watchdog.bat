@echo off
chcp 65001 > nul

set TASK_NAME=QtUIWatchdog
set PROGRAM_PATH="D:\Program Files\Everything\Everything.exe"

echo [Windows] Task Scheduler에 새로운 작업을 생성합니다...

:: 기존 작업 삭제 (에러 무시)
schtasks /Delete /TN "%TASK_NAME%" /F > nul 2>&1

:: 실행 경로 공백 문제 해결 (cmd /c ""경로"")
schtasks /Create /SC MINUTE /MO 1 /TN "%TASK_NAME%" /TR "cmd /c \"\"D:\Program Files\Everything\Everything.exe\"\"" /RL HIGHEST /F

echo ✅ UI 자동 재시작 설정 완료!
pause


::1분마다 실행 → MO 1
::5분마다 실행 → MO 5
::10분마다 실행 → MO 10
::30분마다 실행 → MO 30
::1시간마다 실행 → SC HOURLY /MO 1
::매일 실행 → SC DAILY /MO 1

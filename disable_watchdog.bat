@echo off
chcp 65001 > nul

echo [Windows] Task Scheduler에서 QtUIWatchdog 자동 실행을 해제합니다...

set TASK_NAME=QtUIWatchdog

:: 기존 작업 삭제
schtasks /Delete /TN "%TASK_NAME%" /F

echo ✅ UI 자동 재시작 설정이 해제되었습니다!
pause

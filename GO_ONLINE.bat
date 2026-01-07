@echo off
title ESP32-CAM Remote Access
cls
echo ========================================================
echo   CONNECTING TO INTERNET (PUBLIC ACCESS)
echo ========================================================
echo.
echo 1. Starting Local Server...
start "Surveillance Server" /min python app.py
echo    - Server running on port 5000
echo.
echo 2. Getting Public URL...
echo    - You need 'ngrok' installed for this.
echo    - If you don't have it, dashboard will be local only.
echo.
echo    Running ngrok http 5000...
ngrok http 5000
pause

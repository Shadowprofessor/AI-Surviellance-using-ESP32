# 📸 ESP32-CAM AI Detection System

Real-time Person & Vehicle Detection system using ESP32-CAM + YOLOv8.

## 🚀 Quick Start

### 1. Install Dependencies
```bash
pip install -r requirements.txt
```

### 2. Configure Email Alerts (Optional)
If you want email alerts:
1. Open `.env.json`
2. Add your Gmail credentials (use App Password)
3. Set receiver email

### 3. Start Server
```bash
START_SERVER.bat
```
Or manually:
```bash
python app.py
```

### 4. Setup ESP32-CAM
Follow `UPLOAD_INSTRUCTIONS.md` to upload firmware.

## 🎯 Features

- **Person Detection** (Green Box)
- **Vehicle Detection** (Bicycles - Yellow, Motorcycles - Blue)
- **Email Alerts** for high confidence detections (>59%)
- **Real-time Dashboard** at http://localhost:5000

## 🔧 Configuration

**Detection Settings (`app.py`):**
- Confidence Threshold: 0.45 (Detection), 0.59 (Alerts)
- Classes: Person, Bicycle, Motorcycle

**Email Settings (`.env.json`):**
- SMTP Server: `smtp.gmail.com`
- Port: 587
- Cooldown: 60 seconds (prevent spam)

## 🐛 Troubleshooting

- **Server Crashing?** Ensure `app.py` is running and port 5000 is free.
- **No Email?** Check `.env.json` and ensure "Less Secure Apps" or App Password is used.
- **No Video?** Check ESP32-CAM power and WiFi connection.

## 📁 System Structure

- `app.py`: Main Flask server with YOLOv8 & Email logic
- `templates/index.html`: Dashboard with Vehicle support
- `esp32_cam/`: Firmware for ESP32
- `.env.json`: Email configuration

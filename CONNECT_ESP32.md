# 📸 ESP32-CAM Connection Instructions

## ✅ Server Status: READY
Your detection server is running on:
- **Local**: http://localhost:5000
- **Network**: http://172.19.19.93:5000

## 🔌 Step-by-Step ESP32-CAM Setup

### 1️⃣ Hardware Connections

**For Programming (Upload Firmware):**
```
ESP32-CAM          FTDI Adapter
---------          ------------
GND        ←→      GND
5V         ←→      5V (or VCC)
U0R (RX)   ←→      TX
U0T (TX)   ←→      RX
GPIO0      ←→      GND (for programming mode)
```

**Important**: GPIO0 to GND is ONLY for uploading. Remove after upload!

### 2️⃣ Upload Firmware

1. **Open Arduino IDE**
2. **File** → **Open** → `esp32_cam/esp32_cam.ino`
3. **Tools** → **Board** → **ESP32 Arduino** → **AI Thinker ESP32-CAM**
4. **Tools** → **Port** → Select your COM port
5. Click **Upload** button (→)
6. Wait for "Done uploading"
7. **DISCONNECT GPIO0 from GND**
8. Press **RESET** button on ESP32-CAM

### 3️⃣ Normal Operation Connections

**After uploading, for normal use:**
```
ESP32-CAM          Power Supply
---------          ------------
GND        ←→      GND
5V         ←→      5V (500mA minimum)
```

**Note**: You can use USB power adapter or battery pack (5V)

### 4️⃣ Monitor Connection

1. **Open Serial Monitor** in Arduino IDE
2. Set baud rate to **115200**
3. Press **RESET** on ESP32-CAM
4. You should see:
```
=== ESP32-CAM Detection System ===
📷 Initializing camera...
✅ Camera initialized (VGA 640x480)
📡 Connecting to WiFi...
   SSID: Samsung
✅ WiFi connected!
   IP: 192.168.x.x
   Signal: -XX dBm
=== System Ready ===

📤 Frame 1 sent | Response: 200
📤 Frame 20 sent | Response: 200
```

### 5️⃣ Verify Detection

1. **Open browser**: http://localhost:5000
2. **Check status badge**: Should show "Connected: ESP32_CAM_01"
3. **Video feed**: Should show live camera view
4. **Stats**: Should update with detections

## 🔍 Troubleshooting

### ❌ "WiFi connection failed!"
**Solution:**
- Check WiFi name is exactly: `Samsung`
- Check password is exactly: `eerr123f`
- Make sure WiFi is 2.4GHz (ESP32 doesn't support 5GHz)
- Try moving ESP32-CAM closer to router

### ❌ "Error: -1" when sending frames
**Solution:**
- Check server IP: `192.169.19.93`
- Make sure server is running (check terminal)
- Verify PC and ESP32-CAM are on same network
- Try disabling Windows Firewall temporarily

### ❌ Camera not initializing
**Solution:**
- Check all camera ribbon cable connections
- Try pressing RESET button
- Re-upload firmware
- Check 5V power supply (needs 500mA+)

### ❌ Brown-out detector triggered
**Solution:**
- Use better power supply (at least 500mA)
- Add 100µF capacitor between 5V and GND
- Don't power from FTDI (not enough current)

## 📊 Expected Behavior

**When Working Correctly:**
- ✅ LED blinks every 10 frames
- ✅ Serial shows "Response: 200"
- ✅ Dashboard shows live video
- ✅ Detections appear in real-time
- ✅ FPS around 5

## 🎯 Your Current Configuration

```
WiFi SSID:     Samsung
WiFi Password: eerr123f
Server IP:     192.169.19.93:5000
Camera ID:     ESP32_CAM_01
Frame Rate:    5 FPS
Resolution:    640x480 (VGA)
```

## 🚀 Ready to Connect!

Your server is **LIVE** and waiting for ESP32-CAM!

Just:
1. Upload the firmware
2. Power on ESP32-CAM
3. Watch it connect automatically!

---

**Need help?** Check the Serial Monitor output for detailed error messages.

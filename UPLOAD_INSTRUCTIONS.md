# 🚀 ESP32-CAM UPLOAD CHECKLIST - FOLLOW EXACTLY

## ✅ STEP 1: Verify Firmware Settings

Open this file in Arduino IDE:
`esp32_cam/esp32_cam.ino`

**Check these lines are EXACTLY:**
```cpp
Line 11: const char* ssid = "Samsung";
Line 12: const char* password = "eerr123f";
Line 14: const char* serverUrl = "http://10.89.63.651:5000/upload_frame";
```

✅ If correct, proceed to Step 2
❌ If wrong, fix them first!

---

## ✅ STEP 2: Arduino IDE Setup

1. **Open Arduino IDE**
2. **Tools** → **Board** → **ESP32 Arduino** → **AI Thinker ESP32-CAM**
3. **Tools** → **Port** → Select your COM port (e.g., COM3, COM4)
4. **Tools** → **Upload Speed** → **115200**

---

## ✅ STEP 3: Hardware Connections for Upload

**IMPORTANT: Connect GPIO0 to GND for programming mode!**

```
ESP32-CAM Pin    →    FTDI/USB Adapter
─────────────────────────────────────
GND              →    GND
5V               →    5V (or VCC)
U0R (RX)         →    TX
U0T (TX)         →    RX
GPIO0            →    GND  ⚠️ CRITICAL FOR UPLOAD!
```

**Double-check:**
- [ ] GPIO0 is connected to GND
- [ ] RX and TX are crossed (ESP32 RX → FTDI TX)
- [ ] Power is connected (5V and GND)

---

## ✅ STEP 4: Upload Firmware

1. Click the **Upload** button (→) in Arduino IDE
2. **Wait** - you'll see:
   ```
   Connecting........_____.....
   Writing at 0x00001000... (3%)
   Writing at 0x00002000... (6%)
   ...
   Writing at 0x000f0000... (100%)
   Leaving...
   Hard resetting via RTS pin...
   ```
3. Wait for **"Done uploading"** message

**If upload fails:**
- Press and hold RESET button
- Click Upload
- Release RESET when you see "Connecting..."

---

## ✅ STEP 5: Switch to Normal Mode

**CRITICAL: Remove GPIO0 connection!**

1. **Disconnect GPIO0 from GND** ⚠️
2. Press **RESET** button on ESP32-CAM
3. ESP32-CAM should now boot normally

**For normal operation, you only need:**
```
ESP32-CAM Pin    →    Power Source
─────────────────────────────────────
GND              →    GND
5V               →    5V (500mA minimum)
```

---

## ✅ STEP 6: Monitor Serial Output

1. **Tools** → **Serial Monitor**
2. Set baud rate to **115200**
3. Press **RESET** on ESP32-CAM

**You should see:**
```
=== ESP32-CAM Detection System ===
📷 Initializing camera...
✅ Camera initialized (VGA 640x480)
📡 Connecting to WiFi...
   SSID: Samsung
✅ WiFi connected!
   IP: 10.89.63.175
   Signal: -58 dBm
=== System Ready ===

📤 Frame 1 sent | Response: 200  ← ✅ SUCCESS!
📤 Frame 20 sent | Response: 200
```

**If you see "Error: connection refused":**
- Server IP is wrong - check Step 1
- Server not running - check http://localhost:5000
- Firewall blocking - try disabling Windows Firewall

---

## ✅ STEP 7: Verify on Dashboard

1. Open browser: **http://localhost:5000**
2. Check status badge: Should say **"Connected: ESP32_CAM_01"**
3. Video feed: Should show **live ESP32-CAM view**
4. Bottom of video: Should say **"Source: ESP32_CAM_01"** (not "Webcam")

---

## 🔍 TROUBLESHOOTING

### ❌ Upload Failed
- Check GPIO0 is connected to GND
- Try different USB cable
- Press RESET during upload
- Check COM port is correct

### ❌ WiFi Connection Failed
- Check SSID: "Samsung" (case-sensitive)
- Check password: "eerr123f"
- Make sure it's 2.4GHz WiFi (not 5GHz)
- Move ESP32-CAM closer to router

### ❌ Connection Refused
- **YOU ARE HERE** - This means firmware has old IP
- Re-upload firmware (Steps 1-6)
- Verify server IP: 10.89.63.651

### ❌ Camera Not Initializing
- Check camera ribbon cable
- Try re-seating the cable
- Power cycle ESP32-CAM

---

## ✅ SUCCESS INDICATORS

When everything works, you'll see:

**Serial Monitor:**
```
📤 Frame sent | Response: 200
```

**Dashboard:**
- Status: "Connected: ESP32_CAM_01"
- Video: Live feed from ESP32-CAM
- Source: "ESP32_CAM_01"
- FPS: ~5

---

## 📋 CURRENT CONFIGURATION

```
WiFi Network:  Samsung
WiFi Password: eerr123f
ESP32-CAM IP:  10.89.63.175 (auto-assigned)
Server IP:     10.89.63.651:5000
Camera ID:     ESP32_CAM_01
Frame Rate:    5 FPS (1 frame every 200ms)
Resolution:    640x480 (VGA)
```

---

## 🎯 START HERE

**Your next action:**
1. Open Arduino IDE
2. Open `esp32_cam/esp32_cam.ino`
3. Follow Steps 1-7 above

**Server is ready and waiting!**

The detection system is running at http://localhost:5000
Just upload the firmware and it will connect automatically!

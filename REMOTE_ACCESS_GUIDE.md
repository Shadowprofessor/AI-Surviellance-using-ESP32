# 🌍 How to Access Your Camera from Anywhere

This guide explains how to make your surveillance dashboard "Dynamic" and accessible from any network (mobile data, office, etc.) without changing your code.

## 🚀 The Solution: Ngrok Tunneling
We use a tool called **Ngrok** to create a secure bridge from the internet to your laptop.

### 1. Setup (One Time Only)
1.  **Download Ngrok**: [https://ngrok.com/download](https://ngrok.com/download) (Choose Windows).
2.  **Unzip**: Extract `ngrok.exe` to this folder:
    `c:\Users\Dell\OneDrive\Desktop\New surviellance\`
3.  **Signup (Free)**: Go to [dashboard.ngrok.com](https://dashboard.ngrok.com/signup) and get your **Authtoken**.
4.  **Connect Account**: Open terminal in this folder and run:
    ```cmd
    ngrok config add-authtoken YOUR_TOKEN_HERE
    ```

### 2. Go Online
Just double-click **`GO_ONLINE.bat`**.

You will see a screen like this:
```
Forwarding                    https://1a2b-3c4d.ngrok.app -> http://localhost:5000
```

### 3. Use Anywhere
*   **Copy the URL** (e.g., `https://1a2b-3c4d.ngrok.app`).
*   Open it on your **Phone** or **Laptop** on any network.
*   You can now see the live video and stats!

---

## ⚠️ Important Note for ESP32-CAM
The **ESP32-CAM** still sends data to your laptop's **Local IP** (`10.89.63.65`).
*   **Good News**: You don't need to change ESP32 code if your laptop and ESP32 stay on the same WiFi (Samsung).
*   **Remote Viewing**: You can be *anywhere* in the world viewing the dashboard, as long as your laptop allows the camera to connect locally.

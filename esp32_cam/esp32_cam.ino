/*
 * ═══════════════════════════════════════════════════════════
 * ESP32-CAM AI SURVEILLANCE SYSTEM
 * ═══════════════════════════════════════════════════════════
 * Production-ready firmware for ESP32-CAM with YOLOv8 backend
 * Features: Auto-reconnect, error recovery, optimized streaming
 * ═══════════════════════════════════════════════════════════
 */

#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

/* ═══════════════════════════════════════════════════════════
 * CONFIGURATION - UPDATE THESE VALUES
 * ═══════════════════════════════════════════════════════════ */

// WiFi Credentials
const char* WIFI_SSID = "Samsung";
const char* WIFI_PASS = "eerr123f";

// Server Configuration
// 🔴 CRITICAL: Use your PC's actual IP address (check with ipconfig)
const char* SERVER_URL = "http://10.89.63.65:5000/upload_frame";
const char* CAMERA_ID = "ESP32_CAM_01";

// Performance Settings
// BALANCED FOR QUALITY & SPEED
const unsigned long FRAME_INTERVAL = 100;  // 10 FPS Target
const int HTTP_TIMEOUT = 2000;             // 2 second timeout
const int WIFI_RETRY_DELAY = 500;          // WiFi retry delay
const int MAX_WIFI_RETRIES = 50;           // Max WiFi connection attempts

/* ═══════════════════════════════════════════════════════════
 * AI THINKER ESP32-CAM PIN CONFIGURATION
 * ═══════════════════════════════════════════════════════════ */
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define LED_PIN            4

/* ═══════════════════════════════════════════════════════════
 * GLOBAL STATE
 * ═══════════════════════════════════════════════════════════ */
unsigned long lastFrameTime = 0;
unsigned long lastWiFiCheck = 0;
uint32_t frameCount = 0;
uint32_t successCount = 0;
uint32_t errorCount = 0;

/* ═══════════════════════════════════════════════════════════
 * SETUP - RUNS ONCE ON BOOT
 * ═══════════════════════════════════════════════════════════ */
void setup() {
  // Initialize Serial
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  delay(1000);
  
  // Print banner
  Serial.println("\n╔═══════════════════════════════════════════╗");
  Serial.println("║   ESP32-CAM AI SURVEILLANCE SYSTEM        ║");
  Serial.println("╚═══════════════════════════════════════════╝\n");
  
  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Initialize camera
  if (!initCamera()) {
    Serial.println("❌ FATAL: Camera initialization failed");
    Serial.println("   → Check camera ribbon cable");
    Serial.println("   → Restarting in 5 seconds...");
    delay(5000);
    ESP.restart();
  }
  
  // Connect to WiFi
  if (!connectWiFi()) {
    Serial.println("❌ FATAL: WiFi connection failed");
    Serial.println("   → Check SSID and password");
    Serial.println("   → Ensure 2.4GHz network");
    Serial.println("   → Restarting in 5 seconds...");
    delay(5000);
    ESP.restart();
  }
  
  // Success indicator
  blinkLED(3, 200);
  Serial.println("\n✅ SYSTEM READY - Starting frame capture\n");
  Serial.println("═══════════════════════════════════════════\n");
}

/* ═══════════════════════════════════════════════════════════
 * MAIN LOOP - RUNS CONTINUOUSLY
 * ═══════════════════════════════════════════════════════════ */
void loop() {
  // Check WiFi connection every 10 seconds
  if (millis() - lastWiFiCheck > 10000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("⚠️  WiFi disconnected - reconnecting...");
      connectWiFi();
    }
    lastWiFiCheck = millis();
  }
  
  // Capture and send frame at specified interval
  if (millis() - lastFrameTime >= FRAME_INTERVAL) {
    captureAndSendFrame();
    lastFrameTime = millis();
  }
  
  // Small delay to prevent watchdog issues
  delay(10);
}

/* ═══════════════════════════════════════════════════════════
 * CAMERA INITIALIZATION
 * ═══════════════════════════════════════════════════════════ */
bool initCamera() {
  Serial.println("📷 Initializing camera...");
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  
  // BALANCED SETTINGS
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size   = FRAMESIZE_VGA;  // 640x480 (Good Detail)
  config.jpeg_quality = 12;             // High Quality
  config.fb_count     = 2;              // Double buffering
  config.grab_mode    = CAMERA_GRAB_LATEST;
  
  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("   ❌ Camera init failed: 0x%x\n", err);
    return false;
  }
  
  // Get camera sensor
  sensor_t * s = esp_camera_sensor_get();
  if (s) {
    // HIGH BRIGHTNESS SETTINGS
    s->set_brightness(s, 2);     // -2 to 2 (Max Brightness)
    s->set_contrast(s, 1);       // -2 to 2 (Higher Contrast)
    s->set_saturation(s, 1);     // -2 to 2 (Vivid Colors)
    s->set_whitebal(s, 1);       // Enable auto white balance
    s->set_awb_gain(s, 1);       // Enable auto white balance gain
    s->set_wb_mode(s, 0);        // Auto white balance mode
    s->set_exposure_ctrl(s, 1);  // Enable auto exposure
    s->set_aec2(s, 1);           // Enable AEC2 (DSP)
    s->set_ae_level(s, 2);       // Increase AE Level (-2 to 2)
    s->set_gain_ctrl(s, 1);      // Enable auto gain
    s->set_agc_gain(s, 0);       // Auto gain value
    s->set_gainceiling(s, (gainceiling_t)GAINCEILING_128X);  // Max Gain Ceiling
    s->set_bpc(s, 0);            // Disable black pixel correction
    s->set_wpc(s, 1);            // Enable white pixel correction
    s->set_raw_gma(s, 1);        // Enable gamma correction
    s->set_lenc(s, 1);           // Enable lens correction
    s->set_hmirror(s, 0);        // Disable horizontal mirror
    s->set_vflip(s, 0);          // Disable vertical flip
    s->set_dcw(s, 1);            // Enable downsize
    s->set_colorbar(s, 0);       // Disable color bar test pattern
  }
  
  Serial.println("   ✅ Camera initialized (VGA 640x480)");
  return true;
}

/* ═══════════════════════════════════════════════════════════
 * WIFI CONNECTION
 * ═══════════════════════════════════════════════════════════ */
bool connectWiFi() {
  Serial.println("📡 Connecting to WiFi...");
  Serial.printf("   SSID: %s\n", WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < MAX_WIFI_RETRIES) {
    delay(WIFI_RETRY_DELAY);
    Serial.print(".");
    attempts++;
    
    // Blink LED during connection
    digitalWrite(LED_PIN, attempts % 2);
  }
  
  digitalWrite(LED_PIN, LOW);
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n   ❌ WiFi connection failed");
    return false;
  }
  
  Serial.println("\n   ✅ WiFi connected");
  Serial.print("   IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("   Signal: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.print("   Server: ");
  Serial.println(SERVER_URL);
  
  return true;
}

/* ═══════════════════════════════════════════════════════════
 * CAPTURE AND SEND FRAME
 * ═══════════════════════════════════════════════════════════ */
void captureAndSendFrame() {
  // Capture frame
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("❌ Camera capture failed");
    errorCount++;
    return;
  }
  
  frameCount++;
  
  // Check WiFi before sending
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️  WiFi not connected - skipping frame");
    esp_camera_fb_return(fb);
    errorCount++;
    return;
  }
  
  // Send frame to server
  HTTPClient http;
  http.begin(SERVER_URL);
  http.setTimeout(HTTP_TIMEOUT);
  http.addHeader("Content-Type", "image/jpeg");
  http.addHeader("X-Camera-ID", CAMERA_ID);
  
  int httpCode = http.POST(fb->buf, fb->len);
  
  // Process response
  if (httpCode > 0) {
    successCount++;
    
    // Log every 20th frame to reduce serial spam
    if (frameCount % 20 == 0) {
      Serial.printf("📤 Frame %lu | HTTP %d | Size: %lu KB | Success: %lu | Errors: %lu\n",
                    frameCount, httpCode, fb->len / 1024, successCount, errorCount);
    }
    
    // Blink LED on successful send
    if (frameCount % 10 == 0) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
  } else {
    errorCount++;
    Serial.printf("❌ Frame %lu | HTTP ERROR: %s\n",
                  frameCount, http.errorToString(httpCode).c_str());
    
    // If too many errors, try reconnecting WiFi
    if (errorCount > 10 && errorCount % 10 == 0) {
      Serial.println("⚠️  Multiple errors detected - checking WiFi...");
      connectWiFi();
    }
  }
  
  http.end();
  esp_camera_fb_return(fb);
}

/* ═══════════════════════════════════════════════════════════
 * UTILITY FUNCTIONS
 * ═══════════════════════════════════════════════════════════ */
void blinkLED(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(delayMs);
    digitalWrite(LED_PIN, LOW);
    delay(delayMs);
  }
}

"""
═══════════════════════════════════════════════════════════
ESP32-CAM AI SURVEILLANCE SERVER
═══════════════════════════════════════════════════════════
Production-ready Flask server with YOLOv8 detection
Features: Real-time detection, WebSocket updates, error handling
═══════════════════════════════════════════════════════════
"""

import cv2
import numpy as np
from ultralytics import YOLO
from flask import Flask, Response, render_template, jsonify, request
from flask_socketio import SocketIO
from flask_cors import CORS
import threading
from datetime import datetime
from collections import deque
import time
import logging
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.image import MIMEImage
import json
import os

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# ----- EMAIL CONFIGURATION -----
class EmailService:
    def __init__(self):
        self.config = self._load_config()
        self.last_email_time = 0
        
    def _load_config(self):
        try:
            with open('.env.json', 'r') as f:
                return json.load(f)
        except Exception as e:
            logger.warning(f"Email config not found: {e}")
            return None

    def send_alert(self, class_name, confidence, image_frame):
        if not self.config: return
        
        # Check cooldown
        if time.time() - self.last_email_time < self.config.get('COOLDOWN_SECONDS', 60):
            return

        # Check confidence threshold from config
        if confidence < self.config.get('CONFIDENCE_THRESHOLD', 0.59):
            return

        try:
            msg = MIMEMultipart()
            msg['Subject'] = f"🚨 Alert: {class_name.upper()} Detected ({confidence:.0%})"
            msg['From'] = self.config['SENDER_EMAIL']
            msg['To'] = self.config['RECEIVER_EMAIL']

            body = f"""
            <h3>Security Alert System</h3>
            <p>A <b>{class_name}</b> was detected with <b>{confidence:.1%}</b> confidence.</p>
            <p>Time: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
            """
            msg.attach(MIMEText(body, 'html'))

            # Attach image
            _, img_encoded = cv2.imencode('.jpg', image_frame)
            img = MIMEImage(img_encoded.tobytes())
            img.add_header('Content-Disposition', 'attachment', filename='detection.jpg')
            msg.attach(img)

            # Send email
            with smtplib.SMTP(self.config['SMTP_SERVER'], self.config['SMTP_PORT']) as server:
                server.starttls()
                server.login(self.config['SENDER_EMAIL'], self.config['SENDER_PASSWORD'])
                server.send_message(msg)
            
            self.last_email_time = time.time()
            logger.info(f"📧 Alert email sent for {class_name}")
            
        except Exception as e:
            logger.error(f"Failed to send email: {e}")

email_service = EmailService()

# Initialize Flask app
app = Flask(__name__, template_folder='templates')
app.config['SECRET_KEY'] = 'esp32-surveillance-2024'
app.config['MAX_CONTENT_LENGTH'] = 5 * 1024 * 1024  # 5MB max

CORS(app)
socketio = SocketIO(app, cors_allowed_origins="*", async_mode='threading')

# Load YOLOv8 model
logger.info("Loading YOLOv8 model...")
# Load YOLOv8 model - Using 'small' model for better accuracy than 'nano'
logger.info("Loading YOLOv8s model (Better Accuracy)...")
model = YOLO("yolov8s.pt")
logger.info("✅ Model loaded successfully")

# Detection configuration
TARGET_CLASSES = {
    'person': (0, 255, 0),      # Green
    'bicycle': (255, 255, 0),   # Yellow
    'motorcycle': (0, 0, 255)   # Blue
}

# Global state
latest_frame = None
frame_lock = threading.Lock()
stats = {
    'total_detections': 0,
    'people': 0,
    'vehicles': 0,
    'fps': 0,
    'frames_received': 0,
    'last_update': None
}
alerts = deque(maxlen=100)
camera_status = {
    'connected': False,
    'last_seen': None,
    'camera_id': None
}

def detect_objects(frame, camera_id):
    """
    Run YOLOv8 detection on frame
    Returns: processed frame, people count, vehicle count
    """
    global stats
    
    start_time = time.time()
    
    # Run detection with tuned parameters for People/Cycles
    results = model(
        frame,
        conf=0.35,      # Lower threshold to catch more objects
        iou=0.50,       # Better handling of Person-on-Bike overlap
        imgsz=640,      # Input size
        verbose=False
    )[0]
    
    people_count = 0
    vehicle_count = 0
    
    # Process detections
    for box in results.boxes:
        class_name = model.names[int(box.cls[0])]
        
        if class_name in TARGET_CLASSES:
            confidence = float(box.conf[0])
            x1, y1, x2, y2 = map(int, box.xyxy[0])
            color = TARGET_CLASSES[class_name]
            
            # Draw bounding box
            cv2.rectangle(frame, (x1, y1), (x2, y2), color, 3)
            
            # Draw label with background
            label = f"{class_name.upper()} {confidence:.2f}"
            label_size, _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.6, 2)
            cv2.rectangle(frame, (x1, y1 - label_size[1] - 10), 
                         (x1 + label_size[0], y1), color, -1)
            cv2.putText(frame, label, (x1, y1 - 5),
                       cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2, cv2.LINE_AA)
            
            # Count detections
            if class_name == "person":
                people_count += 1
            else:
                vehicle_count += 1
            
            # Create alert for high-confidence detections
            if confidence > 0.59:
                # Trigger email alert
                threading.Thread(
                    target=email_service.send_alert,
                    args=(class_name, confidence, frame.copy())
                ).start()

                alert = {
                    "type": class_name,
                    "confidence": round(confidence, 2),
                    "time": datetime.now().strftime("%H:%M:%S"),
                    "source": camera_id
                }
                alerts.append(alert)
                socketio.emit("alert", alert)
    
    # Update stats
    stats["people"] = people_count
    stats["vehicles"] = vehicle_count
    stats["total_detections"] += people_count + vehicle_count
    stats["fps"] = round(1 / (time.time() - start_time), 1)
    stats["last_update"] = datetime.now().isoformat()
    
    # Draw info overlay
    cv2.putText(frame, f"People: {people_count} | Vehicles: {vehicle_count}",
               (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
    cv2.putText(frame, f"FPS: {stats['fps']}",
               (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)
    cv2.putText(frame, f"Source: {camera_id}",
               (10, frame.shape[0] - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)
    
    return frame, people_count, vehicle_count

@app.route("/upload_frame", methods=["POST"])
def upload_frame():
    """
    Receive frame from ESP32-CAM
    """
    global latest_frame, camera_status
    
    try:
        # Get camera ID from headers
        camera_id = request.headers.get("X-Camera-ID", "ESP32_UNKNOWN")
        
        # Decode image
        img_data = np.frombuffer(request.data, np.uint8)
        frame = cv2.imdecode(img_data, cv2.IMREAD_COLOR)
        
        if frame is None:
            logger.error("Failed to decode image")
            return jsonify({"error": "Invalid image data"}), 400
        
        # Update camera status
        camera_status['connected'] = True
        camera_status['last_seen'] = datetime.now().isoformat()
        camera_status['camera_id'] = camera_id
        stats['frames_received'] += 1
        
        # Run detection
        processed_frame, people, vehicles = detect_objects(frame, camera_id)
        
        # Update latest frame
        with frame_lock:
            latest_frame = processed_frame.copy()
        
        # Emit WebSocket update
        socketio.emit("update", {
            "people": people,
            "vehicles": vehicles,
            "fps": stats["fps"],
            "camera_id": camera_id
        })
        
        # Log every 50 frames
        if stats['frames_received'] % 50 == 0:
            logger.info(f"📊 Frames: {stats['frames_received']} | "
                       f"FPS: {stats['fps']} | "
                       f"People: {people} | Vehicles: {vehicles}")
        
        return jsonify({"ok": True}), 200
        
    except Exception as e:
        logger.error(f"Error processing frame: {e}")
        return jsonify({"error": str(e)}), 500

def generate_stream():
    """
    Generate MJPEG stream for browser
    """
    while True:
        with frame_lock:
            if latest_frame is None:
                # Create placeholder
                placeholder = np.zeros((480, 640, 3), np.uint8)
                cv2.putText(placeholder, "Waiting for ESP32-CAM...",
                           (120, 240), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
                frame = placeholder
            else:
                frame = latest_frame.copy()
        
        # Encode as JPEG
        ret, jpeg = cv2.imencode(".jpg", frame, [cv2.IMWRITE_JPEG_QUALITY, 90])
        if ret:
            yield (b"--frame\r\n"
                   b"Content-Type: image/jpeg\r\n\r\n" + jpeg.tobytes() + b"\r\n")
        
        time.sleep(0.033)  # ~30 FPS max

@app.route("/")
def index():
    """Serve dashboard"""
    return render_template("index.html")

@app.route("/video")
def video():
    """Video stream endpoint"""
    return Response(generate_stream(),
                   mimetype="multipart/x-mixed-replace; boundary=frame")

@app.route("/stats")
def get_stats():
    """Get current statistics"""
    return jsonify({
        "stats": stats,
        "alerts": list(alerts)[-20:],
        "camera_status": camera_status
    })

@app.route("/health")
def health():
    """Health check endpoint"""
    return jsonify({
        "status": "running",
        "camera_connected": camera_status['connected'],
        "frames_received": stats['frames_received']
    })

if __name__ == "__main__":
    print("\n" + "═" * 60)
    print("🎯 ESP32-CAM AI SURVEILLANCE SERVER")
    print("═" * 60)
    print(f"📡 Server starting on http://0.0.0.0:5000")
    print(f"🌐 Dashboard: http://localhost:5000")
    print(f"📸 ESP32 endpoint: /upload_frame")
    print("═" * 60 + "\n")
    
    # Run server
    socketio.run(
        app,
        host="0.0.0.0",  # CRITICAL: Bind to all interfaces
        port=5000,
        debug=False,
        allow_unsafe_werkzeug=True
    )

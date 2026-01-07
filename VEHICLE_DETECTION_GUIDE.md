# 🚗 Vehicle Detection Update

The system has been upgraded to detect **People** and **Vehicles** (Bicycles & Motorcycles).

## 🎯 New Detection Classes

The system now prioritizes the following classes:

1.  **Person** (Green Box)
2.  **Bicycle** (Yellow Box)
3.  **Motorcycle** (Blue Box)

## 📊 Dashboard Updates

The stats panel has been updated to track:

-   **People**: Count of persons detected
-   **Vehicles**: Combined count of Bicycles and Motorcycles
-   **Total Detections**: Aggregate count
-   **FPS**: Real-time processing speed

## 🔧 backend Configuration

The `app.py` has been modified to:

-   Load standard `yolov8n.pt`.
-   Filter for `person`, `bicycle`, and `motorcycle` classes.
-   Group `bicycle` and `motorcycle` into the "Vehicles" statistics category.

## 🚀 How to Test

1.  **Point the camera** at a person, a bicycle, or a motorcycle.
2.  **Check the Dashboard** (`http://localhost:5000`).
3.  You should see colored bounding boxes and the "Vehicles" count increase for cycles.

## 📝 Notes

-   **Animals** are temporarily disabled to prioritize vehicle detection performance.
-   **Confidence Threshold** is set to 0.45 for balanced detection.
-   **IoU Threshold** is 0.55 to prevent overlapping boxes.

# Project Presentation: Smart Traffic Light System

This document is designed to help you present this project to your instructor. It highlights the technical complexity, modular design, and real-world applicability of the system.

## 1. The "Elevator Pitch"
"I have built a real-time, AI-powered traffic signal controller that uses Computer Vision to reduce urban congestion. Unlike traditional fixed-timer lights, this system 'sees' vehicle density using a YOLOv8 neural network and dynamically allocates green time to the busiest lanes using a Max-Pressure algorithm."

## 2. Key Technical Talking Points

### A. Perception (The "Eyes")
- **Model**: YOLOv8 (You Only Look Once) nano variant in ONNX format.
- **Library**: OpenCV DNN module for high-performance CPU inference.
- **Logic**: The system filters detections to only count vehicles (cars, buses, trucks) while ignoring background noise.

### B. Lane Management (The "Context")
- **ROI Mapping**: Instead of just counting objects, the system uses Regions of Interest (ROIs) to map detections to specific physical lanes.
- **Density Calculation**: It calculates a normalized density score (0.0 to 1.0) for each lane, providing a stable input for the controller.

### C. Adaptive Control (The "Brain")
- **Max-Pressure Algorithm**: A mathematically proven method to minimize average waiting time by prioritizing lanes with the highest traffic pressure.
- **Anti-Starvation**: A critical safety feature that ensures low-traffic lanes aren't ignored indefinitely (Starvation Threshold: 120s).
- **Deterministic State Machine**: Manages mandatory Yellow transitions (3s) to ensure safety, just like real-world signals.

## 3. Live Demonstration Steps
If you are showing the running system:
1. **Start in Fixed Mode**: Show the round-robin behavior (Lane 0 -> 1 -> 2 -> 3).
2. **Switch to Adaptive Mode**: Explain how the "Green Lane" now stays on the busiest lane.
3. **Simulate a Jam**: Place an object in a specific lane's ROI and show how the system extends the green light for that lane.
4. **Point out the Dashboard**: Show the Latency (ms) and FPS to prove it is running in real-time.

## 4. Architecture Summary (For Q&A)
If the instructor asks about the code structure:
- **Modular Design**: "I separated the code into `Detector`, `LaneManager`, and `SignalController` classes. This makes the system extensible—for example, we could swap the YOLO model for a different sensor without touching the signal logic."
- **Build System**: "I used CMake to manage dependencies and ensure cross-platform compatibility (Windows/Linux)."

---
**Instructor Note**: The system is designed to run at >10 FPS on standard CPU hardware, making it a cost-effective solution for smart city infrastructure.

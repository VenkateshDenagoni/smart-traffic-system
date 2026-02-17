# Smart Traffic Light System

A real-time, modular, and adaptive traffic signal controller built in C++ using OpenCV and Deep Learning.

## Project Overview
This system uses a lightweight object detection model to estimate vehicle density across multiple lanes and dynamically adjusts traffic signal timings using a **Max-Pressure** control algorithm with **Anti-Starvation** protection.

### Key Features
- **Real-time Detection**: Uses OpenCV DNN module for vehicle detection.
- **Modular Architecture**: 
  - `Detector`: Handles model inference and post-processing.
  - `LaneManager`: Maps detections to lane-specific ROIs and calculates density.
  - `SignalController`: Implements adaptive logic and state machine transitions.
- **Adaptive Control**: Prioritizes high-density lanes while ensuring no lane is starved of green time.
- **Performance Dashboard**: Real-time HUD showing FPS, latency, and signal states.
- **Deterministic Fallback**: Safely falls back to fixed-time cycles if detection becomes unavailable.

## Architecture
- **Perception Layer**: YOLO/MobileNet-SSD via OpenCV DNN.
- **Logic Layer**: ROI-based density estimation and Max-Pressure state machine.
- **Actuation Layer**: Real-time visualization and signal state management.

## Getting Started

### Prerequisites
- C++17 Compiler (g++)
- CMake 3.10+
- OpenCV 4.x (with DNN module)

### Build Instructions
```bash
mkdir build && cd build
cmake ..
make
```

### Running the System
```bash
# To run the main system
./traffic_system

# To run individual module tests
./test_detector
./test_lane_manager
./test_signal_controller
```

## Performance Analysis
- **Target FPS**: 10+
- **Observed Latency**: ~85ms - 95ms (on CPU)
- **Observed FPS**: ~11.5 FPS
- **Model**: MobileNet-SSD (Verified bridge) / YOLOv5n (Compatible with FP32 export)

## GitHub Repository
The latest source code is maintained at: [https://github.com/VenkateshDenagoni/smart-traffic-system](https://github.com/VenkateshDenagoni/smart-traffic-system)

## License
MIT License

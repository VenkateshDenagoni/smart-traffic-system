#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <fstream>

#include "Detector.hpp"
#include "LaneManager.hpp"
#include "SignalController.hpp"

// Global state for UI control
bool is_paused = false;

bool fileExists(const std::string& filename) {
    std::ifstream f(filename.c_str());
    return f.good();
}

int main(int argc, char** argv) {
    std::cout << "=================================================" << std::endl;
    std::cout << "   SMART TRAFFIC SYSTEM - DEBUG MODE            " << std::endl;
    std::cout << "=================================================" << std::endl;
    
    try {
        // 1. Initialize Modules
        std::string model_path = "../models/yolov8n.onnx";
        std::cout << "[DEBUG] Looking for model at: " << model_path << std::endl;
        
        if (!fileExists(model_path)) {
            std::cout << "[ERROR] Model file NOT FOUND!" << std::endl;
            std::cout << "Expected path: " << model_path << std::endl;
            std::cout << "Please make sure you downloaded 'yolov8n.onnx' and put it in the 'models' folder." << std::endl;
            
            // Try absolute path check as fallback
            std::cout << "[DEBUG] Checking local directory for model..." << std::endl;
            if (fileExists("yolov8n.onnx")) {
                model_path = "yolov8n.onnx";
                std::cout << "[DEBUG] Found model in current directory." << std::endl;
            } else {
                throw std::runtime_error("Cannot find yolov8n.onnx. Please check the 'models' folder.");
            }
        }
        
        std::cout << "[DEBUG] Loading model into OpenCV DNN..." << std::endl;
        Detector detector(model_path, true);
        std::cout << "[DEBUG] Model loaded successfully." << std::endl;
        
        LaneManager lane_manager;
        lane_manager.addLane(0, "North", cv::Rect(280, 50, 80, 200));
        lane_manager.addLane(1, "South", cv::Rect(280, 350, 80, 200));
        lane_manager.addLane(2, "East", cv::Rect(350, 280, 200, 80));
        lane_manager.addLane(3, "West", cv::Rect(50, 280, 200, 80));

        SignalController signal_controller(4);
        signal_controller.setMode(true); 

        // 2. Input Handling
        cv::VideoCapture cap;
        if (argc > 1) {
            std::cout << "[DEBUG] Opening video source: " << argv[1] << std::endl;
            cap.open(argv[1]);
        } else {
            std::cout << "[DEBUG] Opening default camera (0)..." << std::endl;
            cap.open(0);
        }

        if (!cap.isOpened()) {
            std::cout << "[WARNING] Could not open video source. Using dummy simulation." << std::endl;
        }

        cv::Mat frame;
        std::cout << "[DEBUG] Entering main loop. Press 'q' to quit." << std::endl;
        
        while (true) {
            if (!is_paused) {
                cap >> frame;
                if (frame.empty()) {
                    if (argc > 1) { // Loop video
                        cap.set(cv::CAP_PROP_POS_FRAMES, 0);
                        continue;
                    }
                    // If camera fails, use dummy
                    frame = cv::Mat::zeros(640, 640, CV_8UC3);
                    cv::putText(frame, "SIMULATION MODE (No Camera)", cv::Point(100, 320), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 255), 2);
                }
            }

            auto start_time = std::chrono::high_resolution_clock::now();
            std::vector<Detection> detections = detector.detect(frame);
            lane_manager.updateLanes(detections);

            std::map<int, float> densities;
            for (const auto& lane : lane_manager.getLanes()) {
                densities[lane.id] = lane.density;
            }
            signal_controller.update(densities, 0.033f); 

            cv::Mat display = frame.clone();
            lane_manager.drawLanes(display);
            for (const auto& det : detections) {
                cv::rectangle(display, det.box, cv::Scalar(0, 255, 0), 2);
            }

            Signal current_sig = signal_controller.getSignal(signal_controller.getCurrentGreenLane());
            std::string state_str = (current_sig.state == SignalState::GREEN) ? "GREEN" : "YELLOW";

            cv::rectangle(display, cv::Rect(10, 10, 280, 100), cv::Scalar(0, 0, 0), -1);
            cv::putText(display, "Signal: " + state_str, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.7, (current_sig.state == SignalState::GREEN ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 255, 255)), 2);
            cv::putText(display, "Lane: " + std::to_string(current_sig.lane_id) + " (" + std::to_string(current_sig.remaining_time) + "s)", cv::Point(20, 80), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 1);

            cv::imshow("Smart Traffic Dashboard", display);
            char key = (char)cv::waitKey(30);
            if (key == 'q' || key == 'Q') break;
            if (key == 'p' || key == 'P') is_paused = !is_paused;
        }
    } catch (const std::exception& e) {
        std::cerr << "\n[CRITICAL ERROR] " << e.what() << std::endl;
        std::cerr << "Check if all DLLs are in the build folder and models exist." << std::endl;
    }

    std::cout << "\n=================================================" << std::endl;
    std::cout << "Program finished. Press ENTER to exit..." << std::endl;
    std::cin.get(); // This will stop the terminal from closing immediately
    return 0;
}

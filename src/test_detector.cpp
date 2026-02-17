#include "Detector.hpp"
#include <iostream>
#include <chrono>

int main() {
    std::string model_path = "../models/mobilenet_iter_73000.caffemodel";
    Detector detector(model_path, false);

    // Create a dummy image (black image) to test inference
    cv::Mat frame = cv::Mat::zeros(640, 640, CV_8UC3);
    
    // Add a white rectangle to simulate an object (though YOLO won't detect it as a car)
    cv::rectangle(frame, cv::Rect(100, 100, 200, 200), cv::Scalar(255, 255, 255), -1);

    std::cout << "Starting inference test..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Detection> detections = detector.detect(frame);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> diff = end - start;
    
    std::cout << "Inference took: " << diff.count() << " seconds" << std::endl;
    std::cout << "FPS: " << 1.0 / diff.count() << std::endl;
    std::cout << "Detected " << detections.size() << " objects." << std::endl;

    for (const auto& d : detections) {
        std::cout << "Class: " << d.class_id << " Conf: " << d.confidence << " Box: " << d.box << std::endl;
    }

    return 0;
}

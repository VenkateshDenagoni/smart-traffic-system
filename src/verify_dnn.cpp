#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>

int main() {
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;
    
    // We will try to create an empty network first to check if DNN module is linked
    try {
        cv::dnn::Net net;
        std::cout << "Successfully created an empty cv::dnn::Net object." << std::endl;
        
        // Note: We don't have a real ONNX yet, but we can check if the function exists
        // and if it throws the expected 'file not found' error instead of a linker error.
        try {
            cv::dnn::readNetFromONNX("non_existent.onnx");
        } catch (const cv::Exception& e) {
            if (e.code == cv::Error::StsError || e.code == cv::Error::StsObjectNotFound) {
                std::cout << "OpenCV DNN module is working (correctly caught file-not-found exception)." << std::endl;
            } else {
                std::cerr << "Unexpected OpenCV error: " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}

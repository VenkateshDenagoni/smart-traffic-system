#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include <string>

struct Detection {
    int class_id;
    float confidence;
    cv::Rect box;
};

class Detector {
public:
    Detector(const std::string& model_path, bool is_yolov8 = false);
    std::vector<Detection> detect(cv::Mat& frame);

private:
    cv::dnn::Net net;
    bool is_yolov8;
    float conf_threshold = 0.45f;
    float nms_threshold = 0.40f;
    const std::vector<std::string> class_names = {"person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck"};

    cv::Mat preprocess(cv::Mat& frame);
    std::vector<Detection> postprocess(const std::vector<cv::Mat>& outputs, const cv::Size& original_size);
};

#endif

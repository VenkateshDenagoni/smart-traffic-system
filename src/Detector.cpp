#include "Detector.hpp"
#include <iostream>

Detector::Detector(const std::string& model_path, bool is_yolov8) : is_yolov8(is_yolov8) {
    if (model_path.find(".onnx") != std::string::npos) {
        net = cv::dnn::readNetFromONNX(model_path);
    } else {
        std::string proto = "../models/deploy.prototxt";
        net = cv::dnn::readNet(model_path, proto);
    }
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
}

cv::Mat Detector::preprocess(cv::Mat& frame) {
    cv::Mat blob;
    cv::dnn::blobFromImage(frame, blob, 1.0/255.0, cv::Size(640, 640), cv::Scalar(), true, false);
    return blob;
}

std::vector<Detection> Detector::detect(cv::Mat& frame) {
    cv::Mat blob = preprocess(frame);
    net.setInput(blob);
    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());
    return postprocess(outputs, frame.size());
}

std::vector<Detection> Detector::postprocess(const std::vector<cv::Mat>& outputs, const cv::Size& original_size) {
    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    if (outputs[0].dims == 4) {
        float* data = (float*)outputs[0].data;
        for (int i = 0; i < outputs[0].size[2]; ++i) {
            float confidence = data[2];
            if (confidence > conf_threshold) {
                int class_id = (int)data[1];
                int left = static_cast<int>(data[3] * original_size.width);
                int top = static_cast<int>(data[4] * original_size.height);
                int right = static_cast<int>(data[5] * original_size.width);
                int bottom = static_cast<int>(data[6] * original_size.height);
                class_ids.push_back(class_id);
                confidences.push_back(confidence);
                boxes.push_back(cv::Rect(left, top, right - left, bottom - top));
            }
            data += 7;
        }
    } else {
        cv::Mat output = outputs[0];
        if (output.dims == 3 && output.size[0] == 1) {
            output = cv::Mat(output.size[1], output.size[2], CV_32F, output.ptr<float>());
            cv::transpose(output, output);
        }

        float x_factor = original_size.width / 640.0;
        float y_factor = original_size.height / 640.0;
        float* data = (float*)output.data;
        const int rows = output.rows;

        for (int i = 0; i < rows; ++i) {
            float* classes_scores = data + 4;
            cv::Mat scores(1, class_names.size(), CV_32FC1, classes_scores);
            cv::Point class_id_ptr;
            double max_class_score;
            cv::minMaxLoc(scores, 0, &max_class_score, 0, &class_id_ptr);

            if (max_class_score > conf_threshold) {
                float cx = data[0];
                float cy = data[1];
                float w = data[2];
                float h = data[3];
                int left = int((cx - 0.5 * w) * x_factor);
                int top = int((cy - 0.5 * h) * y_factor);
                int width = int(w * x_factor);
                int height = int(h * y_factor);
                int cid = class_id_ptr.x;
                if (cid == 2 || cid == 3 || cid == 5 || cid == 7) {
                    confidences.push_back(max_class_score);
                    class_ids.push_back(cid);
                    boxes.push_back(cv::Rect(left, top, width, height));
                }
            }
            data += output.cols;
        }
    }

    std::vector<int> nms_indices;
    cv::dnn::NMSBoxes(boxes, confidences, conf_threshold, nms_threshold, nms_indices);
    std::vector<Detection> result;
    for (int idx : nms_indices) {
        Detection d;
        d.class_id = class_ids[idx];
        d.confidence = confidences[idx];
        d.box = boxes[idx];
        result.push_back(d);
    }
    return result;
}

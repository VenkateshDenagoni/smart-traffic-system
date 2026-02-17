#ifndef LANE_MANAGER_HPP
#define LANE_MANAGER_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "Detector.hpp"

struct Lane {
    int id;
    std::string name;
    cv::Rect roi;
    int vehicle_count;
    float density; // 0.0 to 1.0
};

class LaneManager {
public:
    LaneManager();
    void addLane(int id, const std::string& name, cv::Rect roi);
    void updateLanes(const std::vector<Detection>& detections);
    const std::vector<Lane>& getLanes() const { return lanes; }
    int getHighestDensityLaneId() const;
    void drawLanes(cv::Mat& frame);

private:
    std::vector<Lane> lanes;
    bool isPointInRect(cv::Point p, cv::Rect r);
};

#endif

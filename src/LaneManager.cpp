#include "LaneManager.hpp"

LaneManager::LaneManager() {}

void LaneManager::addLane(int id, const std::string& name, cv::Rect roi) {
    lanes.push_back({id, name, roi, 0, 0.0f});
}

bool LaneManager::isPointInRect(cv::Point p, cv::Rect r) {
    return (p.x >= r.x && p.x <= (r.x + r.width) && p.y >= r.y && p.y <= (r.y + r.height));
}

void LaneManager::updateLanes(const std::vector<Detection>& detections) {
    // Reset counts
    for (auto& lane : lanes) {
        lane.vehicle_count = 0;
        lane.density = 0.0f;
    }

    // Map detections to lanes
    for (const auto& det : detections) {
        // Use the center of the bounding box for lane assignment
        cv::Point center(det.box.x + det.box.width / 2, det.box.y + det.box.height / 2);
        
        for (auto& lane : lanes) {
            if (isPointInRect(center, lane.roi)) {
                lane.vehicle_count++;
                break; // Assume a vehicle belongs to only one lane
            }
        }
    }

    // Calculate density (simplified: count / max_capacity)
    // For this example, let's assume max capacity per lane ROI is 10 vehicles
    const int max_capacity = 10;
    for (auto& lane : lanes) {
        lane.density = std::min(1.0f, static_cast<float>(lane.vehicle_count) / max_capacity);
    }
}

int LaneManager::getHighestDensityLaneId() const {
    int best_id = -1;
    float max_density = -1.0f;
    for (const auto& lane : lanes) {
        if (lane.density > max_density) {
            max_density = lane.density;
            best_id = lane.id;
        }
    }
    return best_id;
}

void LaneManager::drawLanes(cv::Mat& frame) {
    for (const auto& lane : lanes) {
        cv::rectangle(frame, lane.roi, cv::Scalar(255, 255, 0), 2);
        std::string label = lane.name + ": " + std::to_string(lane.vehicle_count) + " (" + std::to_string(static_cast<int>(lane.density * 100)) + "%)";
        cv::putText(frame, label, cv::Point(lane.roi.x, lane.roi.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0), 1);
    }
}

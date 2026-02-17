#include "LaneManager.hpp"
#include <iostream>

int main() {
    LaneManager manager;

    // Define 4 lanes for a typical intersection
    manager.addLane(0, "North", cv::Rect(300, 0, 40, 300));
    manager.addLane(1, "South", cv::Rect(300, 340, 40, 300));
    manager.addLane(2, "East", cv::Rect(340, 300, 300, 40));
    manager.addLane(3, "West", cv::Rect(0, 300, 300, 40));

    // Simulate some detections
    std::vector<Detection> simulated_detections;
    // 3 cars in North lane
    simulated_detections.push_back({2, 0.9f, cv::Rect(310, 50, 20, 40)});
    simulated_detections.push_back({2, 0.8f, cv::Rect(310, 100, 20, 40)});
    simulated_detections.push_back({2, 0.85f, cv::Rect(310, 150, 20, 40)});
    
    // 5 cars in West lane
    for (int i = 0; i < 5; ++i) {
        simulated_detections.push_back({2, 0.9f, cv::Rect(50 + i*40, 310, 40, 20)});
    }

    manager.updateLanes(simulated_detections);

    std::cout << "Lane Status:" << std::endl;
    for (const auto& lane : manager.getLanes()) {
        std::cout << "Lane " << lane.name << " (ID: " << lane.id << "): " 
                  << lane.vehicle_count << " vehicles, Density: " 
                  << lane.density * 100 << "%" << std::endl;
    }

    int highest_id = manager.getHighestDensityLaneId();
    std::cout << "\nHighest Density Lane ID: " << highest_id << std::endl;

    return 0;
}

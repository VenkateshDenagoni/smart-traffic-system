#ifndef SIGNAL_CONTROLLER_HPP
#define SIGNAL_CONTROLLER_HPP

#include <vector>
#include <string>
#include <chrono>
#include <map>

enum class SignalState { GREEN, YELLOW, RED };

struct Signal {
    int lane_id;
    SignalState state;
    int remaining_time;
};

class SignalController {
public:
    SignalController(int num_lanes);
    
    void setMode(bool adaptive);
    void update(const std::map<int, float>& lane_densities, float delta_time);
    
    Signal getSignal(int lane_id) const;
    bool isAdaptive() const { return adaptive_mode; }
    int getCurrentGreenLane() const { return current_green_lane; }

private:
    bool adaptive_mode = false;
    int num_lanes;
    int current_green_lane = 0;
    SignalState current_state = SignalState::GREEN;
    float state_timer = 0.0f;

    // Timing Constraints (seconds)
    const float MIN_GREEN_TIME = 10.0f;
    const float MAX_GREEN_TIME = 60.0f;
    const float YELLOW_TIME = 3.0f;
    const float FIXED_GREEN_TIME = 30.0f;

    // Anti-starvation: Track time since last green for each lane
    std::vector<float> time_since_last_green;
    const float STARVATION_THRESHOLD = 120.0f;

    void handleFixedMode(float delta_time);
    void handleAdaptiveMode(const std::map<int, float>& lane_densities, float delta_time);
    void transitionToNextLane(const std::map<int, float>& lane_densities);
};

#endif

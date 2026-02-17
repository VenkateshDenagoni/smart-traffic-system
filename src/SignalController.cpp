#include "SignalController.hpp"
#include <algorithm>
#include <iostream>

SignalController::SignalController(int num_lanes) : num_lanes(num_lanes) {
    time_since_last_green.assign(num_lanes, 0.0f);
}

void SignalController::setMode(bool adaptive) {
    adaptive_mode = adaptive;
    std::cout << "Signal Mode set to: " << (adaptive ? "ADAPTIVE" : "FIXED") << std::endl;
}

void SignalController::update(const std::map<int, float>& lane_densities, float delta_time) {
    state_timer += delta_time;
    
    // Update starvation timers for all lanes except the current green one
    for (int i = 0; i < num_lanes; ++i) {
        if (i != current_green_lane || current_state != SignalState::GREEN) {
            time_since_last_green[i] += delta_time;
        } else {
            time_since_last_green[i] = 0.0f;
        }
    }

    if (adaptive_mode) {
        handleAdaptiveMode(lane_densities, delta_time);
    } else {
        handleFixedMode(delta_time);
    }
}

void SignalController::handleFixedMode(float delta_time) {
    if (current_state == SignalState::GREEN) {
        if (state_timer >= FIXED_GREEN_TIME) {
            current_state = SignalState::YELLOW;
            state_timer = 0.0f;
        }
    } else if (current_state == SignalState::YELLOW) {
        if (state_timer >= YELLOW_TIME) {
            current_state = SignalState::GREEN;
            current_green_lane = (current_green_lane + 1) % num_lanes;
            state_timer = 0.0f;
        }
    }
}

void SignalController::handleAdaptiveMode(const std::map<int, float>& lane_densities, float delta_time) {
    if (current_state == SignalState::GREEN) {
        // Minimum green time must be respected
        if (state_timer >= MIN_GREEN_TIME) {
            float current_density = lane_densities.at(current_green_lane);
            
            // Check if we should switch: 
            // 1. Max green time reached
            // 2. Current lane is empty AND others are waiting
            // 3. Starvation detected in another lane
            bool switch_needed = (state_timer >= MAX_GREEN_TIME);
            
            if (!switch_needed) {
                for (int i = 0; i < num_lanes; ++i) {
                    if (i != current_green_lane && time_since_last_green[i] > STARVATION_THRESHOLD) {
                        switch_needed = true;
                        break;
                    }
                }
            }

            if (!switch_needed && current_density < 0.1f) {
                for (int i = 0; i < num_lanes; ++i) {
                    if (i != current_green_lane && lane_densities.at(i) > 0.2f) {
                        switch_needed = true;
                        break;
                    }
                }
            }

            if (switch_needed) {
                current_state = SignalState::YELLOW;
                state_timer = 0.0f;
            }
        }
    } else if (current_state == SignalState::YELLOW) {
        if (state_timer >= YELLOW_TIME) {
            transitionToNextLane(lane_densities);
            current_state = SignalState::GREEN;
            state_timer = 0.0f;
        }
    }
}

void SignalController::transitionToNextLane(const std::map<int, float>& lane_densities) {
    // Max-Pressure Logic with Starvation Override
    int next_lane = -1;
    
    // 1. Check for starvation first (highest priority)
    float max_starvation = 0.0f;
    for (int i = 0; i < num_lanes; ++i) {
        if (time_since_last_green[i] > STARVATION_THRESHOLD && time_since_last_green[i] > max_starvation) {
            max_starvation = time_since_last_green[i];
            next_lane = i;
        }
    }

    // 2. If no starvation, pick lane with highest pressure (density)
    if (next_lane == -1) {
        float max_pressure = -1.0f;
        for (int i = 0; i < num_lanes; ++i) {
            if (lane_densities.at(i) > max_pressure) {
                max_pressure = lane_densities.at(i);
                next_lane = i;
            }
        }
    }

    // Fallback to round-robin if somehow no lane is picked
    if (next_lane == -1) {
        next_lane = (current_green_lane + 1) % num_lanes;
    }

    current_green_lane = next_lane;
    time_since_last_green[current_green_lane] = 0.0f;
}

Signal SignalController::getSignal(int lane_id) const {
    Signal s;
    s.lane_id = lane_id;
    if (lane_id == current_green_lane) {
        s.state = current_state;
        if (current_state == SignalState::GREEN) 
            s.remaining_time = static_cast<int>(std::max(0.0f, (adaptive_mode ? MAX_GREEN_TIME : FIXED_GREEN_TIME) - state_timer));
        else 
            s.remaining_time = static_cast<int>(std::max(0.0f, YELLOW_TIME - state_timer));
    } else {
        s.state = SignalState::RED;
        s.remaining_time = -1; // Unknown in adaptive
    }
    return s;
}

#include "SignalController.hpp"
#include <iostream>
#include <map>
#include <iomanip>

void printStatus(const SignalController& controller, int num_lanes) {
    std::cout << "Current Green Lane: " << controller.getCurrentGreenLane() << " | State: ";
    Signal s = controller.getSignal(controller.getCurrentGreenLane());
    if (s.state == SignalState::GREEN) std::cout << "GREEN";
    else if (s.state == SignalState::YELLOW) std::cout << "YELLOW";
    std::cout << " | Remaining: " << s.remaining_time << "s" << std::endl;
}

int main() {
    int num_lanes = 4;
    SignalController controller(num_lanes);
    std::map<int, float> densities = {{0, 0.1f}, {1, 0.8f}, {2, 0.1f}, {3, 0.1f}};

    std::cout << "--- Testing Fixed Mode ---" << std::endl;
    controller.setMode(false);
    for (int i = 0; i < 35; i += 5) {
        std::cout << "Time: " << i << "s | ";
        printStatus(controller, num_lanes);
        controller.update(densities, 5.0f);
    }

    std::cout << "\n--- Testing Adaptive Mode (Max Pressure) ---" << std::endl;
    controller.setMode(true);
    // Lane 1 has 80% density, should be prioritized
    for (int i = 0; i < 20; i += 5) {
        std::cout << "Time: " << i << "s | ";
        printStatus(controller, num_lanes);
        controller.update(densities, 5.0f);
    }

    std::cout << "\n--- Testing Anti-Starvation ---" << std::endl;
    // Keep Lane 1 high density, but simulate 130s passing to trigger starvation for Lane 0
    densities = {{0, 0.05f}, {1, 0.9f}, {2, 0.05f}, {3, 0.05f}};
    std::cout << "Simulating heavy traffic on Lane 1 and starvation on others..." << std::endl;
    
    // Fast forward time
    controller.update(densities, 130.0f);
    printStatus(controller, num_lanes);
    
    // Now Lane 0 should eventually get green even if density is low
    std::cout << "Triggering transition..." << std::endl;
    // Force current green to end by fast forwarding
    controller.update(densities, 60.0f); 
    printStatus(controller, num_lanes);
    
    return 0;
}

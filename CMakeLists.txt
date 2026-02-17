cmake_minimum_required(VERSION 3.10)
project(SmartTrafficSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Windows OpenCV Helper ---
# If you are on Windows, uncomment the line below and point it to your OpenCV build folder
# set(OpenCV_DIR "C:/opencv/build") 

# Find OpenCV
find_package(OpenCV REQUIRED COMPONENTS core imgproc dnn videoio highgui)

if(OpenCV_FOUND)
    message(STATUS "OpenCV found: ${OpenCV_VERSION}")
    include_directories(${OpenCV_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "OpenCV not found. Please set OpenCV_DIR to your OpenCV build directory.")
endif()

include_directories(include)

# Detector Library
add_library(detector_lib src/Detector.cpp)
target_link_libraries(detector_lib ${OpenCV_LIBS})

# LaneManager Library
add_library(lane_manager_lib src/LaneManager.cpp)
target_link_libraries(lane_manager_lib ${OpenCV_LIBS})

# SignalController Library
add_library(signal_controller_lib src/SignalController.cpp)

# Main Application
add_executable(traffic_system src/main.cpp)
target_link_libraries(traffic_system 
    detector_lib 
    lane_manager_lib 
    signal_controller_lib 
    ${OpenCV_LIBS}
)

# Individual Tests
add_executable(verify_dnn src/verify_dnn.cpp)
target_link_libraries(verify_dnn ${OpenCV_LIBS})

add_executable(test_detector src/test_detector.cpp)
target_link_libraries(test_detector detector_lib ${OpenCV_LIBS})

add_executable(test_lane_manager src/test_lane_manager.cpp)
target_link_libraries(test_lane_manager lane_manager_lib detector_lib ${OpenCV_LIBS})

add_executable(test_signal_controller src/test_signal_controller.cpp)
target_link_libraries(test_signal_controller signal_controller_lib)

# --- Windows DLL Copy ---
# This helper copies OpenCV DLLs to the build folder so the .exe can actually run
if(WIN32)
    add_custom_command(TARGET traffic_system POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${OpenCV_BIN_DIR}/opencv_world${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll"
        $<TARGET_FILE_DIR:traffic_system>)
endif()

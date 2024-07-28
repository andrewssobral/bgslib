cmake_minimum_required(VERSION 3.10)
project(bgslib)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include)

add_library(bgslib INTERFACE)
target_include_directories(bgslib INTERFACE include)

add_executable(list_algorithms examples/list_algorithms.cpp)
target_link_libraries(list_algorithms bgslib ${OpenCV_LIBS})

add_executable(update_params examples/update_params.cpp)
target_link_libraries(update_params bgslib ${OpenCV_LIBS})

add_executable(camera_stream examples/camera_stream.cpp)
target_link_libraries(camera_stream bgslib ${OpenCV_LIBS})

add_executable(interactive_camera_stream examples/interactive_camera_stream.cpp)
target_link_libraries(interactive_camera_stream bgslib ${OpenCV_LIBS})

add_executable(performance_metrics examples/performance_metrics.cpp)
target_link_libraries(performance_metrics bgslib ${OpenCV_LIBS})

add_executable(frame_difference_stream demos/frame_difference_stream.cpp)
target_link_libraries(frame_difference_stream bgslib ${OpenCV_LIBS})

add_executable(adaptive_background_learning_stream demos/adaptive_background_learning_stream.cpp)
target_link_libraries(adaptive_background_learning_stream bgslib ${OpenCV_LIBS})

add_executable(adaptive_selective_bg_learning_stream demos/adaptive_selective_bg_learning_stream.cpp)
target_link_libraries(adaptive_selective_bg_learning_stream bgslib ${OpenCV_LIBS})

add_executable(weighted_moving_mean_stream demos/weighted_moving_mean_stream.cpp)
target_link_libraries(weighted_moving_mean_stream bgslib ${OpenCV_LIBS})

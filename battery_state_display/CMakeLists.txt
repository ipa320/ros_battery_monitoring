cmake_minimum_required(VERSION 3.8)
project(battery_state_display)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rviz_2d_overlay_msgs REQUIRED)
find_package(fmt REQUIRED)

add_executable(battery_state_display src/main.cpp src/BatteryStateDisplay.cpp)
ament_target_dependencies(battery_state_display rclcpp sensor_msgs rviz_2d_overlay_msgs)
target_include_directories(battery_state_display PRIVATE include)
target_link_libraries(battery_state_display fmt::fmt)


install(
  TARGETS battery_state_display
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()

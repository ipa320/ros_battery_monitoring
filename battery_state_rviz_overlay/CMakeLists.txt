cmake_minimum_required(VERSION 3.8)
project(battery_state_rviz_overlay)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# add_compile_options(-v)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rviz_2d_overlay_msgs REQUIRED)
find_package(fmt REQUIRED)
find_package(generate_parameter_library REQUIRED)

# Parameters Library ================================================
set(TARGET ${PROJECT_NAME}_parameters)
generate_parameter_library(${TARGET} src/${TARGET}.yaml)

add_executable(battery_state_rviz_overlay src/main.cpp src/BatteryStateDisplay.cpp)
ament_target_dependencies(battery_state_rviz_overlay rclcpp sensor_msgs rviz_2d_overlay_msgs)

target_include_directories(battery_state_rviz_overlay PUBLIC
  include
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/battery_state_rviz_overlay>
)

target_link_libraries(${PROJECT_NAME} battery_state_rviz_overlay_parameters fmt::fmt)

install(
  TARGETS
  battery_state_rviz_overlay
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()

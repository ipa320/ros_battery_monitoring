cmake_minimum_required(VERSION 3.8)
project(battery_state_broadcaster)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(THIS_PACKAGE_INCLUDE_DEPENDS
  controller_interface
  pluginlib
  realtime_tools
  sensor_msgs
)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(generate_parameter_library REQUIRED)

foreach(dependency IN ITEMS ${THIS_PACKAGE_INCLUDE_DEPENDS})
  find_package(${dependency} REQUIRED)
endforeach()

# Parameters Library ================================================
set(TARGET ${PROJECT_NAME}_parameters)
generate_parameter_library(${TARGET} src/battery_state_broadcaster_parameters.yaml)

add_library(battery_state_broadcaster SHARED src/BatteryStateBroadcaster.cpp)
target_include_directories(battery_state_broadcaster PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/battery_state_broadcaster>
)

target_link_libraries(${PROJECT_NAME} PUBLIC battery_state_broadcaster_parameters)
ament_target_dependencies(battery_state_broadcaster PUBLIC ${THIS_PACKAGE_INCLUDE_DEPENDS})

pluginlib_export_plugin_description_file(controller_interface battery_state_broadcaster.xml)

install(
  DIRECTORY include/
  DESTINATION include/battery_state_broadcaster
)
install(
  TARGETS
  battery_state_broadcaster_parameters
  battery_state_broadcaster
  EXPORT export_battery_state_broadcaster
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  INCLUDES DESTINATION include
)

ament_export_targets(export_battery_state_broadcaster HAS_LIBRARY_TARGET)
ament_export_dependencies(${THIS_PACKAGE_INCLUDE_DEPENDS})

ament_package()

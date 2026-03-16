#include <gmock/gmock.h>
#include <memory>

#include "controller_manager/controller_manager.hpp"
#include "hardware_interface/resource_manager.hpp"
#include "rclcpp/executor.hpp"
#include "rclcpp/executors/single_threaded_executor.hpp"
#include "rclcpp/utilities.hpp"
#include "ros2_control_test_assets/descriptions.hpp"

TEST(TestLoadBatteryStateBroadcaster, load_controller)
{
  std::shared_ptr<rclcpp::Executor> executor = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();

  controller_manager::ControllerManager cm(
      std::make_unique<hardware_interface::ResourceManager>(ros2_control_test_assets::minimal_robot_urdf),
      executor,
      "test_controller_manager");

  ASSERT_NE(cm.load_controller("test_battery_state_broadcaster", "battery_state_broadcaster/BatteryStateBroadcaster"),
            nullptr);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleMock(&argc, argv);
  rclcpp::init(argc, argv);
  int result = RUN_ALL_TESTS();
  rclcpp::shutdown();
  return result;
}

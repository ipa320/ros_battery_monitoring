#include "battery_state_rviz_overlay/BatteryStateDisplay.hpp"
#include <memory>
#include <rclcpp/executors.hpp>
#include <rclcpp/utilities.hpp>

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<BatteryStateDisplay>("battery_state_rviz_overlay");
  rclcpp::spin(node);
  rclcpp::shutdown();
}

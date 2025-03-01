#include "battery_state_rviz_overlay/BatteryStateDisplay.hpp"
#include <memory>
#include <rclcpp/executors.hpp>
#include <rclcpp/utilities.hpp>

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<battery_state_rviz_overlay::BatteryStateDisplay>("battery_state_rviz_overlay");

  try {
    auto param_listener = std::make_shared<battery_state_rviz_overlay::ParamListener>(node);
    node->setParamListener(param_listener);
  } catch (const std::exception & e) {
    RCLCPP_ERROR(
      node->get_logger(), "Exception thrown during BatteryStateDisplay init stage with message: %s \n",
      e.what());
  }

  rclcpp::spin(node);
  rclcpp::shutdown();
}

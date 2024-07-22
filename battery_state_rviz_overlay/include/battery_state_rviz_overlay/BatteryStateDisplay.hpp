#pragma once
#include <rclcpp/node.hpp>
#include <rclcpp/subscription.hpp>
#include <rviz_2d_overlay_msgs/msg/detail/overlay_text__struct.hpp>
#include <rviz_2d_overlay_msgs/msg/overlay_text.hpp>
#include <sensor_msgs/msg/battery_state.hpp>
#include <sensor_msgs/msg/detail/battery_state__struct.hpp>

class BatteryStateDisplay : public rclcpp::Node
{
public:
  explicit BatteryStateDisplay(const std::string& name);

private:
  rclcpp::Publisher<rviz_2d_overlay_msgs::msg::OverlayText>::SharedPtr overlayPublisher_;

  rclcpp::Subscription<sensor_msgs::msg::BatteryState>::SharedPtr batteryStateSubscription_;

  void batteryStateCallback(const sensor_msgs::msg::BatteryState& message);
};

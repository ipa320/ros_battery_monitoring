#include "battery_state_rviz_overlay/BatteryStateDisplay.hpp"
#include <cmath>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <rclcpp/qos.hpp>
#include <rviz_2d_overlay_msgs/msg/detail/overlay_text__struct.hpp>
#include <rviz_2d_overlay_msgs/msg/overlay_text.hpp>
#include <sensor_msgs/msg/battery_state.hpp>

BatteryStateDisplay::BatteryStateDisplay(const std::string& name)
  : rclcpp::Node(name)
  , overlayPublisher_{ create_publisher<rviz_2d_overlay_msgs::msg::OverlayText>("battery_display_text",
                                                                                rclcpp::SystemDefaultsQoS()) }
  , batteryStateSubscription_{ create_subscription<sensor_msgs::msg::BatteryState>(
        "battery_state",
        1,
        [this](const sensor_msgs::msg::BatteryState& message) { batteryStateCallback(message); }) }
{
}

bool isNormal(double d)
{
  return d == 0.0 || std::isnormal(d);
}

void BatteryStateDisplay::batteryStateCallback(const sensor_msgs::msg::BatteryState& message)
{
  using rviz_2d_overlay_msgs::msg::OverlayText;
  OverlayText overlay;
  overlay.action = OverlayText::ADD;
  overlay.text = "<pre>";

  overlay.width = 450;
  overlay.height = 10;

  constexpr auto line_height = 25;

  overlay.text += fmt::format(FMT_COMPILE("Battery voltage: {:>8.2f}V\n"), message.voltage);
  overlay.height += line_height;

  if (isNormal(message.current))
  {
    overlay.text += fmt::format(FMT_COMPILE("Current:       {:>8.2f}A\n"), message.current);
    overlay.height += line_height;
  }

  if (isNormal(message.percentage))
  {
    overlay.text += fmt::format(FMT_COMPILE("Percent SOC:     {:>8.2f}%\n"), message.percentage);
    overlay.height += line_height;
  }

  overlay.horizontal_distance = 20;
  overlay.vertical_distance = 20;
  overlay.horizontal_alignment = OverlayText::LEFT;
  overlay.vertical_alignment = OverlayText::TOP;

  overlay.bg_color.a = 0.5;

  overlay.text_size = 20.0;
  overlay.font = "DejaVu Sans Mono";
  overlay.fg_color.a = 1.0;
  overlay.fg_color.r = 0;
  overlay.fg_color.g = 0.576470588;
  overlay.fg_color.b = 0.454901961;

  overlayPublisher_->publish(overlay);
}

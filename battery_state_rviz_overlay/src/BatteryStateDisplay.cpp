#include "battery_state_rviz_overlay/BatteryStateDisplay.hpp"
#include <cmath>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <rclcpp/qos.hpp>
#include <rviz_2d_overlay_msgs/msg/detail/overlay_text__struct.hpp>
#include <rviz_2d_overlay_msgs/msg/overlay_text.hpp>
#include <sensor_msgs/msg/battery_state.hpp>

namespace battery_state_rviz_overlay
{
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

  params_ = param_listener_->get_params();

  overlay.width = params_.width;
  overlay.height = params_.height;

  auto line_height = params_.line_height;

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

  overlay.horizontal_distance = params_.horizontal_distance;
  overlay.vertical_distance = params_.vertical_distance;
  overlay.horizontal_alignment = params_.horizontal_alignment;
  overlay.vertical_alignment = params_.vertical_alignment;

  overlay.bg_color.a = params_.bg_color_a;

  overlay.text_size = params_.text_size;
  overlay.font = params_.font;

  try {
    std::vector<double> rgba = getRgba();

    overlay.fg_color.a = rgba[3];
    overlay.fg_color.r = rgba[0];
    overlay.fg_color.g = rgba[1];
    overlay.fg_color.b = rgba[2];
  
  } catch (const std::exception & e) {

    overlay.fg_color.a = 1.0;
    overlay.fg_color.r = 0;
    overlay.fg_color.g = 0.576470588;
    overlay.fg_color.b = 0.454901961;
  
    RCLCPP_ERROR(
      get_logger(), "Exception thrown while parsing RGBA parameter fg_color_rgba: %s\n%s \n",
      params_.fg_color_rgba.c_str(),
      e.what());
  }

  overlayPublisher_->publish(overlay);
}

std::vector<double> BatteryStateDisplay::getRgba()
{
  //const std::string s_rgba = params_.fg_color_rgba;
  std::stringstream ss(params_.fg_color_rgba);
  std::vector<double> double_array;
  std::string value;

  while (std::getline(ss, value, ' ')) {
    double_array.push_back(std::stod(value));
  }
  return double_array;
}
}  // namespace battery_state_rviz_overlay

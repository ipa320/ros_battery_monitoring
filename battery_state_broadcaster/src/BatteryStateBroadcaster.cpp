#include "battery_state_broadcaster/BatteryStateBroadcaster.hpp"
#include <cstdint>
#include <rclcpp/logging.hpp>
#include <rclcpp/qos.hpp>

//
// Some code from
// https://github.com/HarvestX/h6x_ros2_controllers/blob/humble/battery_state_broadcaster/src/battery_state_broadcaster.cpp
//

namespace battery_state_broadcaster
{
controller_interface::CallbackReturn BatteryStateBroadcaster::on_init()
{
  try
  {
    param_listener_ = std::make_shared<ParamListener>(get_node());
    params_ = param_listener_->get_params();
  }
  catch (const std::exception& e)
  {
    RCLCPP_ERROR(get_node()->get_logger(), "Exception thrown during init stage with message: %s \n", e.what());
    return CallbackReturn::ERROR;
  }

  return CallbackReturn::SUCCESS;
}

controller_interface::CallbackReturn
BatteryStateBroadcaster::on_configure(const rclcpp_lifecycle::State& /*previous_state*/)
{
  std::string sensor_name = get_node()->get_parameter("sensor_name").as_string();

  params_ = param_listener_->get_params();

  battery_sensor_ = std::make_unique<BatterySensor>(BatterySensor(sensor_name, params_.state_interfaces));

  try
  {
    // register ft sensor data publisher
    battery_state_pub_ =
        get_node()->create_publisher<sensor_msgs::msg::BatteryState>("~/battery_state", rclcpp::SystemDefaultsQoS());
    realtime_publisher_ = std::make_unique<StatePublisher>(battery_state_pub_);
  }
  catch (const std::exception& e)
  {
    RCLCPP_ERROR(get_node()->get_logger(),
                 "Exception thrown during publisher creation at configure stage with message : %s \n",
                 e.what());
    return CallbackReturn::ERROR;
  }

  realtime_publisher_->lock();

  // seed the message with static values from parameters and defaults for dynamic members:
  realtime_publisher_->msg_.header.frame_id = params_.frame_id;
  realtime_publisher_->msg_.design_capacity = params_.design_capacity;
  realtime_publisher_->msg_.voltage = std::numeric_limits<float>::quiet_NaN();
  realtime_publisher_->msg_.temperature = std::numeric_limits<float>::quiet_NaN();
  realtime_publisher_->msg_.charge = std::numeric_limits<float>::quiet_NaN();
  realtime_publisher_->msg_.current = std::numeric_limits<float>::quiet_NaN();
  realtime_publisher_->msg_.capacity = std::numeric_limits<float>::quiet_NaN();
  realtime_publisher_->msg_.percentage = std::numeric_limits<float>::quiet_NaN();
  realtime_publisher_->msg_.power_supply_status = sensor_msgs::msg::BatteryState::POWER_SUPPLY_STATUS_UNKNOWN;
  realtime_publisher_->msg_.power_supply_health = sensor_msgs::msg::BatteryState::POWER_SUPPLY_HEALTH_UNKNOWN;
  realtime_publisher_->msg_.power_supply_technology = params_.power_supply_technology;
  realtime_publisher_->msg_.present = true;
  realtime_publisher_->msg_.location = params_.location;
  realtime_publisher_->msg_.serial_number = params_.serial_number;

  realtime_publisher_->unlock();

  RCLCPP_DEBUG(get_node()->get_logger(), "on_configure() successful");

  return CallbackReturn::SUCCESS;
}

[[nodiscard]] controller_interface::InterfaceConfiguration
BatteryStateBroadcaster::command_interface_configuration() const
{
  controller_interface::InterfaceConfiguration command_interfaces_config;
  command_interfaces_config.type = controller_interface::interface_configuration_type::NONE;
  return command_interfaces_config;
}

[[nodiscard]] controller_interface::InterfaceConfiguration BatteryStateBroadcaster::state_interface_configuration() const
{
  controller_interface::InterfaceConfiguration state_interfaces_config;
  state_interfaces_config.type = controller_interface::interface_configuration_type::INDIVIDUAL;
  state_interfaces_config.names = battery_sensor_->get_state_interface_names();
  return state_interfaces_config;
}

controller_interface::CallbackReturn
BatteryStateBroadcaster::on_activate(const rclcpp_lifecycle::State& /*previous_state*/)
{
  battery_sensor_->assign_loaned_state_interfaces(state_interfaces_);
  return CallbackReturn::SUCCESS;
}

controller_interface::CallbackReturn
BatteryStateBroadcaster::on_deactivate(const rclcpp_lifecycle::State& /*previous_state*/)
{
  battery_sensor_->release_interfaces();
  return CallbackReturn::SUCCESS;
}

controller_interface::return_type BatteryStateBroadcaster::update(const rclcpp::Time& time,
                                                                  const rclcpp::Duration& /*period*/)
{
  if (realtime_publisher_ && realtime_publisher_->trylock())
  {
    realtime_publisher_->msg_.header.stamp = time;
    battery_sensor_->populate_message_from_interfaces(realtime_publisher_->msg_);
    realtime_publisher_->unlockAndPublish();
  }

  return controller_interface::return_type::OK;
}

}  // namespace battery_state_broadcaster

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(battery_state_broadcaster::BatteryStateBroadcaster, controller_interface::ControllerInterface)

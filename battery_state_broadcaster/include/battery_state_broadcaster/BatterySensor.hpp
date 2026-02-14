
#include <limits>
#include <semantic_components/semantic_component_interface.hpp>
#include <sensor_msgs/msg/battery_state.hpp>
#include <sensor_msgs/msg/detail/battery_state__struct.hpp>

namespace battery_state_broadcaster
{
class BatterySensor : public semantic_components::SemanticComponentInterface<sensor_msgs::msg::BatteryState>
{
public:
  explicit BatterySensor(const std::string& name)
    : semantic_components::SemanticComponentInterface<sensor_msgs::msg::BatteryState>(name, 1)
  {
    interface_names_.emplace_back(name_ + "/" + "voltage");
    interface_names_.emplace_back(name_ + "/" + "current");
  }

  virtual ~BatterySensor() = default;

  double get_voltage()
  {
    voltage_ = state_interfaces_[0].get().get_optional().value();
    return voltage_;
  }

  double get_current()
  {
    current_ = state_interfaces_[1].get().get_value();
    return current_;
  }

  bool get_values_as_message(sensor_msgs::msg::BatteryState& message)
  {
    get_voltage();
    get_current();
    
    message.voltage = static_cast<float>(voltage_);
    message.percentage = calculate_percentage();
    message.current = static_cast<float>(current_);
    
    return true;
  }

  float calculate_percentage()
  {
    if (voltage_ >= batteryTable.front().voltage) {
      return 100.0f;
    }
    if (voltage_ <= batteryTable.back().voltage) {
      return 0.0f;
    }

    for (size_t i = 0; i < batteryTable.size() -1 ; ++i) {
      if (voltage_ <= batteryTable[i].voltage && voltage_ >= batteryTable[i + 1].voltage) {
        float vHigh = batteryTable[i].voltage;
        float pHigh = batteryTable[i].percentage;
        float vLow = batteryTable[i + 1].voltage;
        float pLow = batteryTable[i + 1].percentage;

        // Linear interpolation
        return pLow + (pHigh - pLow) * (voltage_ - vLow) / (vHigh - vLow);
      }
    }

    return 0.0f; // Default return value if no match found
  }

  

private:
  double voltage_ = 0.0;
  double current_ = 0.0;
  double percentage_ = 0.0;

  struct VoltageMap {
    float voltage;
    float percentage;
  };
  
  // Based on the 20°C - 30°C curve from your Haisic manual
  const std::vector<VoltageMap> batteryTable = {
      {13.4f, 100.0f},
      {13.2f, 90.0f},
      {13.1f, 70.0f}, // The start of the flat plateau
      {13.0f, 40.0f}, // Mid-plateau
      {12.8f, 20.0f}, // The "knee" where voltage drops fast
      {12.0f, 10.0f}, // Critical level
      {10.0f, 0.0f}   // Empty
  };
};
}  // namespace battery_state_broadcaster

#ifndef H_ABSTRACT_DRIVER
#define H_ABSTRACT_DRIVER

#include <cstdint>
#include <string>
#include <map>
#include <any>

enum class DRIVER_TYPE : std::uint8_t
{
	/**
	 * @brief Sensors and anything that doesn't require a write interface to control systems
	 *
	 */
	READ,

	/**
	 * @brief Thrusters, servos, and anything that doesn't provide useful data back
	 *
	 */
	WRITE,

	/**
	 * @brief Companions and any component that accepts both control inputs as well as provides useful data
	 *
	 */
	READWRITE
};

enum class DRIVER_SIGNATURE : std::uint8_t
{
	UNKNOWN,

	/**
	 * @brief Anything that can be treated as a thruster; this includes ESCs that control thrusters
	 *
	 */
	THRUSTER,
	IMU,

	/**
	 * @brief Generic servo. Prefer THRUSTER over this for thrusters.
	 *
	 */
	SERVO,
	CAMERA
};

enum class DRIVER_CONNECTION_STATUS : std::uint8_t
{
	FAILED,
	SUCCEEDED
};

namespace common_data_headers
{
// Common IMU Values

const std::string LINEAR_ACCELERATION3_ms2 = "linear_acceleration3_m/s^2";
const std::string ANGULAR_ACCELERATION3_rads2 = "angular_acceleration3_rad/s^2";
const std::string MAGNETOMETER_T = "magnetometer_T";

// Common ESC Values

const std::string PWM_us = "pwm_us";

// Generic component values

const std::string PIN = "gpio_pin";
const std::string TEMPERATURE_C = "temp_C";

} // namespace common_data_headers

typedef std::map<std::string, std::any> driver_packet_t;

class abstract_driver_t
{
public:
	explicit abstract_driver_t(DRIVER_TYPE type) { m_type = type; }

	~abstract_driver_t();

	// Mess-prevention
	abstract_driver_t(const abstract_driver_t &) = delete;
	abstract_driver_t &operator=(const abstract_driver_t &) = delete;

	/**
	 * @brief Up to the discretion of the driver what data to return
	 *
	 * @return driver_packet_t Map of all the data this driver wishes to provide
	 */
	virtual auto read() -> driver_packet_t = 0;

	/**
	 * @brief Write data to the driver. It is up to the discretion of the driver what to do with this data.
	 *
	 * @param what The packet to write
	 */
	virtual void write(driver_packet_t what) = 0;

	/**
	 * @brief Attempt to initialize this driver
	 *
	 * @param args The arguments to initialize the driver with (e.g. gpio pin)
	 *
	 * @return DRIVER_CONNECTION_STATUS
	 */
	virtual auto start(driver_packet_t args) -> DRIVER_CONNECTION_STATUS = 0;

	/**
	 * @brief Terminate this driver and instruct it to close its connection on a device
	 *
	 */
	virtual void stop() = 0;

private:
	DRIVER_TYPE m_type;
};

#endif // H_ABSTRACT_DRIVER
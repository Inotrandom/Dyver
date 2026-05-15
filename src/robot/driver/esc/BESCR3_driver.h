#ifndef BESCR3_H
#define BESCR3_H

#include "robot/driver/thruster_driver.h"
#include "BESCR3_T200.h"
#include "utils.h"
#include <mutex>
#include <thread>

#include "pigpio.h"

#define BESCR3_NEUTRAL_SIGNAL 1500
#define BESCR3_OFF_SIGNAL 0

enum BESCR3_CONFIG
{
	/**
	 * @brief Assume a T200 thruster is connected to the ESC
	 *
	 */
	T200
};

class bescr3_t : thruster_driver_t
{
public:
	explicit bescr3_t()
	{
		m_data = std::make_shared<t200_data_t>();
		m_data->load(m_voltage);

		m_handler = std::thread(&bescr3_t::driver_handler, this);
		m_handler.detach();
	}
	explicit bescr3_t(VOLTAGE_MODE voltage, BESCR3_CONFIG mode = BESCR3_CONFIG::T200)
	{
		m_voltage = voltage;
		m_mode = mode;

		m_data = std::make_shared<t200_data_t>();
		m_data->load(m_voltage);

		m_handler = std::thread(&bescr3_t::driver_handler, this);
		m_handler.detach();
	}
	~bescr3_t()
	{
		stop();
		m_should_close = true;
		if (m_handler.joinable())
		{
			m_handler.join();
		}
	}

	void configure_min_max_force()
	{
		if (m_data == nullptr)
		{
			return;
		}

		switch (m_mode)
		{
		case (BESCR3_CONFIG::T200):
		{
			m_force_max_kgf = (m_data->get_force_max_kgf());
			m_force_min_kgf = (m_data->get_force_min_kgf());
		}
		}
	}

	auto init() -> int override
	{
		write_instruction(BESCR3_NEUTRAL_SIGNAL);
		time_sleep(1.0);
		return 0;
	}

	void stop() override { write_instruction(BESCR3_OFF_SIGNAL); }

	/**
	 * @brief Set throttle
	 *
	 * @param t value from 0.0 to 1.0 describing a throttle value for this ESC
	 * @return int 0 is for a successful execution, -1 is for errors
	 */
	auto set_throttle(double t) -> int override
	{
		if (t < 0.0 || t > 1.0)
		{
			return -1;
		}
		utils::linear_percentage_t throttle = utils::linear_percentage_t(m_force_min_kgf, m_force_max_kgf);
		double requested_force = throttle.to_value(t);

		switch (m_mode)
		{
		case (BESCR3_CONFIG::T200):
		{
			std::size_t pwm_instruction_us = m_data->get_pwm_us_from_force_kgf_approx(requested_force);
			if (pwm_instruction_us <= 0)
			{
				return -1;
			}

			write_instruction(pwm_instruction_us);
		}
		}

		return 0;
	}

	void write_instruction(std::size_t pwm_instruction_us)
	{
		std::lock_guard<std::mutex> guard = std::lock_guard<std::mutex>(m_instruction_mutex);
		m_pwm_instruction_us = pwm_instruction_us;
		m_new_instruction = true;
	}

	void driver_handler()
	{
		while (m_should_close == false)
		{
			if (m_new_instruction == false)
			{
				continue;
			}

			if (m_bound_gpio_pin == 0)
			{
				continue;
			}

			std::lock_guard<std::mutex> guard = std::lock_guard<std::mutex>(m_instruction_mutex);

			gpioWrite(m_bound_gpio_pin, m_pwm_instruction_us);

			m_new_instruction = false;
		}
	}

private:
	int m_bound_gpio_pin = 0;
	bool m_should_close = false;
	std::mutex m_instruction_mutex;
	bool m_new_instruction = false;
	std::size_t m_pwm_instruction_us = 0;

	std::thread m_handler;

	double m_force_min_kgf;
	double m_force_max_kgf;

	std::shared_ptr<t200_data_t> m_data = nullptr;
	VOLTAGE_MODE m_voltage = VOLTAGE_MODE::V10;
	BESCR3_CONFIG m_mode = BESCR3_CONFIG::T200;
};

#endif // BESCR3_H
#ifndef I2C_DEVICE_H
#define I2C_DEVICE_H

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include <vector>
#include <string>

#include "fileconstants.h"
#include "cli/cli.h"

#define I2C_TARGET I2C_SLAVE
#define I2C_CONTROLLER I2C_MASTER

class i2c_device_t
{
public:
	explicit i2c_device_t() {}
	~i2c_device_t() {}

	void init(std::string i2c_bus_file, int device_address)
	{
		m_device_fd = open(i2c_bus_file.c_str(), O_RDWR);
		if (m_device_fd == INVALID)
		{
			utils::log("(i2c_device_t) Failure to open I2C bus file. (" + i2c_bus_file + ")", utils::MSG_TYPE::ERROR);
			return;
		}

		if (ioctl(m_device_fd, I2C_TARGET, device_address) == INVALID)
		{
			utils::log("(i2c_device_t) Failure to communicate with bus. (" + i2c_bus_file + ")", utils::MSG_TYPE::ERROR);
			return;
		}
	}

private:
	int m_device_fd = INVALID;
	std::vector<uint8_t> m_input_buf;
	std::vector<uint8_t> m_output_buf;
};

#endif // I2C_DEVICE_H
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

// Thanks https://emlogic.no/2025/06/accessing-i2c-devices-from-userspace-in-linux/

// Probably overkill
#define MAX_WRITE_SIZE 1024

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

	auto is_init() -> bool { return (m_device_fd != INVALID); }

	void reg(uint8_t r)
	{
		m_input_len = 1;
		m_input_buf[0] = r;
	}

	auto write_to_buf(uint8_t b) -> int
	{
		if (++m_input_len > MAX_WRITE_SIZE)
		{
			return -1;
		}

		m_input_buf[m_input_len] = b;
		return 0;
	}

	auto read_to_buf(std::size_t nb) -> int
	{
		if (is_init() == false)
		{
			return -1;
		}

		if (nb > MAX_WRITE_SIZE)
		{
			return -1;
		}

		if (nb <= 0)
		{
			return -1;
		}

		read(m_device_fd, m_output_buf, nb);

		return 0;
	}

	// Size is MAX_WRITE_SIZE
	auto get_output_buf() -> std::uint8_t * { return m_output_buf; }

	int flush_out()
	{
		if (is_init() == false)
		{
			return -1;
		}

		return write(m_device_fd, m_input_buf, m_input_len);
	}

	void kill()
	{
		if (is_init() == false)
		{
			return;
		}

		close(m_device_fd);
	}

private:
	// TODO: C++-ify some of this garbage

	std::uint8_t m_input_buf[MAX_WRITE_SIZE];
	std::size_t m_input_len = 0;
	std::uint8_t m_output_buf[MAX_WRITE_SIZE];
	int m_device_fd = INVALID;
};

#endif // I2C_DEVICE_H
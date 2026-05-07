#ifndef ISM330DHCX_MMC5983MA_H
#define ISM330DHCX_MMC5983MA_H

#include <bitset>
#include <memory>
#include <sstream>

#include "cache/cache_manager.h"

#include "robot/driver/abstract_driver.h"
#include "utils.h"

#ifdef linux
#include "robot/driver/linux/i2c_device.h"
#endif

#include <Eigen/Dense>

#define DEFAULT_BUS "1"
#define ADDR_DEFAULT_ISM330DHCX 0x6b
#define ADDR_DEFAULT_MMC5983MA 0x30

#define REG_FUNC_CFG_ACCESS 0x01
#define REG_PIN_CTRL 0x02
#define REG_FIFO_CTRL1 0x07
#define REG_FIFO_CTRL2 0x08
#define REG_FIFO_CTRL3 0x09
#define REG_FIFO_CTRL4 0x0a
#define REG_BDR_REG1 0x0b
#define REG_BDR_REG2 0x0c
#define REG_INT1_CTRL 0x0d
#define REG_INT2_CTRL 0x0e
#define REG_WHO_AM_I 0x0f
#define REG_CTRL1_XL 0x10
#define REG_CTRL2_G 0x11
#define REG_CTRL3_C 0x12
#define REG_CTRL4_C 0x13
#define REG_CTRL5_C 0x14
#define REG_CTRL6_C 0x15
#define REG_CTRL7_G 0x16
#define REG_CTRL8_XL 0x17
#define REG_CTRL9_XL 0x18
#define REG_CTRL10_C 0x19
#define REG_ALL_INT_SRC 0x1a
#define REG_WAKE_UP_SRC 0x1b
#define REG_TAP_SRC 0x1c
#define REG_D6D_SRC 0x1d

#define REG_STATUS_REG 0x1e
#define BIT_STATUS_REG_TDA 0x6
#define BIT_STATUS_REG_GDA 0x7
#define BIT_STATUS_REG_XLDA 0x8

// Output registers
#define REG_OUT_TEMP_L 0x20
#define REG_OUT_TEMP_H 0x21
#define REG_OUTX_L_G 0x22
#define REG_OUTX_H_G 0x23
#define REG_OUTY_L_G 0x24
#define REG_OUTY_H_G 0x25
#define REG_OUTZ_L_G 0x26
#define REG_OUTZ_H_G 0x27
#define REG_OUTX_L_A 0x28
#define REG_OUTX_H_A 0x29
#define REG_OUTY_L_A 0x2a
#define REG_OUTY_H_A 0x2b
#define REG_OUTZ_L_A 0x2c
#define REG_OUTZ_H_A 0x2d

#define REG_EMB_FUNC_STATUS_MAINPAGE 0x35
#define REG_FSM_STATUS_A_MAINPAGE 0x36
#define REG_FSM_STATUS_B_MAINPAGE 0x37
#define REG_MLC_STATUS_MAINPAGE 0x38
#define REG_STATUS_MASTER_MAINPAGE 0x39
#define REG_FIFO_STATUS1 0x3a
#define REG_FIFO_STATUS2 0x3b

#define REG_TIMESTAMP0 0x40
#define REG_TIMESTAMP1 0x41
#define REG_TIMESTAMP2 0x42
#define REG_TIMESTAMP3 0x43
#define REG_TAP_CFG0 0x56
#define REG_TAP_CFG1 0x57
#define REG_TAP_CFG2 0x58
#define REG_TAP_THS_6D 0x59
#define REG_INT_DUR2 0x5a
#define REG_WAKE_UP_THS 0x5b
#define REG_WAKE_UP_DUR 0x5c
#define REG_FREE_FALL 0x5d
#define REG_MD1_CFG 0x5e
#define REG_MD2_CFG 0x5f
#define REG_INTERNAL_FREQ_FINE 0x63
#define REG_INT_OIS 0x6f
#define REG_CTRL1_OIS 0x70
#define REG_CTRL2_OIS 0x71
#define REG_CTRL3_OIS 0x72

#define REG_X_OFS_USR 0x73
#define REG_Y_OFS_USR 0x74
#define REG_Z_OFS_USR 0x75

#define REG_FIFO_DATA_OUT_TAG 0x78
#define REG_FIFO_DATA_OUT_X_L 0x79
#define REG_FIFO_DATA_OUT_X_H 0x7a
#define REG_FIFO_DATA_OUT_Y_L 0x7b
#define REG_FIFO_DATA_OUT_Y_H 0x7c
#define REG_FIFO_DATA_OUT_Z_L 0x7d
#define REG_FIFO_DATA_OUT_Z_H 0x7e

class ISM330DHCX_MMC5983MA_t : abstract_driver_t
{
public:
	explicit ISM330DHCX_MMC5983MA_t() {}
	~ISM330DHCX_MMC5983MA_t() {}

	auto read() -> driver_packet_t override
	{
		if (m_ism330dhcx->is_init() == false)
		{
			return driver_packet_t();
		}

		m_ism330dhcx->reg(REG_STATUS_REG);
		if (m_ism330dhcx->read_to_buf(1) == INVALID)
		{
			return driver_packet_t();
		}

		std::bitset<8> status = std::bitset<8>(m_ism330dhcx->get_output_buf()[0]);
		if (status[BIT_STATUS_REG_TDA] == 0)
		{
			return driver_packet_t();
		}

		m_ism330dhcx->reg(REG_OUT_TEMP_L);
		if (m_ism330dhcx->read_to_buf(1) == INVALID)
		{
			return driver_packet_t();
		}

		std::uint8_t b1 = m_ism330dhcx->get_output_buf()[0];

		m_ism330dhcx->reg(REG_OUT_TEMP_H);
		if (m_ism330dhcx->read_to_buf(1) == INVALID)
		{
			return driver_packet_t();
		};

		std::uint8_t b2 = m_ism330dhcx->get_output_buf()[0];

		double temp = utils::tc_tw(b1, b2) / 256.0;

		driver_packet_t res = driver_packet_t();
		res[common_data_headers::LINEAR_ACCELERATION3_ms2] = Eigen::Vector3d(0.0, 0.0, 0.0);
		res[common_data_headers::ANGULAR_ACCELERATION3_rads2] = Eigen::Vector3d(0.0, 0.0, 0.0);
		res[common_data_headers::TEMPERATURE_C] = temp;
		res[common_data_headers::MAGNETOMETER_T] = 0.0;

		return res;
	}

	void write(driver_packet_t what) override { (void)what; }

	auto start(driver_packet_t args) -> DRIVER_CONNECTION_STATUS override
	{
		(void)args;

		cache_manager_t cache_manager = cache_manager_t("ism330dhcx");
		cache_manager.load_cache();
		std::string override_address = cache_manager.read_buf_or("override_address", std::to_string(ADDR_DEFAULT_ISM330DHCX));
		std::string bus = cache_manager.read_buf_or("bus", DEFAULT_BUS);
		cache_manager.rebuild_cache();
		std::stringstream s;
		std::int32_t res = 0;
		s << override_address;
		s >> res;

		m_ism330dhcx->init(bus, res);
		if (m_ism330dhcx->is_init() == false)
		{
			return DRIVER_CONNECTION_STATUS::UNKNOWN;
		}

		return DRIVER_CONNECTION_STATUS::CONNECTED;
	}

	void stop() override { m_ism330dhcx->kill(); }

private:
	std::shared_ptr<i2c_device_t> m_ism330dhcx = std::make_shared<i2c_device_t>();
	std::shared_ptr<i2c_device_t> m_mmc5983ma = std::make_shared<i2c_device_t>();

	DRIVER_TYPE m_type = DRIVER_TYPE::SENSOR;
	DRIVER_SIGNATURE m_signature = DRIVER_SIGNATURE::IMU;
	DRIVER_CONNECTION_STATUS m_connection_status = DRIVER_CONNECTION_STATUS::UNKNOWN;
};

#endif // ISM330DHCX_MMC5983MA_H
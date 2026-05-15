#ifndef BESCR3_T200_H
#define BESCR3_T200_H

#include <memory>
#include <vector>

#include "strutils.h"

// This is STUPIDELY inefficient. TODO: Fix whatever... this is. Make it not store values in a string_view?

#include "BESCR3_T200_10v.h"
#include "BESCR3_T200_12v.h"
#include "BESCR3_T200_14v.h"
#include "BESCR3_T200_16v.h"
#include "BESCR3_T200_18v.h"
#include "BESCR3_T200_20v.h"
#include "utils.h"

#define REC_DELIM "\n"
#define REC_COMMENT ";"

#define T200_DAT_PWM_us 0
#define T200_DAT_RPM 1
#define T200_DAT_CURRENT_A 2
#define T200_DAT_VOLTAGE_V 3
#define T200_DAT_POWER_W 4
#define T200_DAT_FORCE_kgf 5
#define T200_DAT_EFFICIENCY_gw 6

typedef std::vector<std::vector<double>> records_t;

enum VOLTAGE_MODE
{
	V20,
	V18,
	V16,
	V14,
	V12,
	V10
};

inline auto get_string_for(VOLTAGE_MODE which)
{
	switch (which)
	{
	case (VOLTAGE_MODE::V20):
		return T200_20V;

	case (VOLTAGE_MODE::V18):
		return T200_18V;

	case (VOLTAGE_MODE::V16):
		return T200_16V;

	case (VOLTAGE_MODE::V14):
		return T200_14V;

	case (VOLTAGE_MODE::V12):
		return T200_12V;

	case (VOLTAGE_MODE::V10):
		return T200_10V;
	}
}

inline auto get_t200_records(VOLTAGE_MODE mode) -> std::shared_ptr<records_t>
{
	std::shared_ptr<records_t> res = std::make_shared<records_t>();
	std::string_view data = get_string_for(mode);
	std::vector<std::string> lines = string_split(std::string(data), REC_DELIM);

	for (const auto &line : lines)
	{
		if (line.empty() == true)
		{
			continue;
		}

		std::vector<std::string> tokens = string_split_whitespace(line);

		std::vector<double> tokensd = {};

		for (const auto &token : tokens)
		{
			double val = string_safe_dcast(token);
			tokensd.push_back(val);
		}

		res->push_back(tokensd);
	}

	return res;
}

inline auto collect_all_of_t200_data(std::size_t index, std::shared_ptr<records_t> records) -> std::vector<double>
{
	std::vector<double> res = {};

	if (index < 0)
	{
		return {};
	}

	for (const auto &record : *records)
	{
		if (index > record.size())
		{
			return {};
		}

		res.push_back(record[index]);
	}

	return res;
}

class t200_data_t
{
public:
	explicit t200_data_t() {}
	~t200_data_t() {}

	void load(VOLTAGE_MODE mode) { m_records = get_t200_records(mode); }

	auto get_var_to_var(double x, std::size_t index_x, std::size_t index_y) -> double
	{
		std::vector<double> x_data = collect_all_of_t200_data(index_x, m_records);
		if (x_data.empty() == true)
		{
			return 0.0;
		}

		double closest_x = utils::closest_value_to(x, x_data);

		for (const auto &record : *m_records)
		{
			if (record[index_x] != closest_x)
			{
				continue;
			}

			if (index_y >= record.size())
			{
				return 0.0;
			}

			return record[index_y];
		}

		return 0.0;
	}

	auto get_current_A_from_pwm_us_approx(int pwm_signal_us) -> double { return get_var_to_var(pwm_signal_us, T200_DAT_PWM_us, T200_DAT_CURRENT_A); }
	auto get_force_kgf_from_pwm_us_approx(int pwm_signal_us) -> double { return get_var_to_var(pwm_signal_us, T200_DAT_PWM_us, T200_DAT_FORCE_kgf); }
	auto get_pwm_us_from_force_kgf_approx(double force_kgf) -> double { return get_var_to_var(force_kgf, T200_DAT_FORCE_kgf, T200_DAT_PWM_us); }

	auto get_force_max_kgf() -> double
	{
		std::vector<double> vec = collect_all_of_t200_data(T200_DAT_FORCE_kgf, m_records);
		utils::sort_greatest_to_least(vec);
		return vec.front();
	}

	auto get_force_min_kgf() -> double
	{
		std::vector<double> vec = collect_all_of_t200_data(T200_DAT_FORCE_kgf, m_records);
		utils::sort_least_to_greatest(vec);
		return vec.front();
	}

private:
	std::shared_ptr<records_t> m_records = nullptr;
};

#endif // BESCR3_T200
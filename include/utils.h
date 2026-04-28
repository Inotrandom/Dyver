#ifndef H_UTILS
#define H_UTILS

#include <cmath>

namespace utils
{
/**
 * @brief Convert metric "milliU" to U
 *
 */
constexpr double MILLIU_T_U = (1.0e-3);

/**
 * @brief Convert metric centiU to U
 *
 */
constexpr double CENTIU_T_U = (1.0e-2);

/**
 * @brief Convert metric nanoU to U
 *
 */
constexpr double NANOU_T_U = (1.0e-9);

/**
 * @brief Convert metric kiloU to U
 *
 */
constexpr double KILOU_T_U = (1.0e3);

/**
 * @brief Returns the square of n
 *
 * @param n The number to take the square of
 * @return double Representing the result of the square operation
 */
inline auto square(const double n) -> double { return n * n; }

struct linear_percentage_t
{
	double m_max;
	double m_min;

	linear_percentage_t(const double min, const double max)
	{
		m_max = max;
		m_min = min;
	}

	auto to_percentage(const double v) -> double
	{
		double diff = (m_max - m_min);
		double inbetween = (v - m_min);

		return (inbetween / diff);
	}

	auto sgn_to_percentage(const double v) -> double
	{
		double diff = (m_max - m_min);

		double mid = diff / 2.0;

		double inbetween = (v - m_min);

		double res;
		linear_percentage_t pc = linear_percentage_t(mid, diff);

		// Gross, but I'm lazy

		if (inbetween < mid)
		{
			pc.m_min = 0.0;
			pc.m_max = mid;
			res = pc.to_percentage(inbetween) * -1.0;

			return res;
		}

		res = pc.to_percentage(inbetween);

		return res;
	}

	auto to_value(const double p) -> double
	{
		double diff = (m_max - m_min);
		return m_min + (p * diff);
	}

	auto sgn_to_value(const double p) -> double
	{
		double diff = (m_max - m_min);
		double mid = diff / 2.0;

		linear_percentage_t pc = linear_percentage_t(mid, diff);

		if (p < 0.0)
		{
			pc.m_min = 0.0;
			pc.m_max = mid;

			return m_min + pc.to_value(std::abs(p));
		}

		return m_min + pc.to_value(p);
	}
};
} // namespace utils

#endif
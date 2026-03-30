#ifndef H_WIDGET
#define H_WIDGET

#include "SDL3/SDL_video.h"

#include <optional>

/**
 * @brief Two-dimensional spatial point
 *
 */
struct point_xy_t
{
	double x = 0.0;
	double y = 0.0;
};

/**
 * @brief Position relatively sized to a SDL window
 *
 */
struct ui_pos_t
{
	point_xy_t pos;

	ui_pos_t(double ax, double ay)
	{
		pos.x = ax;
		pos.y = ay;
	}

	/**
	 * @brief Produces a screen position in px using an SDL window
	 *
	 * @param window the window to use as the viewport
	 * @return std::optional<point_xy_t> screen position in px relative to `window`
	 */
	auto screen_pos(SDL_Window *window) -> std::optional<point_xy_t>
	{
		int w = 0;
		int h = 0;

		h = SDL_GetWindowSurface(window)->h;
		w = SDL_GetWindowSurface(window)->w;

		return point_xy_t{pos.x * w, pos.y * h};
	}
};

class widget_t
{
public:
	auto get_pos() -> ui_pos_t & { return m_pos; }

private:
	ui_pos_t m_pos = ui_pos_t(0.0, 0.0);
};

#endif

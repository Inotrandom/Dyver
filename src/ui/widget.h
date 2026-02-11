#ifndef H_WIDGET
#define H_WIDGET

#include "SDL3/SDL_video.h"

#include <optional>

struct point_xy_t
{
	double x = 0.0;
	double y = 0.0;
};

struct ui_pos_t
{
	point_xy_t pos;

	ui_pos_t(double ax, double ay)
	{
		pos.x = ax;
		pos.y = ay;
	}

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

#ifndef H_RECTANGLE
#define H_RECTANGLE

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_video.h"

#include <optional>

#include "widget.h"

class rectangle_t
{
public:
	explicit rectangle_t() {}
	~rectangle_t() {}

	rectangle_t(const ui_pos_t aa, const ui_pos_t bb, SDL_Color color = {255, 255, 255, 255})
	{
		m_aa = aa;
		m_bb = bb;
		m_color = color;
	}

	auto to_sdl_rect(SDL_Window *window) -> std::optional<SDL_FRect>
	{
		SDL_FRect res;

		std::optional<point_xy_t> posaa = m_aa.screen_pos(window);
		std::optional<point_xy_t> posbb = m_bb.screen_pos(window);

		if (posaa.has_value() == false || posbb.has_value() == false)
		{
			return std::nullopt;
		}

		res.x = posaa->x;
		res.y = posaa->y;

		res.h = (posbb->y - posaa->y);
		res.w = (posbb->x - posaa->x);

		return res;
	}

private:
	ui_pos_t m_aa = ui_pos_t(0.0, 0.0);
	ui_pos_t m_bb = ui_pos_t(0.0, 0.0);
	SDL_Color m_color = {255, 255, 255, 255};
};

#endif // H_RECTANGLE
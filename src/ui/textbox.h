#ifndef H_TEXTBOX
#define H_TEXTBOX

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "ui/proportions.h"

#include "ui/renderobj.h"
#include "ui/rectangle.h"
#include "ui/fontloader.h"

#include <algorithm>
#include <string>
#include <cstdlib>

class textbox_t : renderobj_t
{
public:
	explicit textbox_t() {}

	explicit textbox_t(
		const rectangle_t rect, const SDL_Color text_color = {0, 0, 0, SDL_ALPHA_OPAQUE}, const SDL_Color background_color = {255, 255, 255, SDL_ALPHA_OPAQUE})
	{
		fontloader_t fl;
		m_font = fl.load(FONT_HEVLETICA);

		m_text_color = text_color;
		m_background_color = background_color;

		m_rect = rect;
	}

	~textbox_t()
	{
		TTF_CloseFont(m_font);

		SDL_DestroySurface(m_surface);
		SDL_DestroyTexture(m_texture);
	}

	void rerender()
	{
		SDL_DestroySurface(m_surface); // Out with the old, in with the new
		m_surface = TTF_RenderText_Blended(m_font, m_text.c_str(), 0, m_text_color);

		m_queue_rerender_texture = true;
	}

	void render(SDL_Renderer *renderer, SDL_Window *window) override
	{
		if (m_queue_rerender_texture == true)
		{
			SDL_DestroyTexture(m_texture); // Clean slate
			m_texture = SDL_CreateTextureFromSurface(renderer, m_surface);
			m_queue_rerender_texture = false;
		}

		m_rect.render(renderer, window);

		SDL_SetRenderScale(renderer, DEFAULT_TEXT_RENDER_SCALE, DEFAULT_TEXT_RENDER_SCALE);

		SDL_FRect textbox_rect = m_rect.to_sdl_rect(window).value();
		SDL_FRect dst_rect = SDL_FRect();

		// Get the size of the text
		SDL_GetTextureSize(m_texture, &dst_rect.w, &dst_rect.h);

		// Calculate the text proportions relative to the textbox
		double proportions_h = textbox_rect.h / dst_rect.h;
		double proportions_w = textbox_rect.w / dst_rect.w;

		// Find the smallest dimension that the text has to fit into
		double scale = std::min(proportions_h, proportions_w);

		// Scale it down to fit
		dst_rect.h *= scale;
		dst_rect.w *= scale;

		dst_rect.x = textbox_rect.x;
		dst_rect.y = textbox_rect.y;

		// Center rectangle
		dst_rect.y += (textbox_rect.h / 2) - dst_rect.h / 2;

		SDL_RenderTexture(renderer, m_texture, nullptr, &dst_rect);
	}

	auto get_rect() -> rectangle_t & { return m_rect; }

	void set_text(std::string to)
	{
		m_text = to;
		rerender();
	}

private:
	rectangle_t m_rect = rectangle_t();
	TTF_Font *m_font = nullptr;
	std::string m_text = std::string();

	SDL_Surface *m_surface = nullptr;
	SDL_Texture *m_texture = nullptr;

	SDL_Color m_text_color;
	SDL_Color m_background_color;

	bool m_queue_rerender_texture = false;
};

#endif // H_TEXTBOX
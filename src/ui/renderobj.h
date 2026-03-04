#ifndef H_RENDEROBJ
#define H_RENDEROBJ

#include <SDL3/SDL_render.h>

class renderobj_t
{
public:
	explicit renderobj_t() {}
	~renderobj_t() {}

	/**
	 * @brief Render this renderable
	 *
	 * @param renderer SDL renderer used to render geometry
	 * @param window Window used to extrapolate the relative size of geometry
	 */
	virtual void render(SDL_Renderer *renderer, SDL_Window *window) = 0;

private:
};

#endif // H_RENDEROBJ
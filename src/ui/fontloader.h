#ifndef H_FONTLOADER
#define H_FONTLOADER

#include "SDL3_ttf/SDL_ttf.h"

#include <string>

const std::string FONT_HEVLETICA = "../assets/font/Helvetica Neue/Helvetica Neue Condensed Bold/Helvetica Neue Condensed Bold.ttf";

class fontloader_t
{
public:
	explicit fontloader_t() {}
	~fontloader_t() {}

	auto load(std::string which) -> TTF_Font *
	{
		TTF_Font *res = TTF_OpenFont(which.c_str(), 90.0);

		return res;
	}

private:
};

#endif // H_FONTLOADER
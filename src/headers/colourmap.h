#ifndef COLOURMAP_H
#define COLOURMAP_H

#include <map>

struct Colour {
	Colour(const int B, const int G, const int R) :
			B(B), G(G), R(R) {}
	const int B;
	const int G;
	const int R;
};

const std::map<std::string, Colour> colourMap = {
	{"Black", Colour(0,0,0)},
	{"White", Colour(255,255,255)},
	{"Red", Colour(0,0,255)},
	{"Orange", Colour(0,165,255)},
	{"Orange Red", Colour(0,69,255)},
	{"Lime", Colour(0,255,0)},
	{"Green Yellow", Colour(47,255,173)},
	{"Lawn Green", Colour(0,252,124)},
	{"Magenta", Colour(255,0,255)},
	{"Blue", Colour(255,0,0)},
	{"Khaki", Colour(140,230,240)},
	{"Yellow", Colour(0,255,255)},
	{"Aqua Marine", Colour(212,255,127)},
	{"Light Sky Blue", Colour(250,206,135)},
	{"Turquoise", Colour(208,224,64)},
	{"Cyan", Colour(255,255,0)},
	{"Silver", Colour(192,192,192)},
	{"Maroon", Colour(0,0,128)},
	{"Purple", Colour(128,0,128)},
	{"Blue Violet", Colour(226,43,138)},
	{"Navy", Colour(128,0,0)},
	{"Green", Colour(0,128,0)}
};

#endif

#include "Color.h"
#include <cstdlib>
Color Color::black(0, 0, 0);
Color Color::white(1, 1, 1);
Color Color::red(1, 0, 0);
Color Color::green(0, 1, 0);
Color Color::blue(0, 0, 1);

Color::Color() :Color(0, 0, 0)
{
}

Color::Color(float r, float g, float b) : r(r), g(g), b(b),a(1)
{
}

Color::Color(float r, float g, float b,float a) : r(r), g(g), b(b), a(a)
{
}


Color::Color(const Color& color) : Color(color.r, color.g, color.b, color.a)
{

}

Color Color::Random()
{
	float base = 1.0 / 255.f;
	auto r = (rand() % 255);
	auto g = (rand() % 255);
	auto b = (rand() % 255);
	return Color(r * base,g * base,b * base);
}

Color Color::add(const Color& color)const
{
	return Color(r + color.r, g + color.g, b + color.b);
}

Color Color::modulate(const Color& color)const
{
	return Color(r * color.r, g * color.g, b * color.b);
}

Color Color::multiply(float s)const
{
	return Color(r * s, g * s, b * s);
}



Color Color::Lerp(Color c1, Color c2, float t)
{
	Color res;

	res.r = c1.r + (c2.r - c1.r) * t;
	res.g = c1.g + (c2.g - c1.g) * t;
	res.b = c1.b + (c2.b - c1.b) * t;

	return res;
}
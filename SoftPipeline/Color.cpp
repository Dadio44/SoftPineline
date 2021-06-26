#include "Color.h"

Color Color::black(0, 0, 0);
Color Color::white(1, 1, 1);
Color Color::red(1, 0, 0);
Color Color::green(0, 1, 0);
Color Color::blue(0, 0, 1);

Color::Color() :Color(0, 0, 0)
{
}

Color::Color(float r, float g, float b) : r(r), g(g), b(b)
{
}


Color::Color(const Color& color) : Color(color.r, color.g, color.b)
{

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
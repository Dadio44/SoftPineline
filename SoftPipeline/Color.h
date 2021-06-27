#pragma once

struct Color
{
	float r;
	float g;
	float b;
	float a;

	Color();
	Color(float r, float g, float b);
	Color(float r, float g, float b, float a);
	Color(const Color& color);

	Color add(const Color& color)const;
	Color modulate(const Color& color)const;
	Color multiply(float s)const;
	static Color Random();

	static Color black;
	static Color white;
	static Color red;
	static Color green;
	static Color blue;
};

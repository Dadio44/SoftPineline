#pragma once
#include "glm/glm.hpp"
struct RasterPixel
{
	RasterPixel() :screenPos()
	{

	}
	RasterPixel(glm::ivec2 screenPos) :screenPos(screenPos)
	{

	}
	glm::ivec2 screenPos;
};


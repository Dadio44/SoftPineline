#pragma once
#include "Color.h"
#include "RasterOutput.h"
#include "glm/glm.hpp"

class IPixelShader
{
public:
	virtual Color PixelShader(
		const RasterOutput& pixelInput,
		const RasterOutput& dx, 
		const RasterOutput& dy)const = 0;
};


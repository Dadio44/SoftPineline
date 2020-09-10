#pragma once
#include "glm/glm.hpp"
struct RasterOutput
{
	glm::vec4 sv_position;
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 uv;
	glm::vec2 FscreenPos;
	glm::ivec2 screenPos;
};


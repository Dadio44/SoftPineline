#pragma once
#include "glm/glm.hpp"
struct RasterOutput
{
	glm::vec4 sv_position;
	glm::ivec2 screenPos;
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};


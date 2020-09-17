#pragma once
#include "glm/glm.hpp"
struct RasterOutput
{
	glm::vec4 sv_position;
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec3 uv;
	glm::ivec2 screenPos;
};


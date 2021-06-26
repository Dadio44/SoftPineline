#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

struct VertexOutPut
{
	glm::vec4 sv_position;
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 uv;

public:
	static VertexOutPut lerp(const VertexOutPut& a, const VertexOutPut& b,float weight)
	{
		VertexOutPut res;

		float weight2 = 1 - weight;

		res.sv_position = 
			a.sv_position * weight2 + (b.sv_position * weight);

		res.position =
			a.position * weight2 + (b.position * weight);

		res.normal =
			a.normal * weight2 + (b.normal * weight);

		res.uv =
			a.uv * weight2 + (b.uv * weight);

		return res;
	}
};


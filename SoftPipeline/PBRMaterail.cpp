#include "PBRMaterail.h"

void PBRMaterail::VertexShader(const std::vector<VertexInput>& vsInput, std::vector<VertexOutPut>& vsOutput, int verticesCount) const
{
	for (int i = 0; i < verticesCount; i++)
	{
		vsOutput[i].position = _model * vsInput[i].position;
		vsOutput[i].normal = _model * vsInput[i].normal;
		vsOutput[i].sv_position = _vp * vsOutput[i].position;
		vsOutput[i].uv = vsInput[i].uv;
	}
}

Color PBRMaterail::PixelShader(const RasterOutput& pixelInput, const RasterOutput& dx, const RasterOutput& dy) const
{
	glm::vec3 lightDir(-1, 1, 0);



    return Color::red;
}

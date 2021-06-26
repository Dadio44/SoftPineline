#include "UnlitMaterial.h"


void UnlitMaterial::VertexShader(
	const std::vector<VertexInput>& vsInput, 
	std::vector<VertexOutPut>& vsOutput,
	int verticesCount)const
{
	for (int i = 0; i < verticesCount; i++)
	{
		vsOutput[i].position = _model * vsInput[i].position;
		vsOutput[i].normal = _model * vsInput[i].normal;
		vsOutput[i].sv_position = _vp * vsOutput[i].position;
		vsOutput[i].uv = vsInput[i].uv;
	}
}

Color UnlitMaterial::PixelShader(const RasterOutput& pixelInput, const RasterOutput& rx, const RasterOutput& ry)const
{
	glm::vec2 ddx = rx.uv - pixelInput.uv;
	glm::vec2 ddy = ry.uv - pixelInput.uv;

	return _mainTex->Sampler(pixelInput.uv.x, pixelInput.uv.y, ddx, ddy);
}

#include "UnlitMaterial.h"


void UnlitMaterial::VertexShader(const std::vector<VertexInput>& vsInput, std::vector<VertexOutPut>& vsOutput)const
{
	glm::mat4x4 model = glm::mat4x4(1);
	model = glm::translate(model, _currentPos);
	//model = glm::rotate(model,glm::radians(30.0f), glm::vec3(1, 0, -1.0f));
	//model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0, 1.0, 0));

	glm::mat4x4 view = glm::lookAt(glm::vec3(0, 0, 2.5f), glm::vec3(0), glm::vec3(0, 1, 0));
	glm::mat4x4 projection = glm::perspective(glm::radians(60.0f), (float)_width / _height, 0.3f, 100.0f);

	glm::mat4x4 vp = projection * view;

	int cnt = vsInput.size();

	for (int i = 0; i < cnt; i++)
	{
		vsOutput[i].position = model * vsInput[i].position;
		vsOutput[i].normal = model * vsInput[i].normal;
		vsOutput[i].sv_position = vp * vsOutput[i].position;
		vsOutput[i].uv = vsInput[i].uv;
	}
}

BMP::Color UnlitMaterial::PixelShader(const RasterOutput& pixelInput, const RasterOutput& rx, const RasterOutput& ry)const
{
	glm::vec2 ddx = rx.uv - pixelInput.uv;
	glm::vec2 ddy = ry.uv - pixelInput.uv;


	ddx.x *= _mainTex->GetWidth();
	ddy.x *= _mainTex->GetWidth();
	ddx.y *= _mainTex->GetHeight();
	ddy.y *= _mainTex->GetHeight();


	float rho = glm::max(glm::dot(ddx, ddx), glm::dot(ddy, ddy));
	float lambda = 0.5 * log2(rho);

	float d = glm::max(lambda, 0.0f);


	return _mainTex->Sampler(pixelInput.uv.x, pixelInput.uv.y, 0);
}

#pragma once


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "BMP.h"
#include "RasterOutput.h"
#include "VertexInput.h"
#include "Mesh.h"
#include "VertexOutPut.h"

class Render
{
public:
	int width = 1920 * 2;
	int height = 1080 * 2;

	BMP::BMP rt;
	float* depthBuffer;
	const BMP::BMP* texture;
	RasterOutput* rasterOutBuffer;

	void GetVsInputs(const Mesh& mesh, std::vector<VertexInput>& vsInput);
	void VertexShader(const std::vector<VertexInput>& vsInput, std::vector<VertexOutPut>& vsOutput);
	void Rasterize(const std::vector<VertexOutPut>& vsOutput);
	void DrawTriangle(
		const RasterOutput& v1,
		const RasterOutput& v2,
		const RasterOutput& v3);

	void DrawPixel(const RasterOutput& v,float mipMap);

	RasterOutput GetInterpolationValue(
		const RasterOutput& v1,
		const RasterOutput& v2,
		const RasterOutput& v3,
		float u,
		float v,
		float w,
		int x,
		int y);

	RasterOutput GetRasterOutput(const VertexOutPut& vertex);
	BMP::Color PixelShader(const RasterOutput& pixelInput,const BMP::BMP& texture, float mipMap);
	void ClearColor(BMP::BMP& rt);
	void ClearDepth(float value, float* depthBuffer);

	BMP::Color Lerp(BMP::Color c1, BMP::Color c2, float t);

	BMP::Color Sampler(const BMP::BMP& texture, float u, float v, float mipmapLevel);

	void Draw(const Mesh& mesh,const BMP::BMP& tex, const glm::vec3& pos);

	glm::vec3 currentPos;

	void Init()
	{
		depthBuffer = static_cast<float*>(malloc(sizeof(float) * width * height));

		rasterOutBuffer = static_cast<RasterOutput*>(malloc(sizeof(RasterOutput) * width * height));

		rt.SetOutPut("renderTarget.bmp", width, height);
		ClearColor(rt);
		ClearDepth(1, depthBuffer);
	}

	void output()
	{
		rt.writeImage();
	}

	~Render()
	{
		free(depthBuffer);
		free(rasterOutBuffer);
	}

};
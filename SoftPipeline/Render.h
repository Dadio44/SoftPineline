#pragma once


#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "BMP.h"
#include "RasterOutput.h"
#include "VertexInput.h"
#include "Mesh.h"
#include "VertexOutPut.h"
#include "IVertexShader.h"
#include "IPixelShader.h"

enum CullingType
{
	Back,
	Front,
	None,
};


class Render
{
private:
	int _width;
	int _height;

	BMP::BMP rt;
	float* depthBuffer;
	RasterOutput* rasterOutBuffer;

	const IVertexShader* _vs;
	const IPixelShader* _ps;
	//减少频繁构造,只增不减
	std::vector<VertexInput> _vsInput;
	std::vector<VertexOutPut> _vsout;
	std::vector<VertexOutPut> _cullResBufs[6 + 1];
	int _curOutputBufIndex;

	bool Inside(const glm::vec4& line, const glm::vec4& p);
	VertexOutPut Intersect(const VertexOutPut& v1, const VertexOutPut& v2, const glm::vec4& line);
	bool AllVertexsInside(const std::vector<VertexOutPut> input);
	int SutherlandHodgeman(const VertexOutPut& v1, const VertexOutPut& v2, const VertexOutPut& v3);

	static std::vector<glm::vec4> _ViewLines;

	CullingType _cullingtype;
public:
	
	void GetVsInputs(const Mesh& mesh, std::vector<VertexInput>& vsInput);
	void Rasterize(const std::vector<VertexOutPut>& vsOutput,int verticesCount);
	void DrawTriangle(
		const RasterOutput& v1,
		const RasterOutput& v2,
		const RasterOutput& v3);

	void DrawPixel(const RasterOutput& v, const RasterOutput& dx, const RasterOutput& dy);

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
	void ClearColor(BMP::BMP& rt);
	void ClearDepth(float value, float* depthBuffer);

	void SetShader(const IVertexShader* vs, const IPixelShader* ps);
	void SetCullingOption(CullingType cullingtype)
	{
		_cullingtype = cullingtype;
	}


	void Draw(const Mesh& mesh);

	void Init(int width,int height)
	{
		_width = width;
		_height = height;


		depthBuffer = static_cast<float*>(malloc(sizeof(float) * _width * _height));

		rasterOutBuffer = static_cast<RasterOutput*>(malloc(sizeof(RasterOutput) * _width * _height));

		rt.SetOutPut("renderTarget.bmp", _width, _height);
		ClearColor(rt);
		ClearDepth(1, depthBuffer);
	}

	void output()
	{
		rt.writeImage();
	}

	Render()
	{
		for(int i = 0;i < 7;i++)
			_cullResBufs[i].resize(32);
	}

	~Render()
	{
		free(depthBuffer);
		free(rasterOutBuffer);
	}

};
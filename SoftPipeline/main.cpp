#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <BMP.h>
#include <Mesh.h>
#include <test.h>
#include <VertexInput.h>
#include <VertexOutPut.h>
#include <RasterOutput.h>
#include <RasterPixel.h>

int width = 1920;
int height = 1080;


void GetVsInputs(const Mesh& mesh, std::vector<VertexInput>& vsInput);
void VertexShader(const std::vector<VertexInput>& vsInput,std::vector<VertexOutPut>& vsOutput);
void Rasterize(const std::vector<VertexOutPut>& vsOutput,std::vector<RasterOutput>& rasterOutput);
void DrawTriangle(std::vector<RasterOutput>& rasterOutput, const RasterOutput& v1, const RasterOutput& v2, const RasterOutput& v3);
void DrawLine(std::vector<RasterPixel>& rasterOutput, const RasterPixel & rL, const RasterPixel & rR);
RasterPixel GetRasterPixel(const RasterOutput & rasterOutput);
void FillTriangleByDrawLine(std::vector<RasterPixel>& rasterOutput, const RasterPixel & v1, const RasterPixel & v2, const RasterPixel & v3);
void FillButtomTriangle(std::vector<RasterPixel>& rasterOutput, const RasterPixel & p1, const RasterPixel & p2, const RasterPixel & p3);
void FillTopTriangle(std::vector<RasterPixel>& rasterOutput, const RasterPixel & v1, const RasterPixel & v2, const RasterPixel & v3);
void Interpolation(
	std::vector<RasterOutput>& rasterOutput, 
	const RasterOutput& v1,
	const RasterOutput& v2,
	const RasterOutput& v3,
	const std::vector<RasterPixel>& rasterPixels);

RasterOutput GetInterpolationValue(
	const RasterOutput& v1,
	const RasterOutput& v2,
	const RasterOutput& v3,
	const RasterPixel& pixel);

void FillTriangle(std::vector<RasterOutput>& rasterOutput, const RasterOutput& v1, const RasterOutput& v2, const RasterOutput& v3);
RasterOutput GetRasterOutput(const VertexOutPut& vertex);
void PixelShader(const std::vector<RasterOutput>& rasterOutput,BMP::BMP& rt);
void ClearColor(BMP::BMP& rt);

int main()
{

	Mesh mesh;

	mesh.LoadFromFile("Cube.obj");

	//PrintMesh(mesh);

	std::vector<VertexInput> vsInput(mesh.GetIndicesCount());

	GetVsInputs(mesh, vsInput);

	std::vector<VertexOutPut> vsOutput(vsInput.size());
	VertexShader(vsInput,vsOutput);

	// TODO :clip

	std::vector<RasterOutput> rasterOut;
	Rasterize(vsOutput, rasterOut);


	BMP::BMP rt;

	rt.SetOutPut("renderTarget.bmp", width, height);
	ClearColor(rt);

	PixelShader(rasterOut,rt);

	rt.writeImage();
	
	system("pause");

	return 0;
}

void GetVsInputs(const Mesh & mesh, std::vector<VertexInput>& vsInput)
{
	auto indices = mesh.GetIndices();
	auto indicesCount = mesh.GetIndicesCount();

	auto vertices = mesh.GetVerices();
	auto normal = mesh.GetNormals();
	auto uv = mesh.GetUVs();

	int index;

	for (int i = 0; i < indicesCount; i++)
	{
		index = indices[i] * 3;

		vsInput[i].position = glm::vec4(vertices[index], vertices[index + 1], vertices[index + 2],1);
		vsInput[i].normal = glm::vec4(normal[index], normal[index + 1], normal[index + 2],0);

		index = indices[i] * 2;
		vsInput[i].uv = glm::vec2(uv[index], uv[index + 1]);
	}
}

void VertexShader(const std::vector<VertexInput>& vsInput, std::vector<VertexOutPut>& vsOutput)
{
	glm::mat4x4 model = glm::mat4x4(1);
	glm::translate(model, glm::vec3(0));

	glm::mat4x4 view = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0), glm::vec3(0, 1, 0));
	glm::mat4x4 projection = glm::perspective(60.0f,(float)width / height,0.3f,100.0f);

	glm::mat4x4 vp = projection * view;

	int cnt = vsInput.size();

	for (int i = 0;i < cnt;i++)
	{
		vsOutput[i].position = model * vsInput[i].position;
		vsOutput[i].normal = model * vsInput[i].normal;
		vsOutput[i].sv_position = vp * vsOutput[i].position;
		vsOutput[i].uv = vsOutput[i].uv;
	}
}

void Rasterize(const std::vector<VertexOutPut>& vsOutput, std::vector<RasterOutput>& rasterOutput)
{
	int size = vsOutput.size();

	for (int i = 0; i < size; i += 3)
	{
		DrawTriangle(rasterOutput, 
			GetRasterOutput(vsOutput[i]),
			GetRasterOutput(vsOutput[i + 1]),
			GetRasterOutput(vsOutput[i + 2]));
	}

}

void DrawTriangle(std::vector<RasterOutput>& rasterOutput, const RasterOutput & v1, const RasterOutput & v2, const RasterOutput & v3)
{
	if (v1.screenPos.y > v2.screenPos.y && v1.screenPos.y > v3.screenPos.y)
	{
		if (v2.screenPos.y > v3.screenPos.y)
		{
			FillTriangle(rasterOutput, v1, v2, v3);
		}
		else
		{
			FillTriangle(rasterOutput, v1, v3, v2);
		}
	}

	if (v2.screenPos.y > v1.screenPos.y && v2.screenPos.y > v3.screenPos.y)
	{
		if (v1.screenPos.y > v3.screenPos.y)
		{
			FillTriangle(rasterOutput, v2, v1, v3);
		}
		else
		{
			FillTriangle(rasterOutput, v2, v3, v1);
		}
	}

	if (v3.screenPos.y > v1.screenPos.y && v3.screenPos.y > v2.screenPos.y)
	{
		if (v1.screenPos.y > v2.screenPos.y)
		{
			FillTriangle(rasterOutput, v3, v1, v2);
		}
		else
		{
			FillTriangle(rasterOutput, v3, v2, v1);
		}
	}
	

}

void DrawLine(std::vector<RasterPixel>& rasterOutput, const RasterPixel & rL, const RasterPixel & rR)
{
	for (int i = rL.screenPos.x; i < rR.screenPos.x; i++)
	{
		rasterOutput.push_back(RasterPixel(glm::ivec2(i,rL.screenPos.y)));
	}
}

RasterPixel GetRasterPixel(const RasterOutput& rasterOutput)
{
	RasterPixel res; 
	res.screenPos = rasterOutput.screenPos;
	return res;
}

void FillTriangleByDrawLine(std::vector<RasterPixel>& rasterOutput, const RasterPixel & v1, const RasterPixel & v2, const RasterPixel & v3)
{

}

void FillButtomTriangle(std::vector<RasterPixel>& rasterOutput, const RasterPixel & p1, const RasterPixel & p2, const RasterPixel & p3)
{
	int dy = p1.screenPos.y - p2.screenPos.y;

	int dx = p2.screenPos.x - p3.screenPos.x;

	const RasterPixel* pL;
	const RasterPixel* pR;

	if (dx < 0)
	{
		pL = &p2;
		pR = &p3;
	}
	else
	{
		pL = &p3;
		pR = &p2;
	}



	float kL = (p1.screenPos.x - pL->screenPos.x) / glm::max(1,p1.screenPos.y - pL->screenPos.y);
	float kR = (p1.screenPos.x - pR->screenPos.x) / glm::max(1,p1.screenPos.y - pR->screenPos.y);

	RasterPixel tL;
	RasterPixel tR;


	for (int i = 0; i < dy; i++)
	{
		tL.screenPos.y = tR.screenPos.y = p1.screenPos.y - i;
		tL.screenPos.x = p1.screenPos.x + kL * -i;
		tR.screenPos.x = p1.screenPos.x + kR * -i;

		DrawLine(rasterOutput, tL, tR);
	}

}

void FillTopTriangle(std::vector<RasterPixel>& rasterOutput, const RasterPixel & v1, const RasterPixel & v2, const RasterPixel & v3)
{

}

void Interpolation(
	std::vector<RasterOutput>& rasterOutput,
	const RasterOutput& v1,
	const RasterOutput& v2,
	const RasterOutput& v3,
	const std::vector<RasterPixel>& rasterPixels)
{
	for (auto pixel : rasterPixels)
	{
		rasterOutput.push_back(GetInterpolationValue(v1, v2, v3, pixel));
	}
}

RasterOutput GetInterpolationValue(const RasterOutput & v1, const RasterOutput & v2, const RasterOutput & v3, const RasterPixel & pixel)
{
	RasterOutput res;

	res.screenPos = pixel.screenPos;

	return res;
}

void FillTriangle(std::vector<RasterOutput>& rasterOutput, const RasterOutput & v1, const RasterOutput & v2, const RasterOutput & v3)
{
	assert(v1.screenPos.y >= v2.screenPos.y &&
		v2.screenPos.y >= v3.screenPos.y);

	std::vector<RasterPixel> rasterPixels;

	RasterPixel p1 = GetRasterPixel(v1);
	RasterPixel p2 = GetRasterPixel(v2);
	RasterPixel p3 = GetRasterPixel(v3);

	if (v1.screenPos.y == v2.screenPos.y)
	{
		FillTopTriangle(rasterPixels, p3, p1, p2);		
		Interpolation(rasterOutput, v1, v2, v3,rasterPixels);
		return;
	}

	if (v2.screenPos.y == v3.screenPos.y)
	{
		FillButtomTriangle(rasterPixels, p1, p2, p3);
		Interpolation(rasterOutput, v1, v2, v3, rasterPixels);
		return;
	}

	RasterPixel p4;

	float k = (v1.screenPos.x - v3.screenPos.x) / (v1.screenPos.y - v3.screenPos.y);

	p4.screenPos.x = v1.screenPos.x + (v2.screenPos.y - v1.screenPos.y) * k;
	p4.screenPos.y = v1.screenPos.y;

	FillButtomTriangle(rasterPixels, p1, p2, p4);
	FillTopTriangle(rasterPixels, p2, p4, p3);

	Interpolation(rasterOutput, v1, v2, v3, rasterPixels);
}

RasterOutput GetRasterOutput(const VertexOutPut & vertex)
{
	RasterOutput rasterOutput;

	rasterOutput.sv_position = vertex.sv_position / -vertex.sv_position.w;
	rasterOutput.sv_position.w = 1;
	rasterOutput.normal = vertex.normal;
	rasterOutput.position = vertex.position;
	rasterOutput.uv = vertex.uv;
	rasterOutput.screenPos.x = (vertex.sv_position.x * 0.5f + 0.5f) * width;
	rasterOutput.screenPos.y = (vertex.sv_position.y * 0.5f + 0.5f) * height;

	return rasterOutput;
}

void PixelShader(const std::vector<RasterOutput>& rasterOutput, BMP::BMP& rt)
{
	for (auto pixelInput : rasterOutput)
	{
		rt.drawPixelAt(255, 0, 0, pixelInput.screenPos.x, pixelInput.screenPos.y);
	}
}

void ClearColor(BMP::BMP& rt)
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			rt.drawPixelAt(255, 255, 255, i, j);
		}
	}

}

#include <vector>
#include <iostream>
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
void DrawTriangle(
	std::vector<RasterOutput>& rasterOutput, 
	const RasterOutput& v1, 
	const RasterOutput& v2, 
	const RasterOutput& v3);

bool IsBack(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
bool Inside(const glm::vec3& p, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);

void Interpolation(
	std::vector<RasterOutput>& rasterOutput, 
	const RasterOutput& v1,
	const RasterOutput& v2,
	const RasterOutput& v3,
	int x,
	int y);

RasterOutput GetInterpolationValue(
	const RasterOutput& v1,
	const RasterOutput& v2,
	const RasterOutput& v3,
	int x,
	int y);

RasterOutput GetRasterOutput(const VertexOutPut& vertex);
void PixelShader(const std::vector<RasterOutput>& rasterOutput,BMP::BMP& rt);
void ClearColor(BMP::BMP& rt);

int main()
{

	Mesh mesh;

	mesh.LoadFromFile("default_MeshPart0.obj");

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
	
	//system("pause");

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
	model = glm::translate(model, glm::vec3(0));
	model = glm::rotate(model,glm::radians(30.0f), glm::vec3(1, 0, -1.0f));
	model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0, 1.0, 0));

	glm::mat4x4 view = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0), glm::vec3(0, 1, 0));
	glm::mat4x4 projection = glm::perspective(glm::radians(60.0f),(float)width / height,0.3f,10.0f);

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

bool IsBack(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
	return glm::cross(v1, v2).z < 0;
}

bool Inside(const glm::vec3& p, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
	glm::vec3 v12 = v2 - v1;
	glm::vec3 v23 = v3 - v2;
	glm::vec3 v31 = v1 - v3;

	glm::vec3 v1p = p - v1;
	glm::vec3 v2p = p - v2;
	glm::vec3 v3p = p - v3;

	float c1 = glm::cross(v12, v1p).z;
	float c2 = glm::cross(v23, v2p).z;
	float c3 = glm::cross(v31, v3p).z;

	if (IsBack(v12, v23, v31))
		return (c1 < 0 && c2 < 0 && c3 < 0);
	else
		return (c1 >= 0 && c2 >= 0 && c3 >= 0);
}

void DrawTriangle(std::vector<RasterOutput>& rasterOutput, const RasterOutput & v1, const RasterOutput & v2, const RasterOutput & v3)
{
	int minX = glm::min(v1.screenPos.x, v2.screenPos.x);
	minX = glm::min(v3.screenPos.x, minX);
	minX = glm::max(0, minX);
	minX = glm::min(width, minX);
	
	int maxX = glm::max(v1.screenPos.x, v2.screenPos.x);
	maxX = glm::max(v3.screenPos.x, maxX);
	maxX = glm::max(0, maxX);
	maxX = glm::min(width, maxX);

	int minY = glm::min(v1.screenPos.y, v2.screenPos.y);
	minY = glm::min(v3.screenPos.y, minY);
	minY = glm::max(0, minY);
	minY = glm::min(height,minY);

	int maxY = glm::max(v1.screenPos.y, v2.screenPos.y);
	maxY = glm::max(v3.screenPos.y, maxY);
	maxY = glm::max(0, maxY);
	maxY = glm::min(height, maxY);

	assert(0 <= minX && minX <= maxX && maxX <= width);
	assert(0 <= minY && minY <= maxY && maxY <= height);

	for (int x = minX; x <= maxX; x++)
	{
		for (int y = minY; y <= maxY; y++)
		{
			if (Inside(glm::vec3(x,y,0), 
				glm::vec3(v1.FscreenPos,0),
				glm::vec3(v2.FscreenPos,0), 
				glm::vec3(v3.FscreenPos,0)))
			{
				Interpolation(rasterOutput, v1, v2, v3,x,y);
			}
		}
	}

}

void Interpolation(
	std::vector<RasterOutput>& rasterOutput,
	const RasterOutput& v1,
	const RasterOutput& v2,
	const RasterOutput& v3,
	int x,
	int y)
{
	rasterOutput.push_back(GetInterpolationValue(v1, v2, v3, x,y));
}

RasterOutput GetInterpolationValue(
	const RasterOutput & v1,
	const RasterOutput & v2, 
	const RasterOutput & v3, 
	int x,
	int y)
{
	RasterOutput res;

	res.screenPos.x = x;
	res.screenPos.y = y;

	return res;
}

RasterOutput GetRasterOutput(const VertexOutPut & vertex)
{
	RasterOutput rasterOutput;

	rasterOutput.sv_position = vertex.sv_position / vertex.sv_position.w;
	rasterOutput.normal = vertex.normal;
	rasterOutput.position = vertex.position;
	rasterOutput.uv = vertex.uv;
	rasterOutput.FscreenPos.x = (rasterOutput.sv_position.x * 0.5 + 0.5) * width;
	rasterOutput.FscreenPos.y = (rasterOutput.sv_position.y * 0.5 + 0.5) * height;
	rasterOutput.screenPos.x = rasterOutput.FscreenPos.x;
	rasterOutput.screenPos.y = rasterOutput.FscreenPos.y;

 	return rasterOutput;
}

void PixelShader(const std::vector<RasterOutput>& rasterOutput, BMP::BMP& rt)
{
	for (auto pixelInput : rasterOutput)
	{
		if (
			pixelInput.screenPos.x < width && 
			pixelInput.screenPos.y < height &&
			pixelInput.screenPos.x >= 0 &&
			pixelInput.screenPos.y >= 0
			)
		{
			rt.drawPixelAt(255, 0, 0, pixelInput.screenPos.x, pixelInput.screenPos.y);
		}
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

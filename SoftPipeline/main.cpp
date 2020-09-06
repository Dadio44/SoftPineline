#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "BMP.h"
#include "Mesh.h"
#include "test.h"
#include "VertexInput.h"
#include "VertexOutPut.h"

int width = 1920;
int height = 1080;



void GetVsInputs(const Mesh& mesh, std::vector<VertexInput>& vsInput);
void VertexShader(const std::vector<VertexInput>& vsInput,std::vector<VertexOutPut>& vsOutput);

int main()
{

	Mesh mesh;

	mesh.LoadFromFile("Cube.obj");

	//PrintMesh(mesh);

	std::vector<VertexInput> vsInput(mesh.GetIndicesCount());

	GetVsInputs(mesh, vsInput);

	std::vector<VertexOutPut> vsOutput(vsInput.size());
	VertexShader(vsInput,vsOutput);
	
	BMP::BMP rt;
	rt.SetOutPut("renderTarget.bmp", width, height);

	BMP::ColorPass r = 0;
	BMP::ColorPass b = 0;
	BMP::ColorPass g = 0;	


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
		vsOutput[i].normal = model * vsInput[i].position;
		vsOutput[i].sv_position = vp * vsOutput[i].position;
		vsOutput[i].uv = vsOutput[i].uv;
	}
}

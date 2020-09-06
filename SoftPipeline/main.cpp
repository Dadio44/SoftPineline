#include <iostream>
#include "glm/glm.hpp"
#include "BMP.h"
#include "Mesh.h"
#include "test.h"
#include "VertexInput.h"
#include <vector>

int width = 1920;
int height = 1080;

void GetVsInputs(const Mesh& mesh, std::vector<VertexInput>& vsInput);

int main()
{

	Mesh mesh;

	mesh.LoadFromFile("Cube.obj");

	//PrintMesh(mesh);

	std::vector<VertexInput> vsInput;

	GetVsInputs(mesh, vsInput);
	
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
	vsInput.resize(mesh.GetIndicesCount());

	auto indices = mesh.GetIndices();
	auto indicesCount = mesh.GetIndicesCount();

	auto vertices = mesh.GetVerices();
	auto normals = mesh.GetNormals();
	auto uvs = mesh.GetUVs();

	int index;

	for (int i = 0; i < indicesCount; i++)
	{
		index = indices[i] * 3;

		vsInput[i].position = glm::vec4(vertices[index], vertices[index + 1], vertices[index + 2],1);
		vsInput[i].normals = glm::vec4(normals[index], normals[index + 1], normals[index + 2],0);

		index = indices[i] * 2;
		vsInput[i].uvs = glm::vec2(uvs[index], uvs[index + 1]);
	}
}

#include <iostream>
#include "Mesh.h"

void PrintMesh(const Mesh& mesh)
{
	float* verticesOffset = mesh.GetVerices();

	int cnt = mesh.GetVericesCount() * 3;

	for (int i = 0; i < cnt; i++)
	{
		if (i % 3 == 0)
			std::cout << std::endl;
		std::cout << *(verticesOffset) << ' ';

		verticesOffset++;
	}

	float* normalsOffset = mesh.GetNormals();

	cnt = mesh.GetNormalsCount() * 3;

	for (int i = 0; i < cnt; i++)
	{
		if (i % 3 == 0)
			std::cout << std::endl;
		std::cout << *(normalsOffset) << ' ';

		normalsOffset++;
	}

	float* uvsOffset = mesh.GetUVs();

	cnt = mesh.GetUVsCount() * 2;

	for (int i = 0; i < cnt; i++)
	{
		if (i % 2 == 0)
			std::cout << std::endl;
		std::cout << *(uvsOffset) << ' ';

		uvsOffset++;
	}


	int* indicesOffset = mesh.GetIndices();

	cnt = mesh.GetIndicesCount();

	for (int i = 0; i < cnt; i++)
	{
		if (i % 3 == 0)
			std::cout << std::endl;
		std::cout << *(indicesOffset) << ' ';

		indicesOffset++;
	}
}
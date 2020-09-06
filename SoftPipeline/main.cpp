#include <iostream>
#include "glm/glm.hpp"
#include "BMP.h"
#include "Mesh.h"
#include "test.h"

int width = 1920;
int height = 1080;

int main()
{

	Mesh mesh;

	mesh.LoadFromFile("Cube.obj");

	//PrintMesh(mesh);
	
	BMP::BMP rt;
	rt.SetOutPut("renderTarget.bmp", width, height);

	BMP::ColorPass r = 0;
	BMP::ColorPass b = 0;
	BMP::ColorPass g = 0;	


	rt.writeImage();
	
	system("pause");

	return 0;
}
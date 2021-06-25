#include <vector>
#include <iostream>
#include <memory>



//#include "test.h"
#include "Render.h"
#include "MeshManager.h"
#include "BMPManager.h"
#include "Model.h"

using namespace ResourceManager;


int main()
{
	std::shared_ptr<MeshManager> meshManager = std::make_shared<MeshManager>();
	std::shared_ptr<BMPManager> bmpManager = std::make_shared<BMPManager>();

	Render render;
	render.Init();

	Model hero;

	std::vector<std::string> meshs;
	meshs.push_back("Hair009.obj");
	meshs.push_back("Body010.obj");
	meshs.push_back("Face009.obj");

	hero.LoadMeshes(meshManager, meshs);

	std::vector<std::string> textures;
	textures.push_back("hair.bmp");
	textures.push_back("Body.bmp");
	textures.push_back("Face.bmp");

	hero.LoadTextures(bmpManager, textures);

	glm::vec3 offset(3.679, 2.5, -1.1434);

	hero.SetPos(glm::vec3(0) - offset);
	hero.Render(render);

	render.output();

	//RenderHero(glm::vec3(0) - offset);

	//system("pause");

	return 0;
}
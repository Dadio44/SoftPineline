#include <vector>
#include <iostream>
#include <memory>



//#include "test.h"
#include "Render.h"
#include "MeshManager.h"
#include "BMPManager.h"
#include "Model.h"
#include "UnlitMaterial.h"

const int SRC_WIDTH = 1920;
const int SRC_HEIGHT = 1080;

using namespace ResourceManager;

void RenderHero(
	Render& render, 
	std::shared_ptr<MeshManager>& meshManager,
	std::shared_ptr<BMPManager>& bmpManager)
{
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

	glm::vec3 offset(3.679, 2.5, -1.1434);

	glm::vec3 pos = glm::vec3(0) - offset;

	std::vector<IMaterial*> materials;

	for (auto path : textures)
	{
		auto id = bmpManager->Load(path);
		materials.push_back(new UnlitMaterial(
			bmpManager->Get(id),
			pos,
			SRC_WIDTH,
			SRC_HEIGHT));
	}

	hero.SetMaterials(materials);
	hero.Render(render);
}

int main()
{
	std::shared_ptr<MeshManager> meshManager = std::make_shared<MeshManager>();
	std::shared_ptr<BMPManager> bmpManager = std::make_shared<BMPManager>();

	Render render;
	render.Init(SRC_WIDTH, SRC_HEIGHT);

	RenderHero(render, meshManager, bmpManager);

	render.output();

	//system("pause");

	return 0;
}
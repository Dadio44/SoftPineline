#include <vector>
#include <iostream>
#include <memory>



//#include "test.h"
#include "Render.h"
#include "MeshManager.h"
#include "BMPManager.h"
#include "Model.h"
#include "Camera.h"

#include "UnlitMaterial.h"
#include "SimpleLitMaterail.h"

const int SRC_WIDTH = 1920;
const int SRC_HEIGHT = 1080;

using namespace ResourceManager;

void RenderBox(
	Render& render,
	const Camera& cam,
	std::shared_ptr<MeshManager>& meshManager)
{
	glm::mat4x4 model = glm::mat4x4(1);
	model = glm::translate(model, glm::vec3(0));

	model = glm::rotate(model,glm::radians(45.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1, 0, 0));

	Model box(model);

	std::vector<std::string> meshs;
	meshs.push_back("Cube.obj");
	
	box.LoadMeshes(meshManager, meshs);

	std::vector<IMaterial*> materials;

	auto mat = new SimpleLitMaterail(Color::red);
	mat->SetViewProjection(cam.GetView(), cam.GetProjection());
	materials.push_back(mat);

	box.SetMaterials(materials);
	box.Render(render);	
}

void RenderHero(
	Render& render, 
	const Camera& cam,
	std::shared_ptr<MeshManager>& meshManager,
	std::shared_ptr<BMPManager>& bmpManager)
{
	glm::vec3 offset(3.679, 2.5, -1.1434);

	glm::vec3 pos = glm::vec3(0) - offset;

	glm::mat4x4 model = glm::mat4x4(1);
	model = glm::translate(model, pos);

	//model = glm::rotate(model,glm::radians(30.0f), glm::vec3(1, 0, -1.0f));
	//model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0, 1.0, 0));

	Model hero(model);

	std::vector<std::string> meshs;
	meshs.push_back("Hair009.obj");
	meshs.push_back("Body010.obj");
	meshs.push_back("Face009.obj");

	hero.LoadMeshes(meshManager, meshs);

	std::vector<std::string> textures;
	textures.push_back("hair.bmp");
	textures.push_back("Body.bmp");
	textures.push_back("Face.bmp");

	std::vector<IMaterial*> materials;

	for (auto path : textures)
	{
		auto id = bmpManager->Load(path);
		auto mat = new UnlitMaterial(
			bmpManager->Get(id));
		mat->SetViewProjection(cam.GetView(), cam.GetProjection());
		materials.push_back(mat);
	}

	hero.SetMaterials(materials);
	hero.Render(render);
}

int main()
{
	std::shared_ptr<MeshManager> meshManager = std::make_shared<MeshManager>();
	std::shared_ptr<BMPManager> bmpManager = std::make_shared<BMPManager>();

	Camera camera;

	camera.SetPos(glm::vec3(0, 0, 2.5f));
	camera.SetTarget(glm::vec3(0));
	camera.SetPerspective(glm::radians(60.0f), (float)SRC_WIDTH / SRC_HEIGHT, 0.3f, 100.0f);

	Render render;
	render.Init(SRC_WIDTH, SRC_HEIGHT);

	//RenderHero(render, camera, meshManager, bmpManager);
	
	RenderBox(render, camera, meshManager);
	
	
	render.output();

	//system("pause");

	return 0;
}
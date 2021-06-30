#include <vector>
#include <iostream>
#include <memory>



//#include "test.h"
#include "Render.h"
#include "MeshManager.h"
#include "BMPManager.h"
#include "Model.h"
#include "Camera.h"
#include "CubeMap.h"

#include "UnlitMaterial.h"
#include "SimpleLitMaterail.h"
#include "SkyBoxMaterial.h"

#include "platform.h"

const int SRC_WIDTH = 1920 / 2;
const int SRC_HEIGHT = 1080 / 2;

using namespace ResourceManager;

float lastX = 0, lastY = 0;
float yaw = 90, pitch = 0;
bool firstMouse = true;

void RenderBox(
	Render& render,
	const Camera& cam,
	std::shared_ptr<MeshManager>& meshManager,
	std::shared_ptr<BMPManager>& bmpManager)
{
	glm::mat4x4 model = glm::mat4x4(1);
	model = glm::translate(model, glm::vec3(0));

	model = glm::rotate(model,glm::radians(45.0f), glm::vec3(0, 1, 0));
	//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1, 0, 0));

	Model box(model);

	std::vector<std::string> meshs;
	meshs.push_back("Cube.obj");
	
	box.LoadMeshes(meshManager, meshs);

	std::vector<IMaterial*> materials;

	/*auto id = bmpManager->Load("512_UVChecker.bmp");
	auto mat = new UnlitMaterial(
		bmpManager->Get(id));*/

	auto mat = new SimpleLitMaterail(Color(1.0f,0,0,0.3f));

	mat->SetViewProjection(cam.GetView(), cam.GetProjection());

	/*auto mat = new SimpleLitMaterail(Color::red);
	mat->SetViewProjection(cam.GetView(), cam.GetProjection());
	*/
	materials.push_back(mat);

	box.SetMaterials(materials);
	box.Render(render);	
}

void RenderShaderBall(
	Render& render,
	const Camera& cam,
	std::shared_ptr<MeshManager>& meshManager,
	std::shared_ptr<BMPManager>& bmpManager)
{
	glm::mat4x4 model = glm::mat4x4(1);
	model = glm::translate(model, glm::vec3(0));

	//model = glm::rotate(model,glm::radians(45.0f), glm::vec3(0, 1, 0));
	//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1, 0, 0));

	Model box(model);

	std::vector<std::string> meshs;
	meshs.push_back("Cushion.obj");
	meshs.push_back("GrayBackground.obj");
	meshs.push_back("GrayInlay.obj");
	meshs.push_back("MaterialBase.obj");
	meshs.push_back("MaterialBaseInside.obj");
	meshs.push_back("MaterialSphere.obj");
	

	box.LoadMeshes(meshManager, meshs);

	std::vector<IMaterial*> materials;

	//auto id = bmpManager->Load("512_UVChecker.bmp");
	//auto mat = new UnlitMaterial(
	//	bmpManager->Get(id));
	//mat->SetViewProjection(cam.GetView(), cam.GetProjection());

	srand(0);

	for (int i = 0; i < 6; i++)
	{
		auto col = Color::Random();

		if (i == 5)
		{
			col.a = 0.5f;
		}

		auto mat = new SimpleLitMaterail(col);
		mat->SetViewProjection(cam.GetView(), cam.GetProjection());
		materials.push_back(mat);
	}


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
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, pos);

	
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

void RenderSkyBox(Render& render,
	const Camera& cam,
	std::shared_ptr<MeshManager>& meshManager)
{
	glm::mat4x4 model = glm::mat4x4(1);
	model = glm::translate(model, glm::vec3(0));

	Model box(model);

	std::vector<std::string> meshs;
	meshs.push_back("Cube.obj");

	box.LoadMeshes(meshManager, meshs);

	std::vector<IMaterial*> materials;

	std::vector<std::string> faces
	{
		"right.bmp",
		"left.bmp",
		"top.bmp",
		"bottom.bmp",
		"front.bmp",
		"back.bmp"
	};

	CubeMap* cubeMap = new CubeMap();
	cubeMap->Load(faces);

	auto mat = new SkyBoxMaterial(cubeMap);
	mat->SetViewProjection(cam.GetView(), cam.GetProjection());
	materials.push_back(mat);

	box.SetMaterials(materials);
	box.Render(render);
}

void UpdateCamera(window* window, Camera& camera)
{
	float speed = 0.1f;
	if (input_key_pressed(window, KEY_A)) {
		auto right = camera.GetRight();

		camera.Trasnlate(- speed * right);
	}
	if (input_key_pressed(window, KEY_D)) {
		
		auto right = camera.GetRight();

		camera.Trasnlate(speed * right);
	}
	if (input_key_pressed(window, KEY_S)) {
		auto forward = camera.GetForward();

		camera.Trasnlate(-speed * forward);
	}
	if (input_key_pressed(window, KEY_W)) {
		auto forward = camera.GetForward();

		camera.Trasnlate(speed * forward);
	}

	float xpos, ypos;

	if (input_button_pressed(window, BUTTON_R))
	{

		input_query_cursor(window, &xpos, &ypos);


		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.05;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		auto cameraFront = glm::normalize(front);

		camera.SetTarget(camera.GetPos() + cameraFront);
	}
	else
	{
		firstMouse = true;
	}
}

void Loop()
{
	std::shared_ptr<MeshManager> meshManager = std::make_shared<MeshManager>();
	std::shared_ptr<BMPManager> bmpManager = std::make_shared<BMPManager>();

	Camera camera;

	//camera.SetPos(glm::vec3(0.6f, 0.6f, 0.6f));
	//camera.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));

	camera.SetPos(glm::vec3(0.0f, 0.0f, -3.0f));

	//camera.SetTarget(glm::vec3(0, 0, 0));
	camera.SetPerspective(glm::radians(60.0f), (float)SRC_WIDTH / SRC_HEIGHT, 0.3f, 100.0f);

	auto window = window_create("Ass", SRC_WIDTH, SRC_HEIGHT);

	Render render;
	render.Init(window->surface);


	float prev_time;
	float print_time;
	int num_frames;

	num_frames = 0;
	prev_time = platform_get_time();
	print_time = prev_time;

	char title[128];
	while (!window_should_close(window)) 
	{
		float curr_time = platform_get_time();
		float delta_time = curr_time - prev_time;

		num_frames += 1;
		if (curr_time - print_time >= 1) {
			int sum_millis = (int)((curr_time - print_time) * 1000);
			int avg_millis = sum_millis / num_frames;
			sprintf(title, "fps: %3d, avg: %3d  %f %f ms\n", num_frames, avg_millis,yaw,pitch);
			SetWindowTextA(window->handle, title);
			//printf("fps: %3d, avg: %3d ms\n", num_frames, avg_millis);
			num_frames = 0;
			print_time = curr_time;
		}
		prev_time = curr_time;

		render.ClearColor(Color::black);
		render.ClearDepth(2);

		//RenderHero(render, camera, meshManager, bmpManager);


		RenderBox(render, camera, meshManager, bmpManager);

		//RenderShaderBall(render, camera, meshManager, bmpManager);

		//RenderSkyBox(render, camera, meshManager);
		window_draw_buffer(window);


		input_poll_events();
		UpdateCamera(window, camera);
	}

	

	window_destroy(window);
	render.output();
}

int main()
{
	platform_initialize();
	Loop();
	//system("pause");
	platform_terminate();
	return 0;
}
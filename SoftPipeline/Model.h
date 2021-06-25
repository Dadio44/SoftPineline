#pragma once
#include <vector>
#include "MeshManager.h"
#include "BMPManager.h"
#include "Render.h"
#include <memory>

class Model
{
public:

	void LoadMeshes(std::shared_ptr<ResourceManager::MeshManager> meshManager,const std::vector<std::string> meshesPath)
	{
		_meshManager = meshManager;
		for (auto path : meshesPath)
		{
			_meshes.push_back(meshManager->Load(path));
		}	
	}

	void LoadTextures(std::shared_ptr<ResourceManager::BMPManager> texManager, const std::vector<std::string> texturesPath)
	{
		_texManager = texManager;
		for (auto path : texturesPath)
		{
			_textures.push_back(texManager->Load(path));
		}
	}



	void SetPos(const glm::vec3 pos)
	{
		_pos = pos;
	}

	void Render(Render& render)
	{
		auto cnt = _meshes.size();
		for(int i = 0;i < cnt;i++)
		{
			render.Draw(*_meshManager->Get(_meshes[i]),*_texManager->Get(_textures[i]), _pos);
		}
	}

private:
	std::vector<ResourceManager::ResHandler> _meshes;
	std::vector<ResourceManager::ResHandler> _textures;
	glm::vec3 _pos;
	std::shared_ptr<ResourceManager::MeshManager> _meshManager;
	std::shared_ptr<ResourceManager::BMPManager> _texManager;
};


#pragma once
#include <vector>
#include "MeshManager.h"
#include "BMPManager.h"
#include "Render.h"
#include <memory>
#include "IMaterial.h"

class Model
{
public:

	void LoadMeshes(
		std::shared_ptr<ResourceManager::MeshManager> meshManager,
		const std::vector<std::string> meshesPath)
	{
		_meshManager = meshManager;
		for (auto path : meshesPath)
		{
			_meshes.push_back(meshManager->Load(path));
		}	
	}

	void SetMaterials(const std::vector<IMaterial*> mats)
	{
		_materials = mats;
	}


	void Render(Render& render)
	{
		auto cnt = _meshes.size();

		if (cnt > 0)
			assert(cnt == _materials.size());

		for(int i = 0;i < cnt;i++)
		{
			IMaterial* mat = _materials[i];
			render.SetShader(mat, mat);
			render.Draw(*_meshManager->Get(_meshes[i]));
		}
	}


	~Model()
	{
		for (auto mat : _materials)
		{
			delete mat;
		}
	}

private:
	std::vector<IMaterial*> _materials;
	std::vector<ResourceManager::ResHandler> _meshes;
	std::shared_ptr<ResourceManager::MeshManager> _meshManager;
};


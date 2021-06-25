#include "MeshManager.h"

namespace ResourceManager
{
	Mesh* MeshManager::Get(ResHandler handle)
	{
		auto kv = _resources.find(handle);

		if (kv != _resources.end())
		{
			return kv->second;
		}

		return nullptr;
	}
	ResHandler MeshManager::Load(const std::string& filePath)
	{
		ResHandler res = INVALID_RES;

		auto kv = _str2Id.find(filePath);

		if (kv == _str2Id.end())
		{
			res = _GetNextId();
			_str2Id.insert({ filePath,res });
			_id2Str.insert({res,filePath});

			Mesh* mesh = new Mesh();

			mesh->LoadFromFile(filePath.c_str());

			_resources.insert({res,mesh});
		}
		else
		{
			res = kv->second;
		}

		return res;
	}
	void MeshManager::Unload(ResHandler handler)
	{
		auto kv = _resources.find(handler);

		if (kv != _resources.end())
		{
			auto id = kv->first;
			auto mesh = kv->second;
			
			_str2Id.erase(_id2Str[id]);
			_id2Str.erase(id);
			_resources.erase(id);

			delete mesh;
		}
	}
	void MeshManager::Clear()
	{
		for (auto& kv : _resources)
		{
			delete kv.second;
		}

		_str2Id.clear();
		_id2Str.clear();
		_resources.clear();
	}
}

#include "BMPManager.h"

namespace ResourceManager
{
	BMP::BMP* BMPManager::Get(ResHandler handle)
	{
		auto kv = _resources.find(handle);

		if (kv != _resources.end())
		{
			return kv->second;
		}

		return nullptr;
	}
	ResHandler BMPManager::Load(const std::string& filePath)
	{
		ResHandler res = INVALID_RES;

		auto kv = _str2Id.find(filePath);

		if (kv == _str2Id.end())
		{
			res = _GetNextId();
			_str2Id.insert({ filePath,res });
			_id2Str.insert({ res,filePath });

			BMP::BMP* tex = new BMP::BMP();

			tex->ReadFrom(filePath.c_str());
			tex->GenerateMipMap();

			_resources.insert({ res,tex });
		}
		else
		{
			res = kv->second;
		}

		return res;
	}
	void BMPManager::Unload(ResHandler handler)
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
	void BMPManager::Clear()
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

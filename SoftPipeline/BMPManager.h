#include "BMP.h"
#include "IResouceManager.h"
#include <unordered_map>
#pragma once

namespace ResourceManager
{
	class BMPManager : IResouceManager<BMP::BMP>
	{
	public:
		// 通过 IResouceManager 继承
		virtual BMP::BMP* Get(ResHandler) override;

		// 通过 IResouceManager 继承
		virtual ResHandler Load(const std::string& filePath) override;
		virtual void Unload(ResHandler handler) override;
		// 通过 IResouceManager 继承
		virtual void Clear() override;

		~BMPManager()
		{
			this->Clear();
		}

	private:
		std::unordered_map<ResHandler, BMP::BMP*> _resources;
		std::unordered_map<std::string, ResHandler> _str2Id;
		std::unordered_map<ResHandler, std::string> _id2Str;
		ResHandler _currentId = 0;
		ResHandler _GetNextId() {
			_currentId++;
			return _currentId;
		}
	};
}


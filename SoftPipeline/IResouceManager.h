#pragma once
#include <string>

namespace ResourceManager
{

    typedef unsigned int ResHandler;

    const ResHandler INVALID_RES = 0;

    template<typename T>
    class IResouceManager {
    public:
        virtual ResHandler Load(const std::string& filePath) = 0;
        virtual T* Get(ResHandler handler) = 0;
        virtual void Unload(ResHandler handler) = 0;
        virtual void Clear() = 0;
        typedef T ValueType;
    };
}
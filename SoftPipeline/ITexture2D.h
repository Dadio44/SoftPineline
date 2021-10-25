#pragma once
#include "Color.h"
#include "glm/glm.hpp"

typedef unsigned short UINT16;
typedef unsigned UINT32;
typedef unsigned char UINT8;
typedef UINT8 ColorPass;

class ITexture2D
{
public:
	virtual UINT32 GetWidth()const = 0;
	virtual UINT32 GetHeight()const = 0;

	virtual void ReadFrom(const char* fileName) = 0;
	
	virtual void SetSize(unsigned width, unsigned height) = 0;


	virtual void GetColorAt(unsigned x, unsigned y, Color* color) const = 0;

	virtual void GenerateMipMap() = 0;

	virtual void writeImage(const char* name = nullptr) = 0;

	virtual Color GetColorAt(unsigned x, unsigned y, int mipmapLevel)const = 0;

	virtual Color Sampler(float u, float v, float mipmapLevel)const = 0;

	virtual Color Sampler(float u, float v, glm::vec2& ddx, glm::vec2& ddy)const = 0;
};


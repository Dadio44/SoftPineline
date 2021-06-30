#pragma once
#include "ITexture2D.h"

class IRenderTarget
{
public:
	virtual UINT32 GetWidth()const = 0;
	virtual UINT32 GetHeight()const = 0;

	virtual void ReadFrom(const char* fileName) = 0;

	virtual void SetSize(unsigned width, unsigned height) = 0;

	virtual unsigned char** GetBitmapBufferAddress() = 0;

	virtual void SetBitmapBuffer(unsigned char* buffer) = 0;

	virtual void drawPixelAt(ColorPass r, ColorPass g, ColorPass b, unsigned x, unsigned y) = 0;

	virtual void drawPixelAt(float r, float g, float b, unsigned x, unsigned y) = 0;

	virtual void drawPixelAt(const Color& c, unsigned x, unsigned y) = 0;

	virtual void GetColorAt(unsigned x, unsigned y, Color* color) const = 0;

	virtual void ClearColor(const Color& c) = 0;

	virtual ~IRenderTarget() {}
};


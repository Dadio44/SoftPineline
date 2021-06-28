#ifndef _RENDERTARGET_H_
#define _RENDERTARGET_H_

#include <vector>
#include <string>

#include "glm/glm.hpp"
#include "Color.h"
#include "ITexture2D.h"
#include "IRenderTarget.h"

namespace BMP {

#pragma pack(push,2)
	struct BITMAPFILEHEADER
	{
		UINT16 signature;
		UINT32 fileSize;
		UINT32 reserved;
		UINT32 fileOffsetToPixelArray;
	};

	struct DIBHEADER
	{
		UINT32 dibHeaderSize;
		UINT32 imageWidth;
		UINT32 imageHeight;
		UINT16 planes;
		UINT16 bitsPerPixel;
		UINT32 compression;
		UINT32 imageSize;
		UINT32 xPiexlPerMeter;
		UINT32 yPiexlPerMeter;
		UINT32 colorsInColorTable;
		UINT32 importantColorCount;
	};

#pragma pack(pop)

	class BMP : 
		public ITexture2D,
		public IRenderTarget
	{
	private:

		std::string _fileName;
		UINT32 _width;
		UINT32 _height;
		UINT8* _buffer;
		UINT32 _rowSize;
		BITMAPFILEHEADER _bitmapFileHeader;
		DIBHEADER _dibHeader;
		UINT8* _mipMapBuffer;
		UINT32 _maxMipmapLevel;

		void WriteMipMap(int offset,int sourceOffset, int srcWidth, int resolutionX, int resolutionY);
		void GetMipmapData(int mipmapLevel,int& offset,int& rowSize)const;
	public:

		virtual UINT32 GetWidth()const override;
		UINT32 GetMaxMipMapLevel()const
		{
			return _maxMipmapLevel;
		}
		virtual UINT32 GetHeight()const override;
		void GetResolution(int mipMapLevel,int& width,int& height)const;

		BMP();

		void SetOutPut(const char * fileName, unsigned width, unsigned height);

		virtual void SetSize(unsigned width, unsigned height) override;

		virtual void ReadFrom(const char * fileName)override;

		~BMP()override;

		virtual void drawPixelAt(ColorPass r, ColorPass g, ColorPass b, unsigned x, unsigned y)override;

		virtual void drawPixelAt(float r, float g, float b, unsigned x, unsigned y)override;

		virtual void drawPixelAt(const Color& c, unsigned x, unsigned y)override;

		virtual void GetColorAt(unsigned x, unsigned y, Color* color) const override;

		virtual void writeImage(const char* name = NULL)override;

		virtual void GenerateMipMap()override;

		void writeMipMapImage(const char* name = NULL);

		virtual Color GetColorAt(unsigned x, unsigned y, int mipmapLevel)const override;

		virtual Color Sampler(float u, float v, float mipmapLevel)const override;

		virtual Color Sampler(float u, float v, glm::vec2 ddx, glm::vec2 ddy)const override;

		virtual unsigned char** GetBitmapBufferAddress() override
		{
			return &_buffer;
		}

		virtual void SetBitmapBuffer(unsigned char* buffer) override
		{
			_buffer = buffer;
		}
	};

}
#endif // !_RENDERTARGET_H_

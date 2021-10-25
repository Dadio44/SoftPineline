#include "BMP.h"
#include <cstdlib>
#include <cassert>
#include <cstdio>

#include "glm/glm.hpp"


#define _SIGNATURE 0x4d42
#define _BITS_OF_PIXEL 24
#define _COMPRESSION 0
#define _X_PIXEL_PER_METER 0
#define _Y_PIXEL_PER_METER 0
#define _COLORS_IN_COLOR_TABLE 0
#define _IMPORTANT_COLOR_COUNT 0
#define BYTES_OF_PIXEL 3
#define ALIGN(x,a)    (((x)+(a)-1)&~(a-1))    

namespace BMP {


	UINT32 BMP::GetWidth()const
	{
		return _width;
	}

	void BMP::GetResolution(int mipmapLevel, int& width, int& height)const
	{
		width = _width;
		height = _height;

		int curLevel = 0;

		while (width > 1 && height > 1)
		{
			if (mipmapLevel == curLevel)
			{
				break;
			}

			width = width / 2;
			height = height / 2;

			curLevel += 1;
		}
	}

	UINT32 BMP::GetHeight()const
	{
		return _height;
	}

	BMP::BMP()
		:_fileName(), _width(0), _height(0), _buffer(NULL),_rowSize(0)
	{
		assert(sizeof(UINT8) == 1);
		assert(sizeof(UINT16) == 2);
		assert(sizeof(UINT32) == 4);

		assert(sizeof(BITMAPFILEHEADER) == 14);
		assert(sizeof(DIBHEADER) == 40);
	}

	void BMP::SetSize(unsigned width, unsigned height)
	{
		this->_width = width;
		this->_height = height;
		this->_buffer = NULL;
		this->_rowSize = 0;

		assert(_width);
		assert(_height);

		_rowSize = ALIGN(_width, 4);
		UINT32 imageSize = _height * _rowSize * BYTES_OF_PIXEL;// assert the bits of pixel is 24 = 3 * 8

		// initialize the DIB header
		_bitmapFileHeader.signature = _SIGNATURE;
		_bitmapFileHeader.fileSize = sizeof(BITMAPFILEHEADER) + sizeof(DIBHEADER) + imageSize;
		_bitmapFileHeader.reserved = 0;
		_bitmapFileHeader.fileOffsetToPixelArray = sizeof(BITMAPFILEHEADER) + sizeof(DIBHEADER);


		// initialize the DIB header
		_dibHeader.dibHeaderSize = sizeof(DIBHEADER);
		_dibHeader.imageWidth = _rowSize;
		_dibHeader.imageHeight = _height;
		_dibHeader.planes = 1;
		_dibHeader.bitsPerPixel = _BITS_OF_PIXEL;
		_dibHeader.compression = _COMPRESSION;
		_dibHeader.imageSize = sizeof(UINT8) * imageSize;
		_dibHeader.xPiexlPerMeter = _X_PIXEL_PER_METER;
		_dibHeader.yPiexlPerMeter = _Y_PIXEL_PER_METER;
		_dibHeader.colorsInColorTable = _COLORS_IN_COLOR_TABLE;
		_dibHeader.importantColorCount = _IMPORTANT_COLOR_COUNT;
	}


	void BMP::SetOutPut(const char * fileName, unsigned width, unsigned height)
	{
		
		this->_fileName = fileName;
		SetSize(width, height);

		// malloc the memories of buffer
		_buffer = static_cast<UINT8*>(malloc(_dibHeader.imageSize));
	}

	void BMP::ReadFrom(const char * fileName)
	{
		this->_fileName = fileName;

		FILE* f = fopen(fileName, "rb");
	
		fread(&(this->_bitmapFileHeader), sizeof(BITMAPFILEHEADER), 1, f);
		fread(&(this->_dibHeader), sizeof(DIBHEADER), 1, f);

		if (this->_buffer)
			free(this->_buffer);
		this->_buffer = static_cast<UINT8*>(malloc(_dibHeader.imageSize));

		fread(this->_buffer, sizeof(UINT8), _dibHeader.imageSize,f);

		this->_width = _dibHeader.imageWidth;
		this->_height = _dibHeader.imageHeight;
		this->_rowSize = ALIGN(_width, 4);

		assert(_width);
		assert(_height);


		fclose(f);
	}

	BMP::~BMP()
	{
		free(_buffer);
		free(_mipMapBuffer);
	}

	void BMP::drawPixelAt(ColorPass r, ColorPass g, ColorPass b, unsigned x, unsigned y)
	{

		assert(x < _width);
		assert(y < _height);

		_buffer[(y * _rowSize + x) * BYTES_OF_PIXEL] = b;
		_buffer[(y * _rowSize + x) * BYTES_OF_PIXEL + 1] = g;
		_buffer[(y * _rowSize + x) * BYTES_OF_PIXEL + 2] = r;

	}

	void BMP::drawPixelAt(float r, float g, float b, unsigned x, unsigned y)
	{
		r = fmax(0.0, r);
		r = fmin(1.0, r);
		b = fmax(0.0, b);
		b = fmin(1.0, b);
		g = fmax(0.0, g);
		g = fmin(1.0, g);

		drawPixelAt(
			(ColorPass)(r * 255),
			(ColorPass)(g * 255), 
			(ColorPass)(b * 255),
			x, y);
	}

	void BMP::drawPixelAt(const Color & c, unsigned x, unsigned y)
	{
		drawPixelAt(c.r, c.g, c.b, x, y);
	}

	void BMP::GetColorAt(unsigned x, unsigned y, Color* color) const
	{
		assert(x < _width);
		assert(y < _height);


		ColorPass b = _buffer[(y * _rowSize + x) * BYTES_OF_PIXEL];
		ColorPass g = _buffer[(y * _rowSize + x) * BYTES_OF_PIXEL + 1];
		ColorPass r = _buffer[(y * _rowSize + x) * BYTES_OF_PIXEL + 2];
		
		float inv = 1.0f / 255;

		color->r = r * inv;
		color->g = g * inv;
		color->b = b * inv;
	}
	
	void BMP::writeImage(const char* name)
	{
		if (name == NULL)
			name = _fileName.c_str();
		FILE* f = fopen(name, "wb");
		assert(f);

		fwrite(&_bitmapFileHeader, sizeof(_bitmapFileHeader),1,f);
		fwrite(&_dibHeader, sizeof(_dibHeader), 1, f);
		fwrite(_buffer, _dibHeader.imageSize, 1, f);

		fclose(f);
	}

	void BMP::WriteMipMap(int offset, int sourceOffset, int srcWidth, int resolutionX, int resolutionY)
	{
		int last = offset + resolutionX * resolutionY;

		int r, g, b;
		
		int tsOffset;
		int csRowOffsetStart = sourceOffset;
		int csOffset = sourceOffset;

		for (;offset < last; offset++)
		{
			r = g = b = 0;

			tsOffset = csOffset;
			b += _mipMapBuffer[(tsOffset) * BYTES_OF_PIXEL];
			g += _mipMapBuffer[(tsOffset) * BYTES_OF_PIXEL + 1];
			r += _mipMapBuffer[(tsOffset) * BYTES_OF_PIXEL + 2];

			tsOffset = csOffset + 1;
			b += _mipMapBuffer[(tsOffset) * BYTES_OF_PIXEL];
			g += _mipMapBuffer[(tsOffset) * BYTES_OF_PIXEL + 1];
			r += _mipMapBuffer[(tsOffset) * BYTES_OF_PIXEL + 2];

			tsOffset = csOffset + srcWidth;
			b += _mipMapBuffer[(tsOffset) * BYTES_OF_PIXEL];
			g += _mipMapBuffer[(tsOffset) * BYTES_OF_PIXEL + 1];
			r += _mipMapBuffer[(tsOffset) * BYTES_OF_PIXEL + 2];

			tsOffset = tsOffset + 1;
			b += _mipMapBuffer[(tsOffset) * BYTES_OF_PIXEL];
			g += _mipMapBuffer[(tsOffset) * BYTES_OF_PIXEL + 1];
			r += _mipMapBuffer[(tsOffset) * BYTES_OF_PIXEL + 2];

			_mipMapBuffer[(offset) * BYTES_OF_PIXEL] = b * 0.25f;
			_mipMapBuffer[(offset) * BYTES_OF_PIXEL + 1] = g * 0.25f;
			_mipMapBuffer[(offset) * BYTES_OF_PIXEL + 2] = r * 0.25f;

			csOffset += 2;
			if (csOffset - csRowOffsetStart >= (srcWidth & 0xfffffffe))
			{
				csRowOffsetStart += 2 * srcWidth;
				csOffset = csRowOffsetStart;
			}
		}
	}

	void BMP::GetMipmapData(int mipmapLevel, int& offset, int& rowSize) const
	{
		int curResolutionX = _width;
		int curResolutionY = _height;

		offset = 0;

		int curLevel = 0;

		while (curResolutionX > 1 && curResolutionY > 1)
		{
			if (mipmapLevel == curLevel)
			{
				break;
			}

			offset += curResolutionX * curResolutionY;

			curResolutionX = curResolutionX / 2;
			curResolutionY = curResolutionY / 2;

			curLevel += 1;
		}

		rowSize = curResolutionX;
	}

	void BMP::GenerateMipMap()
	{
		if (_mipMapBuffer != nullptr)
		{
			free(_mipMapBuffer);
		}

		int total = (_width * _height);

		_mipMapBuffer = static_cast<UINT8*>(malloc(BYTES_OF_PIXEL * (total + total / 2)));

		memcpy(_mipMapBuffer, _buffer, total * BYTES_OF_PIXEL);


		int curResolutionX = _width / 2;
		int curResolutionY = _height / 2;

		int srcOffset = 0;
		int offset = total;

		int oldWidth = _width;

		_maxMipmapLevel = 0;

		while (curResolutionX > 0 && curResolutionY > 0)
		{
			WriteMipMap(offset, srcOffset, oldWidth, curResolutionX, curResolutionY);

			srcOffset = offset;
			offset += curResolutionX * curResolutionY;

			curResolutionX = (curResolutionX / 2);
			curResolutionY = (curResolutionY / 2);
			
			oldWidth = (oldWidth / 2);
			_maxMipmapLevel += 1;
		}

	}

	void BMP::writeMipMapImage(const char * name)
	{
		BMP mipMap;
		
		UINT32 mipmapWidth = _width + _width / 2;

		mipMap.SetOutPut(name, mipmapWidth, _height);


		for (int i = 0; i < _width; i++)
		{
			for (int j = 0; j < _height; j++)
			{
				mipMap.drawPixelAt(GetColorAt(i,j,0), i, j);
			}
		}

		UINT32 cWidth = _width / 2;
		UINT32 sy = 0;
		UINT32 cHeight = _height / 2;
		UINT32 mipMapLevel = 1;

		while (cWidth > 0)
		{
			for (int i = 0; i < cWidth; i++)
			{
				for (int j = 0; j < cHeight; j++)
				{
					mipMap.drawPixelAt(GetColorAt(i, j, mipMapLevel), i + _width, j + sy);
				}
			}

			cWidth /= 2;
			sy += cHeight;
			cHeight /= 2;
			mipMapLevel += 1;
		}

		mipMap.writeImage();
	}

	Color BMP::GetColorAt(unsigned x, unsigned y, int mipmapLevel)const
	{
		Color res;

		int offset, rowSize;
		GetMipmapData(mipmapLevel,offset, rowSize);

		ColorPass b = _mipMapBuffer[(offset + (y * rowSize + x)) * BYTES_OF_PIXEL];
		ColorPass g = _mipMapBuffer[(offset + (y * rowSize + x)) * BYTES_OF_PIXEL + 1];
		ColorPass r = _mipMapBuffer[(offset + (y * rowSize + x)) * BYTES_OF_PIXEL + 2];

		float inv = 1.0f / 255;

		res.r = r * inv;
		res.g = g * inv;
		res.b = b * inv;

		return res;
	}


	Color BMP::Sampler(float u, float v, float mipmapLevel)const
	{
		int lv = (int)mipmapLevel;

		int width;
		int height;

		this->GetResolution(lv, width, height);

		float ux = (u * width - 0.5f);
		float uy = (v * height - 0.5f);

		int x = (int)ux % width;
		int y = (int)uy % height;
		x = x < 0 ? width + x : x;
		y = y < 0 ? height + y : y;

		int x1 = glm::min(x + 1, width - 1);
		int x2 = x;
		int x3 = glm::min(x + 1, width - 1);

		int y1 = y;
		int y2 = glm::min(y + 1, height - 1);
		int y3 = glm::min(y + 1, height - 1);


		Color c0 = GetColorAt(x, y, lv);
		Color c1 = GetColorAt(x1, y1, lv);
		Color c2 = GetColorAt(x2, y2, lv);
		Color c3 = GetColorAt(x3, y3, lv);


		float ht = glm::fract(ux);
		float vt = glm::fract(uy);

		auto ch1 = Color::Lerp(c0, c1, ht);
		auto ch2 = Color::Lerp(c2, c3, ht);

		return Color::Lerp(ch1, ch2, vt);
	}

	void BMP::ClearColor(const Color& c)
	{
		BMPColor bmpc;
		bmpc.r = c.b;
		bmpc.g = c.g;
		bmpc.b = c.r;

		BMPColor* cols = reinterpret_cast<BMPColor*>(_buffer);
		std::fill(cols, cols + _width * _height, std::move(bmpc));
	}

	Color BMP::Sampler(float u, float v, glm::vec2& ddx, glm::vec2& ddy) const
	{
		ddx.x *= _width;
		ddy.x *= _width;
		ddx.y *= _height;
		ddy.y *= _height;

		float dotDDx = ddx.x * ddx.x + ddx.y * ddx.y;
		float dotDDy = ddy.x * ddy.x + ddy.y * ddy.y;

		float rho = glm::max(dotDDx, dotDDy);
		float lambda = 0.5 * log2(rho);

		float d = glm::max(lambda, 0.0f);


		return Sampler(u, v, d);
	}
}
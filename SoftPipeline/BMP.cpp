#include "BMP.h"
#include <cstdlib>
#include <cassert>
#include <cstdio>

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

	Color Color::black(0,0,0);
	Color Color::white(1, 1, 1);
	Color Color::red(1,0,0);
	Color Color::green(0, 1,0);
	Color Color::blue(0,0, 1);

	UINT32 BMP::GetWidth()const
	{
		return _width;
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

	void BMP::SetOutPut(const char * fileName, unsigned width, unsigned height)
	{
		
		this->_fileName = fileName;
		this->_width = width;
		this->_height = height;
		this->_buffer = NULL;
		this->_rowSize = 0;

		assert(_width);
		assert(_height);

		_rowSize = ALIGN(_width, 4);
		UINT32 imageSize = _height * _rowSize * BYTES_OF_PIXEL;// assert the bits of pixel is 24 = 3 * 8
		
		// initialize the DIB header
		bitmapFileHeader.signature = _SIGNATURE;
		bitmapFileHeader.fileSize = sizeof(BITMAPFILEHEADER) + sizeof(DIBHEADER) + imageSize;
		bitmapFileHeader.reserved = 0;
		bitmapFileHeader.fileOffsetToPixelArray = sizeof(BITMAPFILEHEADER) + sizeof(DIBHEADER);


		// initialize the DIB header
		dibHeader.dibHeaderSize = sizeof(DIBHEADER);
		dibHeader.imageWidth = _width;
		dibHeader.imageHeight = _height;
		dibHeader.planes = 1;
		dibHeader.bitsPerPixel = _BITS_OF_PIXEL;
		dibHeader.compression = _COMPRESSION;
		dibHeader.imageSize = sizeof(UINT8) * imageSize;
		dibHeader.xPiexlPerMeter = _X_PIXEL_PER_METER;
		dibHeader.yPiexlPerMeter = _Y_PIXEL_PER_METER;
		dibHeader.colorsInColorTable = _COLORS_IN_COLOR_TABLE;
		dibHeader.importantColorCount = _IMPORTANT_COLOR_COUNT;

		// malloc the memories of buffer
		_buffer = static_cast<UINT8*>(malloc(dibHeader.imageSize));
	}

	void BMP::ReadFrom(const char * fileName)
	{
		this->_fileName = fileName;

		FILE* f = fopen(fileName, "rb");
	
		fread(&(this->bitmapFileHeader), sizeof(BITMAPFILEHEADER), 1, f);
		fread(&(this->dibHeader), sizeof(DIBHEADER), 1, f);

		if (this->_buffer)
			free(this->_buffer);
		this->_buffer = static_cast<UINT8*>(malloc(dibHeader.imageSize));

		fread(this->_buffer, sizeof(UINT8), dibHeader.imageSize,f);

		this->_width = dibHeader.imageWidth;
		this->_height = dibHeader.imageHeight;
		this->_rowSize = ALIGN(_width, 4);

		assert(_width);
		assert(_height);


		fclose(f);
	}

	BMP::~BMP()
	{
		free(_buffer);
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
		r = fmax(0, r);
		r = fmin(1, r);
		b = fmax(0, b);
		b = fmin(1, b);
		g = fmax(0, g);
		g = fmin(1, g);

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

		fwrite(&bitmapFileHeader, sizeof(bitmapFileHeader),1,f);
		fwrite(&dibHeader, sizeof(dibHeader), 1, f);
		fwrite(_buffer, dibHeader.imageSize, 1, f);

		fclose(f);
	}

	Color::Color():Color(0,0,0)
	{
	}

	Color::Color(float r, float g, float b) : r(r), g(g), b(b)
	{
	}


	Color::Color(const Color & color):Color(color.r, color.g, color.b)
	{
		
	}

	Color Color::add(const Color & color)const
	{
		return Color(r + color.r,g + color.g,b + color.b);
	}

	Color Color::modulate(const Color & color)const
	{
		return Color(r * color.r, g * color.g, b * color.b);
	}

	Color Color::multiply(float s)const
	{
		return Color(r * s, g * s, b * s);
	}
}
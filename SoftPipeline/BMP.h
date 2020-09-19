#ifndef _RENDERTARGET_H_
#define _RENDERTARGET_H_

#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <string>

namespace BMP {

	typedef unsigned short UINT16;
	typedef unsigned UINT32;
	typedef unsigned char UINT8;
	typedef UINT8 ColorPass;

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

	struct Color
	{
		float r;
		float g;
		float b;

		Color();
		Color(float r, float g, float b);
		Color(const Color& color);

		Color add(const Color& color)const;
		Color modulate(const Color& color)const;
		Color multiply(float s)const;

		static Color black;
		static Color white;
		static Color red;
		static Color green;
		static Color blue;
	};

	struct Pixel
	{
		Color color;
		UINT32 x;
		UINT32 y;
	};

#pragma pack(pop)

	class BMP
	{
	private:

		std::string _fileName;
		UINT32 _width;
		UINT32 _height;
		UINT8* _buffer;
		UINT32 _rowSize;
		BITMAPFILEHEADER bitmapFileHeader;
		DIBHEADER dibHeader;

	public:

		UINT32 GetWidth()const;
		UINT32 GetHeight()const;

		BMP();

		void SetOutPut(const char * fileName, unsigned width, unsigned height);

		void ReadFrom(const char * fileName);

		~BMP();

		void drawPixelAt(ColorPass r, ColorPass g, ColorPass b, unsigned x, unsigned y);

		void drawPixelAt(float r, float g, float b, unsigned x, unsigned y);

		void drawPixelAt(const Color& c, unsigned x, unsigned y);

		void GetColorAt(unsigned x, unsigned y, Color* color) const;

		void writeImage(const char* name = NULL);
	};

}
#endif // !_RENDERTARGET_H_

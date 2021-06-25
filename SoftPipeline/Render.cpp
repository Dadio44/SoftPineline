#include "Render.h"

void Render::GetVsInputs(const Mesh& mesh, std::vector<VertexInput>& vsInput)
{
	auto indices = mesh.GetIndices();
	auto indicesCount = mesh.GetIndicesCount();

	auto vertices = mesh.GetVerices();
	auto normal = mesh.GetNormals();
	auto uv = mesh.GetUVs();

	int index;

	for (int i = 0; i < indicesCount; i++)
	{
		index = indices[i] * 3;

		vsInput[i].position = glm::vec4(vertices[index], vertices[index + 1], vertices[index + 2], 1);
		vsInput[i].normal = glm::vec4(normal[index], normal[index + 1], normal[index + 2], 0);

		index = indices[i] * 2;
		vsInput[i].uv = glm::vec2(uv[index], uv[index + 1]);
	}
}

void Render::VertexShader(const std::vector<VertexInput>& vsInput, std::vector<VertexOutPut>& vsOutput)
{
	glm::mat4x4 model = glm::mat4x4(1);
	model = glm::translate(model, currentPos);
	//model = glm::rotate(model,glm::radians(30.0f), glm::vec3(1, 0, -1.0f));
	//model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0, 1.0, 0));

	glm::mat4x4 view = glm::lookAt(glm::vec3(0, 0, 2.5f), glm::vec3(0), glm::vec3(0, 1, 0));
	glm::mat4x4 projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);

	glm::mat4x4 vp = projection * view;

	int cnt = vsInput.size();

	for (int i = 0; i < cnt; i++)
	{
		vsOutput[i].position = model * vsInput[i].position;
		vsOutput[i].normal = model * vsInput[i].normal;
		vsOutput[i].sv_position = vp * vsOutput[i].position;
		vsOutput[i].uv = vsInput[i].uv;
	}
}

void Render::Rasterize(const std::vector<VertexOutPut>& vsOutput)
{
	int size = vsOutput.size();

	for (int i = 0; i < size; i += 3)
	{
		DrawTriangle(
			GetRasterOutput(vsOutput[i]),
			GetRasterOutput(vsOutput[i + 1]),
			GetRasterOutput(vsOutput[i + 2]));
	}

}

void Render::DrawTriangle(const RasterOutput& v1, const RasterOutput& v2, const RasterOutput& v3)
{
	int minX = glm::min(v1.screenPos.x, v2.screenPos.x);
	minX = glm::min(v3.screenPos.x, minX);
	minX = glm::max(0, minX);
	minX = glm::min(width - 1, minX);

	int maxX = glm::max(v1.screenPos.x, v2.screenPos.x);
	maxX = glm::max(v3.screenPos.x, maxX);
	maxX = glm::max(0, maxX);
	maxX = glm::min(width - 1, maxX);

	int minY = glm::min(v1.screenPos.y, v2.screenPos.y);
	minY = glm::min(v3.screenPos.y, minY);
	minY = glm::max(0, minY);
	minY = glm::min(height - 1, minY);

	int maxY = glm::max(v1.screenPos.y, v2.screenPos.y);
	maxY = glm::max(v3.screenPos.y, maxY);
	maxY = glm::max(0, maxY);
	maxY = glm::min(height - 1, maxY);

	assert(0 <= minX && minX <= maxX && maxX <= width);
	assert(0 <= minY && minY <= maxY && maxY <= height);

	const glm::vec2& A = v1.screenPos;
	const glm::vec2& B = v2.screenPos;
	const glm::vec2& C = v3.screenPos;
	glm::vec2 P = glm::vec2(minX, minY);

	float i01 = A.y - B.y;
	float j01 = B.x - A.x;
	float k01 = A.x * B.y - A.y * B.x;
	float f01 = i01 * P.x + j01 * P.y + k01;

	float i02 = B.y - C.y;
	float j02 = C.x - B.x;
	float k02 = B.x * C.y - B.y * C.x;
	float f02 = i02 * P.x + j02 * P.y + k02;

	float i03 = C.y - A.y;
	float j03 = A.x - C.x;
	float k03 = C.x * A.y - C.y * A.x;
	float f03 = i03 * P.x + j03 * P.y + k03;

	float cy1 = f01;
	float cy2 = f02;
	float cy3 = f03;

	float cx1;
	float cx2;
	float cx3;

	float square = f01 + f02 + f03;

	//���С�ڵ���0������
	if (square <= 0)
	{
		return;
	}

	float oneDevidesquare = 1.0f / square;

	int triWidth = maxX - minX + 1;



	for (int y = minY; y <= maxY; y++)
	{
		cx1 = cy1;
		cx2 = cy2;
		cx3 = cy3;

		for (int x = minX; x <= maxX; x++)
		{
			int srcPosIndex = x + y * width;
			if ((cx1 >= 0 && cx2 >= 0 && cx3 >= 0))
			{
				rasterOutBuffer[srcPosIndex] = GetInterpolationValue(v1, v2, v3, cx2 * oneDevidesquare, cx3 * oneDevidesquare, cx1 * oneDevidesquare, x, y);
			}
			else
			{
				rasterOutBuffer[srcPosIndex].screenPos.x = -1;
			}

			cx1 += i01; cx2 += i02; cx3 += i03;
		}

		cy1 += j01;
		cy2 += j02;
		cy3 += j03;
	}

	int preX;
	int preY;

	for (int y = minY; y <= maxY; y++)
	{
		for (int x = minX; x <= maxX; x++)
		{
			int srcPosIndex = x + y * width;
			RasterOutput& ro = rasterOutBuffer[srcPosIndex];

			if (ro.screenPos.x < 0)
				continue;

			// ��Ȳ���
			if (depthBuffer[srcPosIndex] > ro.sv_position.z)
			{

				preX = srcPosIndex;
				if (x <= minX)
				{
					if (x < maxX)
					{
						preX++;
					}
				}
				else
				{
					if (rasterOutBuffer[preX - 1].screenPos.x < 0)
					{
						if (x < maxX)
						{
							preX++;
						}
					}
					else
					{
						preX--;
					}
				}

				preY = srcPosIndex;
				if (y <= minY)
				{
					if (y < maxY)
					{
						preY += width;
					}
				}
				else
				{
					if (rasterOutBuffer[preY - width].screenPos.x < 0)
					{
						if (y < maxY)
						{
							preY += width;
						}
					}
					else
					{
						preY -= width;
					}
				}

				glm::vec2 dx = rasterOutBuffer[preX].screenPos.x >= 0 ? (rasterOutBuffer[preX].uv - ro.uv) : glm::vec2(0, 0);
				glm::vec2 dy = rasterOutBuffer[preY].screenPos.x >= 0 ? (rasterOutBuffer[preY].uv - ro.uv) : glm::vec2(0, 0);

				dx.x *= texture->GetWidth();
				dy.x *= texture->GetWidth();
				dx.y *= texture->GetHeight();
				dy.y *= texture->GetHeight();


				float rho = glm::max(glm::dot(dx, dx), glm::dot(dy, dy));
				float lambda = 0.5 * log2(rho);

				float d = glm::max(lambda, 0.0f);


				DrawPixel(ro, x, y, d);

				depthBuffer[srcPosIndex] = ro.sv_position.z;

			}
		}
	}
}

void Render::DrawPixel(
	const RasterOutput& v,
	int x,
	int y,
	float mipmapLv)
{
	PixelShader(
		v,
		rt,
		*texture,
		depthBuffer,
		mipmapLv);
}

RasterOutput Render::GetInterpolationValue(
	const RasterOutput& v1,
	const RasterOutput& v2,
	const RasterOutput& v3,
	float u,
	float v,
	float w,
	int x,
	int y)
{
	RasterOutput res;

	res.screenPos.x = x;
	res.screenPos.y = y;

	res.sv_position = (v1.sv_position * u + v2.sv_position * v + v3.sv_position * w);

	float sw = 1 / res.sv_position.w;

	u = u * sw;
	v = v * sw;
	w = w * sw;

	res.position = (v1.position * u + v2.position * v + v3.position * w);
	res.normal = (v1.normal * u + v2.normal * v + v3.normal * w);
	res.uv = (v1.uv * u + v2.uv * v + v3.uv * w);

	return res;
}

RasterOutput Render::GetRasterOutput(const VertexOutPut& vertex)
{
	RasterOutput rasterOutput;

	float invW = 1 / vertex.sv_position.w;

	rasterOutput.sv_position = vertex.sv_position * invW;
	rasterOutput.sv_position.w = invW;
	rasterOutput.normal = vertex.normal * invW;
	rasterOutput.position = vertex.position * invW;
	rasterOutput.uv = vertex.uv * invW;
	rasterOutput.screenPos.x = (rasterOutput.sv_position.x * 0.5 + 0.5) * (width - 1);
	rasterOutput.screenPos.y = (rasterOutput.sv_position.y * 0.5 + 0.5) * (height - 1);

	return rasterOutput;
}

void Render::PixelShader(const RasterOutput& pixelInput, BMP::BMP& rt, const BMP::BMP& texture, float* depthBuffer, float mipmapLv)
{
	rt.drawPixelAt(
		Sampler(texture, pixelInput.uv.x, pixelInput.uv.y, mipmapLv),
		pixelInput.screenPos.x, pixelInput.screenPos.y);
}

void Render::ClearColor(BMP::BMP& rt)
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			rt.drawPixelAt(0.0f, 0.0f, 0.0f, i, j);
		}
	}

}

void Render::ClearDepth(float value, float* depthBuffer)
{
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			depthBuffer[j * width + i] = value;
		}
	}
}

BMP::Color Render::Lerp(BMP::Color c1, BMP::Color c2, float t)
{
	BMP::Color res;

	res.r = c1.r + (c2.r - c1.r) * t;
	res.g = c1.g + (c2.g - c1.g) * t;
	res.b = c1.b + (c2.b - c1.b) * t;

	return res;
}

BMP::Color Render::Sampler(const BMP::BMP& texture, float u, float v, float mipmapLevel)
{
	int lv = (int)mipmapLevel;

	int width;
	int height;

	texture.GetResolution(lv, width, height);

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


	BMP::Color c0 = texture.GetColorAt(x, y, lv);
	BMP::Color c1 = texture.GetColorAt(x1, y1, lv);
	BMP::Color c2 = texture.GetColorAt(x2, y2, lv);
	BMP::Color c3 = texture.GetColorAt(x3, y3, lv);


	float ht = glm::fract(ux);
	float vt = glm::fract(uy);

	auto ch1 = Lerp(c0, c1, ht);
	auto ch2 = Lerp(c2, c3, ht);

	return Lerp(ch1, ch2, vt);
}

void Render::Draw(const Mesh& mesh, const BMP::BMP& texture , const glm::vec3& pos)
{
	currentPos = pos;
	
	std::vector<VertexInput> vsInput(mesh.GetIndicesCount());

	GetVsInputs(mesh, vsInput);

	std::vector<VertexOutPut> vsOutput(vsInput.size());
	VertexShader(vsInput, vsOutput);

	this->texture = &texture;
	//texture.ReadFrom(texturePath);
	//texture.GenerateMipMap();
	//std::string mipmapPath(texturePath);
	//texture.writeMipMapImage(mipmapPath.replace(mipmapPath.find(".bmp"),4, "_mipmap.bmp").c_str());

	Rasterize(vsOutput);
}
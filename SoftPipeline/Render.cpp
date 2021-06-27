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

std::vector<glm::vec4> Render::_ViewLines = {
	//Near
	glm::vec4(0,0,1,1),
	//far
	glm::vec4(0,0,-1,1),
	//left
	glm::vec4(1,0,0,1),
	//top
	glm::vec4(0,1,0,1),
	//right
	glm::vec4(-1,0,0,1),
	//bottom 
	glm::vec4(0,-1,0,1)
};

bool  Render::Inside(const glm::vec4& line, const glm::vec4& p) {
	return line.x * p.x + line.y * p.y + line.z * p.z + line.w * p.w >= 0;
}

//交点，通过端点插值
VertexOutPut  Render::Intersect(
	const VertexOutPut& v1, 
	const VertexOutPut& v2, 
	const glm::vec4& line) {
	
	float da = 
		v1.sv_position.x * line.x + 
		v1.sv_position.y * line.y + 
		v1.sv_position.z * line.z + 
		line.w * v1.sv_position.w;
	float db = 
		v2.sv_position.x * line.x + 
		v2.sv_position.y * line.y + 
		v2.sv_position.z * line.z + 
		line.w * v2.sv_position.w;

	float weight = da / (da - db);
	return VertexOutPut::lerp(v1, v2, weight);
}

bool Render::AllVertexsInside(const std::vector<VertexOutPut> input)
{
	for (int i = 0; i < _ViewLines.size(); i++) 
	{
		for (int j = 0; j < input.size(); j++) 
		{
			if (!Inside(_ViewLines[i], input[j].sv_position))
			{
				return false;
			}
		}

	}

	return true;
}

//输入 三个顶点 输出 裁剪后的顶点组
int Render::SutherlandHodgeman(
	const VertexOutPut& v1,
	const VertexOutPut& v2, 
	const VertexOutPut& v3) {
	
	int outputSize = 3;

	_curOutputBufIndex = 0;
	

	_cullResBufs[_curOutputBufIndex][0] = v1;
	_cullResBufs[_curOutputBufIndex][1] = v2;
	_cullResBufs[_curOutputBufIndex][2] = v3;

	if (AllVertexsInside(_cullResBufs[_curOutputBufIndex])) {
		return outputSize;
	}

	int inputSize = 0;

	for (int i = 0; i < _ViewLines.size(); i++) {

		
		std::vector<VertexOutPut>& input = _cullResBufs[i];
		inputSize = outputSize;

		_curOutputBufIndex = i + 1;
		std::vector<VertexOutPut>& output = _cullResBufs[_curOutputBufIndex];
		outputSize = 0;

		for (int j = 0; j < inputSize; j++) {
			VertexOutPut current = input[j];
			VertexOutPut last = input[(j + inputSize - 1) % inputSize];
			if (Inside(_ViewLines[i], current.sv_position)) {
				if (!Inside(_ViewLines[i], last.sv_position)) {
					VertexOutPut intersecting = Intersect(last, current, _ViewLines[i]);
					output[outputSize++] = intersecting;
				}
				output[outputSize++] = current;
			}
			else if (Inside(_ViewLines[i], last.sv_position)) {
				VertexOutPut intersecting = Intersect(last, current, _ViewLines[i]);
				output[outputSize++] = intersecting;
			}
		}
	}

	return outputSize;
}

void Render::Rasterize(const std::vector<VertexOutPut>& vsOutput, int verticesCount)
{
	for (int i = 0; i < verticesCount; i += 3)
	{
		int cnt = SutherlandHodgeman(
			_vsout[i],
			_vsout[i + 1],
			_vsout[i + 2]);

		//GL_TRIANGLES_FAN方式组装
		for (int j = 2; j < cnt; j++)
		{
			DrawTriangle(
				GetRasterOutput(_cullResBufs[_curOutputBufIndex][0]),
				GetRasterOutput(_cullResBufs[_curOutputBufIndex][j - 1]),
				GetRasterOutput(_cullResBufs[_curOutputBufIndex][j]));
		}
	}

}

void Render::DrawTriangle(const RasterOutput& v1, const RasterOutput& v2, const RasterOutput& v3)
{
	int minX = glm::min(v1.screenPos.x, v2.screenPos.x);
	minX = glm::min(v3.screenPos.x, minX);
	minX = glm::max(0, minX);
	minX = glm::min(_width - 1, minX);

	int maxX = glm::max(v1.screenPos.x, v2.screenPos.x);
	maxX = glm::max(v3.screenPos.x, maxX);
	maxX = glm::max(0, maxX);
	maxX = glm::min(_width - 1, maxX);

	int minY = glm::min(v1.screenPos.y, v2.screenPos.y);
	minY = glm::min(v3.screenPos.y, minY);
	minY = glm::max(0, minY);
	minY = glm::min(_height - 1, minY);

	int maxY = glm::max(v1.screenPos.y, v2.screenPos.y);
	maxY = glm::max(v3.screenPos.y, maxY);
	maxY = glm::max(0, maxY);
	maxY = glm::min(_height - 1, maxY);

	assert(0 <= minX && minX <= maxX && maxX <= _width);
	assert(0 <= minY && minY <= maxY && maxY <= _height);

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

	//面积等于0，丢弃
	if (square == 0)
	{
		return;
	}

	switch (_cullingtype)
	{
	case CullingType::Back:
		if (square < 0)
			return;
		break;
	case CullingType::Front:
		if (square > 0)
			return;
		break;
	case CullingType::None:
		break;
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
			int srcPosIndex = x + y * _width;
			if ((cx1 >= 0 && cx2 >= 0 && cx3 >= 0) || (cx1 <= 0 && cx2 <= 0 && cx3 <= 0))
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
			int srcPosIndex = x + y * _width;
			RasterOutput& ro = rasterOutBuffer[srcPosIndex];

			if (ro.screenPos.x < 0)
				continue;

			// 深度测试
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
						preY += _width;
					}
				}
				else
				{
					if (rasterOutBuffer[preY - _width].screenPos.x < 0)
					{
						if (y < maxY)
						{
							preY += _width;
						}
					}
					else
					{
						preY -= _width;
					}
				}


				RasterOutput& rox = rasterOutBuffer[preX].screenPos.x >= 0 ? rasterOutBuffer[preX] : ro;
				RasterOutput& roy = rasterOutBuffer[preY].screenPos.x >= 0 ? rasterOutBuffer[preY] : ro;

				DrawPixel(ro,rox,roy);

				depthBuffer[srcPosIndex] = ro.sv_position.z;

			}
		}
	}
}

void Render::DrawPixel(const RasterOutput& v, const RasterOutput& dx, const RasterOutput& dy)
{
	auto color = _ps->PixelShader(v, dx, dy);
	if (_enabledBlend)
	{
		Color dstCol;
		rt.GetColorAt(v.screenPos.x, v.screenPos.y, &dstCol);
		
		auto blendCol = color.multiply(color.a).add(dstCol.multiply(1 - color.a));
	
		rt.drawPixelAt(blendCol, v.screenPos.x, v.screenPos.y);
	}
	else
	{
		rt.drawPixelAt(color, v.screenPos.x, v.screenPos.y);
	}
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
	//透视除法
	float invW = 1 / vertex.sv_position.w;

	rasterOutput.sv_position = vertex.sv_position * invW;
	rasterOutput.sv_position.w = invW;
	rasterOutput.normal = vertex.normal * invW;
	rasterOutput.position = vertex.position * invW;
	rasterOutput.uv = vertex.uv * invW;
	rasterOutput.screenPos.x = (rasterOutput.sv_position.x * 0.5 + 0.5) * (_width - 1);
	rasterOutput.screenPos.y = (rasterOutput.sv_position.y * 0.5 + 0.5) * (_height - 1);


	return rasterOutput;
}


void Render::ClearColor(BMP::BMP& rt)
{
	for (int i = 0; i < _width; i++)
	{
		for (int j = 0; j < _height; j++)
		{
			rt.drawPixelAt(0.0f, 0.0f, 0.0f, i, j);
		}
	}

}

void Render::ClearDepth(float value, float* depthBuffer)
{
	for (int j = 0; j < _height; j++)
	{
		for (int i = 0; i < _width; i++)
		{
			depthBuffer[j * _width + i] = value;
		}
	}
}

void Render::SetShader(const IVertexShader* vs, const IPixelShader* ps)
{
	_vs = vs;
	_ps = ps;
}



void Render::Draw(const Mesh& mesh)
{
	auto verticesCount = mesh.GetIndicesCount();

	if (_vsInput.size() < verticesCount)
		_vsInput.resize(verticesCount);
	
	GetVsInputs(mesh, _vsInput);

	if (_vsout.size() < verticesCount)
		_vsout.resize(verticesCount);
	
	_vs->VertexShader(_vsInput, _vsout, verticesCount);
	//texture.ReadFrom(texturePath);
	//texture.GenerateMipMap();
	//std::string mipmapPath(texturePath);
	//texture.writeMipMapImage(mipmapPath.replace(mipmapPath.find(".bmp"),4, "_mipmap.bmp").c_str());

	Rasterize(_vsout, verticesCount);

}

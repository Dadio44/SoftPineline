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
	//面法线的w分量始终为 1
	return line.x * p.x + line.y * p.y + line.z * p.z + p.w >= 0;
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

bool Render::AllVertexsInside(const std::vector<VertexOutPut>& input)
{
	auto sizeOfline = _ViewLines.size();
	auto sizeOfInputs = input.size();
	for (int i = 0; i < sizeOfline; i++) 
	{
		for (int j = 0; j < sizeOfInputs; j++) 
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

	assert(0 <= minX && minX <= maxX && maxX < _width);
	assert(0 <= minY && minY <= maxY && maxY < _height);

	const glm::ivec2& A = v1.screenPos;
	const glm::ivec2& B = v2.screenPos;
	const glm::ivec2& C = v3.screenPos;
	glm::ivec2 P = glm::ivec2(minX, minY);

	int i01 = A.y - B.y;
	int j01 = B.x - A.x;
	int k01 = A.x * B.y - A.y * B.x;
	int f01 = i01 * P.x + j01 * P.y + k01;

	int i02 = B.y - C.y;
	int j02 = C.x - B.x;
	int k02 = B.x * C.y - B.y * C.x;
	int f02 = i02 * P.x + j02 * P.y + k02;

	int i03 = C.y - A.y;
	int j03 = A.x - C.x;
	int k03 = C.x * A.y - C.y * A.x;
	int f03 = i03 * P.x + j03 * P.y + k03;

	int cy1 = f01;
	int cy2 = f02;
	int cy3 = f03;

	int cx1;
	int cx2;
	int cx3;

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

	//此处需要是 double ，否则重心坐标（uvw）精度不足，会导致三角形邻接片元重复
	double oneDevidesquare = 1.0 / square;


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

	for (int y = minY; y < maxY; y++)
	{
		for (int x = minX ; x < maxX; x++)
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
	auto srcColor = _ps->PixelShader(v, dx, dy);
	if (_enabledBlend)
	{
		Color dstCol;
		_rt->GetColorAt(v.screenPos.x, v.screenPos.y, &dstCol);
		
		auto blendCol = srcColor.multiply(srcColor.a).add(dstCol.multiply(1 - srcColor.a));
	
		_rt->drawPixelAt(blendCol, v.screenPos.x, v.screenPos.y);
	}
	else
	{
		_rt->drawPixelAt(srcColor, v.screenPos.x, v.screenPos.y);
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

	

	float invW = (v1.sv_position.w * u + v2.sv_position.w * v + v3.sv_position.w * w);

	float W = 1.0f / invW;

	res.sv_position.z = W;

	res.position = (v1.position * u + v2.position * v + v3.position * w) * W;
	res.normal = (v1.normal * u + v2.normal * v + v3.normal * w) * W;
	res.uv = (v1.uv * u + v2.uv * v + v3.uv * w) * W;



	return res;
}

RasterOutput Render::GetRasterOutput(const VertexOutPut& vertex)
{
	RasterOutput rasterOutput;
	float w = vertex.sv_position.w;

	//透视除法
	float invW = 1.0f / w;

	rasterOutput.sv_position = vertex.sv_position * invW;
	rasterOutput.sv_position.w = invW;
	
	rasterOutput.normal.x = vertex.normal.x * invW;
	rasterOutput.normal.y = vertex.normal.y * invW;
	rasterOutput.normal.z = vertex.normal.z * invW;

	rasterOutput.position.x = vertex.position.x * invW;
	rasterOutput.position.y = vertex.position.y * invW;
	rasterOutput.position.z = vertex.position.z * invW;

	rasterOutput.uv = vertex.uv * invW;
	//四舍五入，消除接缝
	rasterOutput.screenPos.x = ((rasterOutput.sv_position.x * 0.5f + 0.5f) * (_width - 1) + 0.5f);
	rasterOutput.screenPos.y = ((rasterOutput.sv_position.y * 0.5f + 0.5f) * (_height - 1) + 0.5f);


	return rasterOutput;
}

void Render::ClearColor(const Color& color)
{
	_rt->ClearColor(color);
}

void Render::ClearDepth(float value)
{
	std::fill(depthBuffer, depthBuffer + _width * _height, value);
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

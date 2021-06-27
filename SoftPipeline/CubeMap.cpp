#include "CubeMap.h"

void CubeMap::Load(std::vector<std::string> fileName)
{
	assert(fileName.size() == 6);

	for (auto path : fileName)
	{
		BMP::BMP* bmp = new BMP::BMP();
		bmp->ReadFrom(path.c_str());
		_textures.push_back(bmp);
	}

}

CubeMapTexDiretcion CubeMap::GetTarget(float x, float y, float z, float& s, float& t)const
{

	CubeMapTexDiretcion targetDir;

	float mx = abs(x);
	float my = abs(y);
	float mz = abs(z);

	float sc, tc, ma;

	if (mx > my && mx > mz)
	{
		if (x > 0)
		{
			targetDir = CubeMapTexDiretcion::POSITIVE_X;
			sc = -z;
		}
		else
		{
			targetDir = CubeMapTexDiretcion::NEGATIVE_X;
			sc = z;
		}
		ma = x;
		tc = -y;

	}
	else
	{
		if (my > mz)
		{
			if (y > 0)
			{
				targetDir = CubeMapTexDiretcion::POSITIVE_Y;
				tc = z;
			}
			else
			{
				targetDir = CubeMapTexDiretcion::NEGATIVE_Y;
				tc = -z;
			}
			sc = x;
			ma = y;
		}
		else
		{
			if (z > 0)
			{
				targetDir = CubeMapTexDiretcion::POSITIVE_Z;
				sc = x;
			}
			else
			{
				targetDir = CubeMapTexDiretcion::NEGATIVE_Z;
				sc = -x;
			}
			tc = -y;
			ma = z;
		}
	}

	assert(targetDir != None);

	s = 0.5f * (sc / abs(ma) + 1);
	t = 0.5f * (tc / abs(ma) + 1);

	return targetDir;
}

void CubeMap::GetColorAt(CubeMapTexDiretcion direction,unsigned x, unsigned y, Color* color) const
{
	return _textures[direction]->GetColorAt(x, y, color);
}

Color CubeMap::Sampler(float x, float y, float z, float mipmapLevel) const
{
	float s, t;
	auto target = GetTarget(x, y, z, s, t);

	return _textures[target]->Sampler(s, t, mipmapLevel);
}

Color CubeMap::Sampler(float x, float y, float z, glm::vec2 ddx, glm::vec2 ddy) const
{
	float s, t;
	auto target = GetTarget(x, y, z, s, t);

	return _textures[target]->Sampler(s,t,ddx,ddy);
}

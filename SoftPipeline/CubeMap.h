#pragma once

#include <vector>

#include "BMP.h"
enum CubeMapTexDiretcion
{
	POSITIVE_X,
	NEGATIVE_X,
	POSITIVE_Y,
	NEGATIVE_Y,
	POSITIVE_Z,
	NEGATIVE_Z,
};


class CubeMap
{
public:

	void Load(std::vector<std::string> fileName);

	void GetColorAt(CubeMapTexDiretcion direction,unsigned x, unsigned y, Color* color) const;

	Color Sampler(float x, float y, float z,float mipmapLevel)const;

	Color Sampler(float x, float y, float z, glm::vec2 ddx, glm::vec2 ddy)const;


	~CubeMap()
	{
		for (auto tex : _textures)
		{
			delete tex;
		}
	}

private:
	std::vector<BMP::BMP*> _textures;
	CubeMapTexDiretcion GetTarget(float x, float y, float z, float& s, float& t)const;

};


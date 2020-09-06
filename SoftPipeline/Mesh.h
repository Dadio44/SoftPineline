#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "glm/glm.hpp"

class Mesh
{
public :
	Mesh();
	void LoadFromFile(const char * filaName);
	~Mesh();
	float* GetVerices()const;
	int GetVericesCount()const;
	float* GetNormals()const;
	int GetNormalsCount()const;
	float* GetUVs()const;
	int GetUVsCount()const;
	int* GetIndices()const;
	int GetIndicesCount()const;
private:
	unsigned char * _buffer;
	int _verticesCount;
	int _indicesCount;
	int _normalsCount;
	int _uvsCount;
};


#include "Mesh.h"
#include <iostream>
#include <assert.h>

Mesh::Mesh():
	_buffer(nullptr),
	_verticesCount(0),
	_normalsCount(0),
	_uvsCount(0),
	_indicesCount(0)
{
}

void Mesh::LoadFromFile(const char * filaName)
{
	FILE* f = fopen(filaName, "rb");
	assert(f);

	if (_buffer != nullptr)
	{
		free(_buffer);
	}

	_buffer = static_cast<unsigned char*>(malloc(sizeof(int) * 4));

	auto readCnt = fread(_buffer, sizeof(int), 4, f);
	assert(readCnt == 4);

	_verticesCount = *reinterpret_cast<int*>(_buffer);
	_normalsCount = *reinterpret_cast<int*>(_buffer + sizeof(int));
	_uvsCount = *reinterpret_cast<int*>(_buffer + sizeof(int) * 2);
	_indicesCount = *reinterpret_cast<int*>(_buffer + sizeof(int) * 3);

	free(_buffer);

	auto size =
		_verticesCount * sizeof(float) * 3 +
		_normalsCount * sizeof(float) * 3 +
		_uvsCount * sizeof(float) * 2 +
		_indicesCount * sizeof(int);

	_buffer = static_cast<unsigned char*>(malloc(size));

	readCnt = fread(_buffer, sizeof(unsigned char), size, f);
	assert(readCnt == size);

	
	fclose(f);

}

Mesh::~Mesh()
{
	if (_buffer != nullptr)
	{
		free(_buffer);
	}
}

float * Mesh::GetVerices()const
{
	return reinterpret_cast<float*>(_buffer);
}

int Mesh::GetVericesCount()const
{
	return _verticesCount;
}

float * Mesh::GetNormals()const
{
	return reinterpret_cast<float*>(_buffer + 
		sizeof(float) * 3 * _verticesCount);
}

int Mesh::GetNormalsCount()const
{
	return _normalsCount;
}

float * Mesh::GetUVs()const
{
	return reinterpret_cast<float*>(_buffer +
		sizeof(float) * 3 * _verticesCount +
		sizeof(float) * 3 * _normalsCount);
}

int Mesh::GetUVsCount()const
{
	return _uvsCount;
}

int * Mesh::GetIndices()const
{
	return reinterpret_cast<int*>(_buffer +
		sizeof(float) * 3 * _verticesCount +
		sizeof(float) * 3 * _normalsCount + 
		sizeof(float) * 2 * _uvsCount);
}

int Mesh::GetIndicesCount()const
{
	return _indicesCount;
}

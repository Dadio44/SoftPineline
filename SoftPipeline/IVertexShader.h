#pragma once

#include <vector>
#include "VertexInput.h"
#include "VertexOutPut.h"

class IVertexShader
{
public:
	virtual void VertexShader(
		const std::vector<VertexInput>& vsInput, 
		std::vector<VertexOutPut>& vsOutput,
		int verticesCount)const = 0;
};


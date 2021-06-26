#pragma once
#include "IVertexShader.h"
#include "IPixelShader.h"
#include "ICullOption.h"

class IMaterial : 
	public IVertexShader, 
	public IPixelShader,
	public ICullOption
{
	virtual CullingType GetCullingOption() override
	{
		return CullingType::Back;
	}
};


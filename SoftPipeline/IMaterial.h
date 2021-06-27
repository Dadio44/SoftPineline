#pragma once
#include "IVertexShader.h"
#include "IPixelShader.h"
#include "ICullOption.h"
#include "IBlendOption.h"

class IMaterial : 
	public IVertexShader, 
	public IPixelShader,
	public ICullOption,
	public IBlendOption
{
	virtual CullingType GetCullingOption() override
	{
		return CullingType::Back;
	}

	virtual bool GetBlendOption() override
	{
		return false;
	}
};


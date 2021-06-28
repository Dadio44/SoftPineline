#pragma once
#include "CubeMap.h"
#include "IMaterial.h"
#include "IModelProp.h"
#include "IVPProp.h"

class SkyBoxMaterial : 
	public IMaterial,
	public IModelProp,
	public IVPProp
{
public:
	// 通过 IMaterial 继承
	virtual void VertexShader(const std::vector<VertexInput>& vsInput, std::vector<VertexOutPut>& vsOutput, int verticesCount) const override;
	virtual Color PixelShader(const RasterOutput& pixelInput, const RasterOutput& dx, const RasterOutput& dy) const override;

	// 通过 IModelProp 继承
	virtual void SetModel(glm::mat4x4) override;

	// 通过 IVPProp 继承
	virtual void SetViewProjection(glm::mat4x4 view, glm::mat4x4 projection) override;

	// 通过 ICullOption 继承
	virtual CullingType GetCullingOption() override;

	SkyBoxMaterial(CubeMap* cubeMap):_cubeMap(cubeMap)
	{

	}

private:
	CubeMap* _cubeMap;
	glm::mat4x4 _view;
	glm::mat4x4 _projection;
};


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
	// ͨ�� IMaterial �̳�
	virtual void VertexShader(const std::vector<VertexInput>& vsInput, std::vector<VertexOutPut>& vsOutput, int verticesCount) const override;
	virtual Color PixelShader(const RasterOutput& pixelInput, const RasterOutput& dx, const RasterOutput& dy) const override;

	// ͨ�� IModelProp �̳�
	virtual void SetModel(glm::mat4x4) override;

	// ͨ�� IVPProp �̳�
	virtual void SetViewProjection(glm::mat4x4 view, glm::mat4x4 projection) override;

	// ͨ�� ICullOption �̳�
	virtual CullingType GetCullingOption() override;

	SkyBoxMaterial(CubeMap* cubeMap):_cubeMap(cubeMap)
	{

	}

private:
	CubeMap* _cubeMap;
	glm::mat4x4 _view;
	glm::mat4x4 _projection;
};


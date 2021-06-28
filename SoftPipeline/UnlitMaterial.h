#pragma once
#include "IMaterial.h"
#include "IResouceManager.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "BMP.h"
#include "IModelProp.h"
#include "IVPProp.h"

class UnlitMaterial :
    public IMaterial, 
    public IModelProp,
    public IVPProp
{
public:
    // 通过 IMaterial 继承
    virtual void VertexShader(
        const std::vector<VertexInput>& vsInput, 
        std::vector<VertexOutPut>& vsOutput,
        int verticesCount)const override;
    virtual Color PixelShader(const RasterOutput& pixelInput, const RasterOutput& dx, const RasterOutput& dy)const override;

    virtual void SetModel(glm::mat4x4 model) override 
    {
        _model = model;
    }

    virtual void SetViewProjection(glm::mat4x4 view, glm::mat4x4 projection) override
    {
        _view = view;
        _projection = projection;
        _vp = _projection * _view;
    }

    UnlitMaterial(BMP::BMP* tex)
        :_mainTex(tex)
    {

    }
private:
    BMP::BMP* _mainTex;
    glm::mat4x4 _model;
    glm::mat4x4 _vp;
    glm::mat4x4 _view;
    glm::mat4x4 _projection;

    // 通过 ICullOption 继承
    virtual CullingType GetCullingOption() override;
};


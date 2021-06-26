#pragma once
#include "IMaterial.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "IModelProp.h"
#include "IVPProp.h"

class SimpleLitMaterail : 
	public IMaterial,
    public IModelProp,
    public IVPProp
{
public:
    // Í¨¹ý IMaterial ¼Ì³Ð
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

    SimpleLitMaterail(Color color):_color(color)
    {

    }

private :
    Color _color;
    glm::mat4x4 _model;
    glm::mat4x4 _vp;
    glm::mat4x4 _view;
    glm::mat4x4 _projection;
};


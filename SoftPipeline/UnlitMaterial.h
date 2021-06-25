#pragma once
#include "IMaterial.h"
#include "IResouceManager.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class UnlitMaterial :
    public IMaterial
{
public:
    // Í¨¹ý IMaterial ¼Ì³Ð
    virtual void VertexShader(const std::vector<VertexInput>& vsInput, std::vector<VertexOutPut>& vsOutput)const override;
    virtual BMP::Color PixelShader(const RasterOutput& pixelInput, const RasterOutput& dx, const RasterOutput& dy)const override;

    UnlitMaterial(BMP::BMP* tex,glm::vec3 pos,int width,int height) 
        :_mainTex(tex),
        _currentPos(pos),
        _width(width),
        _height(height)
    {

    }
private:
    BMP::BMP* _mainTex;
    glm::vec3 _currentPos;
    int _width;
    int _height;
};


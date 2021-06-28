#include "SkyBoxMaterial.h"

void SkyBoxMaterial::VertexShader(const std::vector<VertexInput>& vsInput, std::vector<VertexOutPut>& vsOutput, int verticesCount) const
{
    for (int i = 0; i < verticesCount; i++)
    {
        vsOutput[i].position = vsInput[i].position;
        vsOutput[i].sv_position = _projection * _view * vsInput[i].position;
        // 保证透视除法后，z = 1.0，始终位于最远处
       vsOutput[i].sv_position.z = vsOutput[i].sv_position.w;
    }
    
}

Color SkyBoxMaterial::PixelShader(const RasterOutput& pixelInput, const RasterOutput& dx, const RasterOutput& dy) const
{
    auto& pos = pixelInput.position;
    return _cubeMap->Sampler(pos.x,pos.y,pos.z,0);
}

void SkyBoxMaterial::SetModel(glm::mat4x4)
{
}

void SkyBoxMaterial::SetViewProjection(glm::mat4x4 view, glm::mat4x4 projection)
{
    //移除摄影机移动
    _view = glm::mat4(glm::mat3(view));
    _projection = projection;
}

CullingType SkyBoxMaterial::GetCullingOption()
{
    return CullingType::Front;
}

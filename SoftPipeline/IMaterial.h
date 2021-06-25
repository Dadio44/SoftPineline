#pragma once
#include "IVertexShader.h"
#include "IPixelShader.h"

class IMaterial : public IVertexShader, public IPixelShader
{

};


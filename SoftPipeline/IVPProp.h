#pragma once
#include "glm/glm.hpp"

/// <summary>
/// ≥÷”– ViewProjection æÿ’Û
/// </summary>
class IVPProp
{
public:
	virtual void SetViewProjection(glm::mat4x4 view, glm::mat4x4 projection) = 0;
};


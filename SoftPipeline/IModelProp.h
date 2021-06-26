#pragma once

#include "glm/glm.hpp"

class IModelProp
{
public :
	virtual void SetModel(glm::mat4x4) = 0;
};


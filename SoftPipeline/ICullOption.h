#pragma once

#include "Render.h"

class ICullOption
{
public:
	virtual CullingType GetCullingOption() = 0;
};


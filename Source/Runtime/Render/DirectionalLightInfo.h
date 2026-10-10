#pragma once

#include "Math/Vector4.h"

struct FDirectionalLightInfo
{
	FVector4 Direction = { 90.0f, 0.0f, 0.0f, 1.0f };
	FVector4 ColorIntensity = { 1.0f,1.0f,1.0f,1.0f };
	FVector4 EXP = { 0.0f,0.0f,0.0f,0.0f };
};
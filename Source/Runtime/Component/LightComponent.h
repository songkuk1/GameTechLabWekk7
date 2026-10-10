#pragma once

#include "SceneComponent.h"
#include "Render/LineBatcher.h"

class ULightComponent : public USceneComponent
{
	DECLARE_CLASS(ULightComponent, USceneComponent)
	REFLECT_START(ULightComponent)
	REFLECT_END()
public:
		virtual void DrawDebug(FLineBatcher* LineBatcher) const = 0;
private:
};
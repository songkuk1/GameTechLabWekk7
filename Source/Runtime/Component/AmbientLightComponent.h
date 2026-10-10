#pragma once

#include "LightComponent.h"

//언리얼에서 이런 컴포넌트는 존재하지 않음... 그나마 비슷한 거라면 skyLightComponent 인듯
class UAmbientLightComponent final : public ULightComponent
{
	DECLARE_CLASS(UAmbientLightComponent, ULightComponent)
	REFLECT_START(UAmbientLightComponent)
		PROPERTY(Intensity)
		PROPERTY_TYPE(LightColor, Color)
	REFLECT_END()

public:
	UAmbientLightComponent() = default;
	~UAmbientLightComponent() = default;

	UAmbientLightComponent(const UAmbientLightComponent&) = delete;
	UAmbientLightComponent& operator=(const UAmbientLightComponent& ) = delete;

private:
	//빛의 강도
	float Intensity = 0.2f;
	FVector4 LightColor = FVector4(1, 1, 1, 1);




};
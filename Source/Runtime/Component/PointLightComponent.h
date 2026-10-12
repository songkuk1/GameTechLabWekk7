#pragma once

#include "component/LightComponent.h"

class FLineBatcher;

class UPointLightComponent : public ULightComponent
{
	DECLARE_CLASS(UPointLightComponent, ULightComponent)

	REFLECT_START(ClassName)
		PROPERTY(Radius)
		PROPERTY(Falloff)
		PROPERTY(Intensity)
		PROPERTY_TYPE(LightColor, Color)
	REFLECT_END()

public:
	UPointLightComponent() = default;
	virtual ~UPointLightComponent() override;

	void InitializeComponent();

	void DrawDebug(FLineBatcher* LineBatcher) const override;

	float GetFalloff() const { return Falloff; }
	void SetFalloff(float InFalloff) { Falloff = std::max(0.001f, InFalloff); }

	float GetIntensity() const { return Intensity; }
	void SetIntensity(float InIntensity) { Intensity = InIntensity; }
		
	float GetRadius() const { return Radius; }
	void SetRadius(float InRadius) { Radius = std::max(0.001f, InRadius); }

	const FVector4& GetLightColor() const { return LightColor; }
	void SetLightColor(const FVector4& InColor) { LightColor = InColor; }

private:
	// 빛이 닿는 거리
	float Radius = 5.0f;

	// Inner에서 Outer로 가며 밝기가 떨어지는 정도.
	float Falloff = 1.0f;

	float Intensity = 1.0f;

	FVector4 LightColor = FVector4(1.0f, 1.0f, 0.6f, 1.0f);
};

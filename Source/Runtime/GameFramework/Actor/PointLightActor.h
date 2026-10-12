#pragma once

#include "GameFramework/Actor/LightActor.h"
#include "Component/BillboardComponent.h"
#include "Component/PointLightComponent.h"

class APointLightActor : public ALightActor
{
	DECLARE_CLASS(APointLightActor, ALightActor)

	REFLECT_START(ClassName)
	REFLECT_END()

public:
	APointLightActor();
	virtual ~APointLightActor() override = default;
	void OnSelected() override;
	void OnDeselected() override;

private:
	UBillboardComponent* Icon;
	UPointLightComponent* PointLight;
	UMaterial* onMaterial;
	UMaterial* offMaterial;

private:
	const FString UnHighLightedPointLightIconPath =
		"Assets/Icons/Component/UnHighLightedPointLight.png";
	const FString HighLightedPointLightOnPath =
		"Assets/Icons/Component/HighLightedPointLight.png";
};

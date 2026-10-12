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

private:
	const FString UnHighLightedPointLightIconPath =
		"Assets/Icons/Component/UnHighLightedPointLight.png";
	const FString HighLightedPointLightOnPath =
		"Assets/Icons/Component/HighLightedPointLight.png";
};

#pragma once

#include "GameFramework/Actor/LightActor.h"
#include "Component/BillboardComponent.h"
#include "Component/SpotLightComponent.h"

class ASpotLightActor : public ALightActor
{
	DECLARE_CLASS(ASpotLightActor, ALightActor)

	REFLECT_START(ClassName)
	REFLECT_END()

public:
	ASpotLightActor();
	virtual ~ASpotLightActor() override = default;

private:
	const FString UnHighLightedSpotLightIconPath =
		"Assets/Icons/Component/UnHighLightedSpotLight.png";
	const FString HighLightedSpotLightOnPath =
		"Assets/Icons/Component/HighLightedSpotLight.png";
};

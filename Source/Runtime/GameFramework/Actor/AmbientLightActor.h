#pragma once
#include "GameFramework/Actor/LightActor.h"


class UBillboardComponent;
class UAmbientLightComponent;

class AAmbientLightActor final : public ALightActor
{
	DECLARE_CLASS(AAmbientLightActor, AActor)


public:
	AAmbientLightActor();
	virtual ~AAmbientLightActor() override = default;

	AAmbientLightActor(const AAmbientLightActor&) = delete;
	AAmbientLightActor& operator=(const AAmbientLightActor&) = delete;

};
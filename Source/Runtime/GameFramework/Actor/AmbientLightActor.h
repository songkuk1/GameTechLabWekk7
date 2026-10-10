#pragma once
#include "GameFramework/Actor.h"


class UBillboardComponent;
class UAmbientLightComponent;

class AAmbientLightActor final : public AActor
{
	DECLARE_CLASS(AAmbientLightActor, AActor)


public:
	AAmbientLightActor();
	virtual ~AAmbientLightActor() override = default;

	AAmbientLightActor(const AAmbientLightActor&) = delete;
	AAmbientLightActor& operator=(const AAmbientLightActor&) = delete;

	UBillboardComponent* GetBillboardComponent() const { return BillboardComponent; }
	UAmbientLightComponent* GetExponentialHeightFogComponent() const { return AmbientLightComponent; }

private:
	UAmbientLightComponent* AmbientLightComponent = nullptr;
	UBillboardComponent* BillboardComponent = nullptr;



};
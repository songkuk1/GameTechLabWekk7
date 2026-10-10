#pragma once

#include "GameFramework/Actor.h"
#include "Component/BillboardComponent.h"
#include "Component/DirectionalLightComponent.h"

// 에디터에서 아이콘(빌보드)으로 보이고, 선택하면 스포트라이트 원뿔이 라인으로 그려진다.
class ADirectionalLightActor : public AActor
{
	DECLARE_CLASS(ADirectionalLightActor, AActor)
	REFLECT_START(ClassName)
	REFLECT_END()

public:
	ADirectionalLightActor();
	virtual ~ADirectionalLightActor() override = default;

	UBillboardComponent* GetBillboardComponent() const { return Icon; }
	UDirectionalLightComponent* GetDirectLightComponent() const { return DirectLight; }

private:
	UBillboardComponent* Icon;
	UDirectionalLightComponent* DirectLight;
	UStaticMeshComponent* DirectionArrow;

private: 
	const FString UnHighLightedDirectionalLightIconPath =
		"Assets/Icons/Component/UnHighLightedDirectionalLight.png";
	const FString HighLightedDirectionalLightOnPath =
		"Assets/Icons/Component/HighLightedDirectionalLightOn.png";
};
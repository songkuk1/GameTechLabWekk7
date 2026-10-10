#include "EnginePCH.h"
#include "AmbientLightActor.h"
#include "Component/AmbientLightComponent.h"
#include "Component/BillboardComponent.h"


AAmbientLightActor::AAmbientLightActor()
{
	AmbientLightComponent = CreateDefaultSubobject<UAmbientLightComponent>("UAmbientLightComponent");
	SetRootComponent(AmbientLightComponent);

	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>("UBillboardComponent");
	BillboardComponent->SetupAttachment(AmbientLightComponent, EAttachmentRule::SnapToTarget);
	BillboardComponent->SetHiddenInDetails(true);
}

#include "EnginePCH.h"
#include "LightActor.h"

ALightActor::ALightActor()
{
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>("UBillboardComponent");
	BillboardComponent->SetupAttachment(GetRootComponent());
}

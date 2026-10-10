#include "EnginePCH.h"
#include "Component/DirectionalLightComponent.h"
#include "Asset/AssetManager.h"
#include "Engine/World.h"

UDirectionalLightComponent::~UDirectionalLightComponent()
{
	if (AActor* Owner = GetOwner())
	{
		if (UWorld* World = Owner->GetWorld())
		{
			World->GetScene().UnregisterDirectLight(this);
		}
	}
}

void UDirectionalLightComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (AActor* Owner = GetOwner())
	{
		if (UWorld* World = Owner->GetWorld())
		{
			World->GetScene().RegisterDirectLight(this);
		}
	}
}
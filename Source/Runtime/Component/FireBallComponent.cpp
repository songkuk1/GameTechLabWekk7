#include "EnginePCH.h"
#include "FireBallComponent.h"
#include "Engine/World.h"

void UFireBallComponent::InitializeComponent()
{
    Super::InitializeComponent();

    if (AActor* Owner = GetOwner())
    {
        if (UWorld* World = Owner->GetWorld())
        {
            World->GetScene().RegisterPointLight(Cast<UPointLightComponent>(this));
        }
    }
}

UFireBallComponent::~UFireBallComponent()
{
    if (AActor* Owner = GetOwner())
    {
        if (UWorld* World = Owner->GetWorld())
        {
            World->GetScene().UnregisterPointLight(Cast<UPointLightComponent>(this));
        }
    }
}
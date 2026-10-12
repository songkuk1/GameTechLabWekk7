#include "EnginePCH.h"
#include "AmbientLightActor.h"
#include "Component/AmbientLightComponent.h"
#include "Asset/AssetManager.h"
#include "Component/BillboardComponent.h"


AAmbientLightActor::AAmbientLightActor()
{
	UMaterial* BaseMaterial = UAssetManager::GetAssetByPath<UMaterial>("SubUVMaterial");
	UTexture2D* IconTexture = UAssetManager::Get().LoadTexture(UnHighLightedAmbientLightIconPath);
	UMaterial* IconMaterial = UMaterial::CreateInstance(BaseMaterial);

	AmbientLightComponent = CreateDefaultSubobject<UAmbientLightComponent>("UAmbientLightComponent");
	SetRootComponent(AmbientLightComponent);

	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>("UBillboardComponent");
	BillboardComponent->SetupAttachment(AmbientLightComponent, EAttachmentRule::SnapToTarget);

	if (IconMaterial && IconTexture)
	{
		IconMaterial->Textures.Reset();
		IconMaterial->Textures.Add(IconTexture);
		BillboardComponent->SetMaterial(0, IconMaterial);
	}

	BillboardComponent->SetRelativeScale3D({ 1.0f,0.5f,0.5f });
}

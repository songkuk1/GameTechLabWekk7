#include "EnginePCH.h"
#include "DirectionalLightActor.h"
#include "Asset/AssetManager.h"
#include "Render/Material.h"

ADirectionalLightActor::ADirectionalLightActor()
{
	Icon = BillboardComponent;

	UMaterial* BaseMaterial = UAssetManager::GetAssetByPath<UMaterial>("SubUVMaterial");
	UTexture2D* IconTexture = UAssetManager::Get().LoadTexture(UnHighLightedDirectionalLightIconPath);
	UMaterial* IconMaterial = UMaterial::CreateInstance(BaseMaterial);

	if (IconMaterial && IconTexture)
	{
		IconMaterial->Textures.Reset();
		IconMaterial->Textures.Add(IconTexture);
		Icon->SetMaterial(0, IconMaterial);
	}

	Icon->SetupAttachment(GetRootComponent());
	Icon->SetRelativeScale3D({1.0f,0.5f,0.5f });

	DirectLight = CreateDefaultSubobject<UDirectionalLightComponent>("UDirectionalLightComponent");
	SetLightComponent(DirectLight);
	DirectLight->SetupAttachment(GetRootComponent());

	DirectionArrow = CreateDefaultSubobject<UStaticMeshComponent>("UStaticMeshComponent");
	DirectionArrow->SetStaticMesh(UAssetManager::GetAssetByPath<UStaticMesh>("Arrow"));
	DirectionArrow->SetupAttachment(DirectLight, EAttachmentRule::SnapToTarget);
	DirectionArrow->SetRelativeScale3D({ 0.2f,0.2f,0.5f });
}

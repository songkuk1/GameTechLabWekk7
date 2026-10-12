#include "EnginePCH.h"
#include "PointLightActor.h"
#include "Asset/AssetManager.h"


APointLightActor::APointLightActor()
{
	Icon = BillboardComponent;

	UMaterial* BaseMaterial = UAssetManager::GetAssetByPath<UMaterial>("SubUVMaterial");
	UTexture2D* onTexture = UAssetManager::Get().LoadTexture(UnHighLightedPointLightIconPath);
	UTexture2D* offTexture = UAssetManager::Get().LoadTexture(UnHighLightedPointLightIconPath);
	UMaterial* IconMaterial = UMaterial::CreateInstance(BaseMaterial);

	if (IconMaterial && onTexture)
	{
		IconMaterial->Textures.Reset();
		IconMaterial->Textures.Add(onTexture);
		Icon->SetMaterial(0, IconMaterial);
	}

	if (offMaterial && onTexture)
	{
		offMaterial->Textures.Reset();
		offMaterial->Textures.Add(onTexture);
		Icon->SetMaterial(0, offMaterial);
	}

	Icon->SetupAttachment(GetRootComponent());
	Icon->SetRelativeScale3D({ 1.0f,0.5f,0.5f });

	PointLight = CreateDefaultSubobject<UPointLightComponent>("UPointLightComponent");
	SetLightComponent(PointLight);
	PointLight->SetupAttachment(GetRootComponent());
}

void APointLightActor::OnSelected()
{
	Icon->SetMaterial(0, onMaterial);
}

void APointLightActor::OnDeselected()
{
	Icon->SetMaterial(0, offMaterial);
}

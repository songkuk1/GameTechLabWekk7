#include "EnginePCH.h"
#include "PointLightActor.h"
#include "Asset/AssetManager.h"

APointLightActor::APointLightActor()
{
	// 아이콘용 메시/머티리얼. Plane은 FVertex 포맷이라 DefaultShader로 그대로 그려진다.
	// 라이트 아이콘 텍스처가 준비되면 Property 창에서 머티리얼 슬롯에 끼우면 된다.
	UMaterial* BaseMaterial = UAssetManager::GetAssetByPath<UMaterial>("SubUVMaterial");
	UTexture2D* IconTexture = UAssetManager::Get().LoadTexture(UnHighLightedPointLightIconPath);
	UMaterial* IconMaterial = UMaterial::CreateInstance(BaseMaterial);

	SetLightComponent(CreateDefaultSubobject<UPointLightComponent>("UPointLightComponent"));
	LightComponent->SetupAttachment(GetRootComponent());

	BillboardComponent->SetupAttachment(LightComponent, EAttachmentRule::SnapToTarget);
	BillboardComponent->SetHiddenInDetails(true);

	if (IconMaterial && IconTexture)
	{
		IconMaterial->Textures.Reset();
		IconMaterial->Textures.Add(IconTexture);
		BillboardComponent->SetMaterial(0, IconMaterial);
	}

	BillboardComponent->SetRelativeScale3D({ 1.0f,0.5f,0.5f });
}

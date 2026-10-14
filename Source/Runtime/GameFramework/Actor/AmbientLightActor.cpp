#include "EnginePCH.h"
#include "AmbientLightActor.h"
#include "Component/AmbientLightComponent.h"
#include "Component/BillboardComponent.h"


AAmbientLightActor::AAmbientLightActor()
{
	SetLightComponent(CreateDefaultSubobject<UPointLightComponent>("UPointLightComponent"));
	SetRootComponent(LightComponent);
	//UMaterial* BaseMaterial = UAssetManager::GetAssetByPath<UMaterial>("BillboardIconMaterial");
	//UTexture2D* IconTexture = UAssetManager::Get().LoadTexture(PointLightOffIconPath);
	//UMaterial* IconMaterial = UMaterial::CreateInstance(BaseMaterial);

	//if (IconMaterial && IconTexture)
	//{
	//	IconMaterial->Textures.Reset();
	//	IconMaterial->Textures.Add(IconTexture);
	//	BillboardComponent->SetMaterial(0, IconMaterial);
	//}

	BillboardComponent->SetupAttachment(GetRootComponent());
	//빌보드 컴포넌트가 detail 패널에 안보이게함
	BillboardComponent->SetHiddenInDetails(true);


}

#pragma once

#include "PrimitiveSceneProxy.h"
#include "LightSceneProxy.h"
#include "Component/BillboardComponent.h"
#include "Component/PrimitiveComponent.h"
#include "Component/FireBallComponent.h"
#include "Component/DirectionalLightComponent.h"
#include "Math/Frustum.h"
#include "Math/BVH.h"
#include "Render/FogInfo.h"
#include "Render/Renderer.h"

struct FExponentialHeightFogSceneInfo
{
	uint32 Id;
	FFogInfo FogInfo;
};


class FScene
{
public:
	void AddPrimitive(UPrimitiveComponent* Component);
	void RemovePrimitive(UPrimitiveComponent* Component);
	// 모든 프록시를 한 번에 지운다. 액터를 통째로 지우기 전(ClearWorld)에 불러야 지워진 컴포넌트를 가리키는 프록시가 남지 않는다.
	void RemoveAllPrimitives();

	void AddFogInfo(uint32 Id, const FFogInfo& FogInfo);
	void RemoveFogInfo(uint32 Id);
	void RemoveAllFogInfo();
	void UpdateFogInfo(uint32 Id, const FFogInfo& FogInfo);
	void UpdateAllTransforms();

	void BuildBVH();

	void MarkDirty(FPrimitiveSceneProxy* Proxy);
	void MarkRenderStateDirty(FPrimitiveSceneProxy* Proxy);

	// Todo : LightComponent로 묶게 되면서 상위 클래스에서 해당 함수를 통일하게 할 것인가?

	void UpdateFireBallLight(FRenderer* Renderer);
	void RegisterFireBall(UFireBallComponent* FireBall);
	void UnregisterFireBall(UFireBallComponent* FireBall);
	
	void UpdateDirectionalLight(FRenderer* Renderer);
	void RegisterDirectLight(UDirectionalLightComponent* DLightComp);
	void UnregisterDirectLight(UDirectionalLightComponent* DLightComp);

	void AddLight(ULightComponent* Component);
	void RemoveLight(ULightComponent* Component);
	// 모든 프록시를 한 번에 지운다. 액터를 통째로 지우기 전(ClearWorld)에 불러야 지워진 컴포넌트를 가리키는 프록시가 남지 않는다.
	void RemoveAllLights();
	void MarkLightDirty(FLightSceneProxy* Proxy);

	TArray<FPrimitiveSceneProxy*> Proxies;
	TArray<FPrimitiveSceneProxy*> DirtyProxies;
	TArray<FPrimitiveSceneProxy*> RenderStateDirtyProxies;
	TArray<FAABB> PrimitiveBounds;
	TArray<uint8> PrimitiveFlags;
	
	TArray<FExponentialHeightFogSceneInfo> FogInfos;

	// 컬링 결과로 프록시를 바로 내보내 컴포넌트를 역참조하지 않는다. 경계는 Build/Refit 때만 계산한다.
	TBVH<FPrimitiveSceneProxy*> BVH{
		[](const FPrimitiveSceneProxy* Proxy) -> FBox
		{
			const FAABB& B = Proxy->GetBounds();
			return FBox{ B.Center - B.Extent, B.Center + B.Extent };
		}
	};
	bool bElementListChanged = false;
	

	TArray<UFireBallComponent*> FireBallComponents;
	TArray<UDirectionalLightComponent*> DirectionalLightComponents;

	TArray<FLightSceneProxy*> LightProxies;
	//TODO: Dirty를 두는 방식도 Thread Safe하지않음 여긴 게임,렌더 스레드가 둘다 접근할 수 있는 공간
	TArray<FLightSceneProxy*> DirtyLightProxies;


};
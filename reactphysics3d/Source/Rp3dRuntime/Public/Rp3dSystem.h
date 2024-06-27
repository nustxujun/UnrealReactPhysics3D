#pragma once 

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Rp3dCommon.h"
#include "Rp3dWorldSettings.h"
#include "Rp3dSystem.generated.h"

class URp3dWorld;



UCLASS()
class RP3DRUNTIME_API URp3dSystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	static URp3dSystem& Get();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "reactphysics3d")
	URp3dWorld* CreateWorld(UWorld* World, const FRp3dWorldSettings& Settings);

	URp3dWorld* GetRp3dWorld(UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "reactphysics3d")
	URp3dWorld* GetRp3dWorld(UObject* Obj);

	reactphysics3d::PhysicsCommon& GetRp3dPhysicsCommon();


private:
	TSharedPtr<reactphysics3d::PhysicsCommon> PhysicsCommon;

	UPROPERTY()
	FRp3dWorldSettings WorldSettings;

	UPROPERTY()
	TArray<URp3dWorld*> Worlds;
	
	TMap<UWorld*, URp3dWorld*> WorldMap;
};
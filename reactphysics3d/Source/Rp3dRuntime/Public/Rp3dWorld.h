#pragma once 
#include "CoreMinimal.h"
#include "Rp3dCommon.h"
#include "Rp3dWorldSettings.h"
#include "Rp3dWorld.generated.h"

class UWorld;

UCLASS()
class RP3DRUNTIME_API URp3dWorld: public UObject
{
	GENERATED_BODY()
	friend class URp3dRigidBody;
public:

	static URp3dWorld* Get(UWorld* World);

	reactphysics3d::PhysicsWorld& GetRp3dPhysicsWorld();

	void Initialize(const FRp3dWorldSettings& Settings, UWorld* InWorld);

	UFUNCTION(BlueprintCallable, Category = "reactphysics3d")
	void EnableDebugDraw(bool Val);

	void DrawDebug();
	void UpdatePhysics(reactphysics3d::decimal DeltaTime);

	virtual void BeginDestroy();
private:
	void AddRigidBody(URp3dRigidBody* RigidBody);
private:
	TSharedPtr<reactphysics3d::PhysicsWorld> PhysicsWorld;
	TWeakObjectPtr<UWorld> World;
	TArray<TWeakObjectPtr<URp3dRigidBody>> RigidBodies;

	FDelegateHandle TickHandle;
	reactphysics3d::decimal TotalTime = 0;
};


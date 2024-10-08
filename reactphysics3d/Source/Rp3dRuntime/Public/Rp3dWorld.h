#pragma once 
#include "CoreMinimal.h"
#include "Rp3dCommon.h"
#include "Rp3dWorldSettings.h"
#include "Rp3dWorld.generated.h"



class RP3DRUNTIME_API FRp3dPhysicsWorld
{
	friend class FRp3dRigidBody;
public:
	FRp3dPhysicsWorld(const FRp3dWorldSettings& Settings);
	TSharedPtr<FRp3dRigidBody> CreateRigidBody();
	void SetIsDebugRenderingEnabled(bool Val);
	void Update(reactphysics3d::decimal DeltaTime);

	void DrawDebug(UWorld* World);

private:
	TSharedPtr<reactphysics3d::PhysicsWorld> PhysicsWorld;
};

class UWorld;

UCLASS()
class RP3DRUNTIME_API URp3dWorld: public UObject
{
	GENERATED_UCLASS_BODY()
	friend class URp3dRigidBody;
public:

	static URp3dWorld* Get(UWorld* World);
	virtual ~URp3dWorld();

	TSharedPtr<FRp3dPhysicsWorld> GetRp3dPhysicsWorld();

	void Initialize(const FRp3dWorldSettings& Settings);

	UFUNCTION(BlueprintCallable, Category = "reactphysics3d")
	void EnableDebugDraw(bool Val);

	void DrawDebug();
	void UpdatePhysics(reactphysics3d::decimal DeltaTime);
	void Step(reactphysics3d::decimal Interval);

	virtual void BeginDestroy();
private:
	void AddRigidBody(URp3dRigidBody* RigidBody);
	void RemoveRigidBody(URp3dRigidBody* RigidBody);
private:
	TSharedPtr<FRp3dPhysicsWorld> PhysicsWorld;
	UPROPERTY()
	TArray<URp3dRigidBody*> RigidBodies;
#if ENGINE_MAJOR_VERSION >= 5
	FTSTicker::
#endif
	FDelegateHandle TickHandle;
	reactphysics3d::decimal TotalTime = 0;
};

//UCLASS()
//class RP3DRUNTIME_API URp3dWorldSubsystem : public UWorldSubsystem
//{
//	GENERATED_BODY()
//public:
//	UFUNCTION(BlueprintCallable, Category = "reactphysics3d")
//	TSharedPtr<FRp3dPhysicsWorld> GetRp3dWorld();
//
//	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
//	virtual void Deinitialize() override;
//	bool ShouldCreateSubsystem(UObject* Outer) const override;
//private:
//	void CreatePhysicsWorld();
//private:
//	UPROPERTY()
//	URp3dWorld* World;
//
//};
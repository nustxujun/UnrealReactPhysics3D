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
	friend class URp3dWorld;
public:
	static URp3dSystem& Get();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;


	reactphysics3d::PhysicsCommon& GetRp3dPhysicsCommon();

private:
	TSharedPtr<reactphysics3d::PhysicsCommon> PhysicsCommon;

};
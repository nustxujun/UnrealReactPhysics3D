#include "Rp3dRigidBodyComponent.h"
#include "Rp3dRigidBody.h"
#include "Rp3dUtils.h"
#include "Rp3dWorld.h"

URp3dRigidBodyComponent::URp3dRigidBodyComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseEditorCompositing = true;

	PrimaryComponentTick.bCanEverTick = true;
}


void URp3dRigidBodyComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OnCreateRp3dState();
}

void URp3dRigidBodyComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnDestroyRp3dState();
}

void URp3dRigidBodyComponent::OnCreateRp3dState(TSharedPtr<FRp3dPhysicsWorld> RWorld)
{
	
	if (!RWorld)
	{
		auto World = URp3dWorld::Get(GetWorld());
		if (World)
			RWorld = World->GetRp3dPhysicsWorld();
	}
	if (!RWorld)
	{
		SetComponentTickEnabled(false);
		return;
	}
	RigidBody = MakeShared<FRp3dRigidBody>(RWorld.Get());

	auto Trans = GetComponentTransform();
	Scale3D = GetComponentTransform().GetScale3D();
	RigidBody->SetTransform(UE_TO_RP3D(Trans));

	EBodyType Type = EBodyType::STATIC;
	Mobility = GetOwner()->GetRootComponent()->Mobility;
	if (Mobility == EComponentMobility::Stationary)
	{
		Type = EBodyType::KINEMATIC;
	}
	else if (Mobility == EComponentMobility::Movable)
	{
		Type = EBodyType::DYNAMIC;
	}

	SetBodyType(Type);

#if WITH_EDITOR
	RigidBody->SetIsDebugEnabled(bEnableDebugDraw);
#endif

}

void URp3dRigidBodyComponent::SetBodyType(EBodyType Type)
{
	using namespace reactphysics3d;

	SetComponentTickEnabled(bTickable && Type != EBodyType::STATIC);

	RigidBody->SetBodyType(Type);
}

void URp3dRigidBodyComponent::OnDestroyRp3dState()
{
	//RigidBody.Reset();
}

void URp3dRigidBodyComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if ( RigidBody->IsActive())
	{
		auto Trans = RP3D_TO_UE(RigidBody->GetTransform());
		Trans.SetScale3D(Scale3D);
		SetWorldTransform(Trans);
	}
}

#if WITH_EDITOR
void URp3dRigidBodyComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (RigidBody )
	{
		RigidBody->SetIsDebugEnabled(bEnableDebugDraw);
	}
}
#endif

void URp3dRigidBodyComponent::AddVelocity(const FVector& AddVel)
{

}

void URp3dRigidBodyComponent::SetVelocity(const FVector& Vel)
{
}


void URp3dRigidBodyComponent::DisableTick()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetComponentTickEnabled(false);
}

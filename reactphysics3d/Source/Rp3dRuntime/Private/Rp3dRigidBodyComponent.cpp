#include "Rp3dRigidBodyComponent.h"
#include "Rp3dRigidBody.h"
#include "Rp3dUtils.h"

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

void URp3dRigidBodyComponent::OnCreateRp3dState()
{


	auto Trans = GetComponentTransform();
	Scale3D = GetComponentTransform().GetScale3D();
	RigidBody = NewObject<URp3dRigidBody>(this);
	RigidBody->SetTransform(UE_TO_RP3D(Trans));

	EBodyType Type = EBodyType::STATIC;
	if (Mobility == EComponentMobility::Stationary)
	{
		Type = EBodyType::KINEMATIC;
	}
	else if (Mobility == EComponentMobility::Movable)
	{
		Type = EBodyType::DYNAMIC;
	}

	SetBodyType(Type);

#if WITH_EDITORONLY_DATA
	RigidBody->SetIsDebugEnabled(bEnableDebugDraw);
#endif

}

void URp3dRigidBodyComponent::SetBodyType(EBodyType Type)
{
	using namespace reactphysics3d;
	if (Type == EBodyType::STATIC)
	{
		SetComponentTickEnabled(false);
	}

	RigidBody->SetBodyType(Type);
}

void URp3dRigidBodyComponent::OnDestroyRp3dState()
{
	RigidBody->RemoveFromWorld();
	RigidBody = nullptr;
}

void URp3dRigidBodyComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (RigidBody->IsActive())
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
	if (RigidBody)
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

void URp3dRigidBodyComponent::SetAngularLockAxisFactor(const FVector& Factor)
{
	RigidBody->SetAngularLockAxisFactor({Factor.X,Factor.Y,Factor.Z});
}
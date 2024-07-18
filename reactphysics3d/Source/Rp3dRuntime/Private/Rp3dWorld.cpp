#include "Rp3dWorld.h"
#include "Rp3dUtils.h"
#include "Rp3dSystem.h"
#include "Rp3dRigidBody.h"
#include "DrawDebugHelpers.h"

#include "reactphysics3d/engine/PhysicsCommon.h"
#include "reactphysics3d/engine/PhysicsWorld.h"


URp3dWorld* URp3dWorld::Get(UWorld* World)
{
    auto Sys = World->GetSubsystem<URp3dWorldSubsystem>();
    URp3dWorld* RWorld = nullptr;
    if (Sys)
        RWorld = Sys->GetRp3dWorld();
    return RWorld;
}


URp3dWorld::URp3dWorld(const FObjectInitializer& ObjectInitializer):
    Super(ObjectInitializer)
{
}

void URp3dWorld::Initialize(const FRp3dWorldSettings& WorldSettings)
{
	using namespace reactphysics3d;
	PhysicsWorld::WorldSettings Settings;

    Settings.worldName = UE_TO_RP3D( WorldSettings.WorldName);
    Settings.gravity = UE_TO_RP3D(WorldSettings.Gravity);
    Settings.persistentContactDistanceThreshold = UE_TO_RP3D(WorldSettings.PersistentContactDistanceThreshold);
    Settings.defaultFrictionCoefficient = decimal(WorldSettings.DefaultFrictionCoefficient);
    Settings.defaultBounciness = decimal(WorldSettings.DefaultBounciness);
    Settings.restitutionVelocityThreshold = UE_TO_RP3D(WorldSettings.RestitutionVelocityThreshold);
    Settings.isSleepingEnabled = WorldSettings.bIsSleepingEnabled;
    Settings.defaultVelocitySolverNbIterations = WorldSettings.DefaultVelocitySolverNbIterations;
    Settings.defaultPositionSolverNbIterations = WorldSettings.DefaultPositionSolverNbIterations;
    Settings.defaultTimeBeforeSleep = WorldSettings.DefaultTimeBeforeSleep;
    Settings.defaultSleepLinearVelocity = UE_TO_RP3D(WorldSettings.DefaultSleepLinearVelocity);
    Settings.defaultSleepAngularVelocity = decimal(WorldSettings.DefaultSleepAngularVelocity);
    Settings.cosAngleSimilarContactManifold = decimal(WorldSettings.CosAngleSimilarContactManifold);

    auto& PhysicsCommon = URp3dSystem::Get().GetRp3dPhysicsCommon();

    PhysicsWorld = TSharedPtr<reactphysics3d::PhysicsWorld>(
        PhysicsCommon.createPhysicsWorld(Settings), 
        [&](auto World){
            PhysicsCommon.destroyPhysicsWorld(World);
        });


    {
        TickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this, bUpdate = WorldSettings.bAutoUpdate](auto DeltaTime){
            DrawDebug();
            if (bUpdate)
                UpdatePhysics(DeltaTime);
            return true;
        }),0);
    }


    PhysicsWorld->getDebugRenderer().setIsDebugItemDisplayed(DebugRenderer::DebugItem::COLLISION_SHAPE, true);

}

void URp3dWorld::EnableDebugDraw(bool Val)
{
    PhysicsWorld->setIsDebugRenderingEnabled(Val);
}


reactphysics3d::PhysicsWorld& URp3dWorld::GetRp3dPhysicsWorld()
{
    return *PhysicsWorld;
}

void URp3dWorld::UpdatePhysics(reactphysics3d::decimal DeltaTime)
{
    TotalTime += DeltaTime;
    constexpr float Interval = 1.0f / 60.0f;
    while (TotalTime > Interval)
    {
        PhysicsWorld->update(Interval);
        TotalTime -= Interval;
    }

}

void URp3dWorld::DrawDebug()
{
    
    auto World = GetWorld();
    if (!World)
        return;

    using namespace reactphysics3d;
    DebugRenderer& Renderer = PhysicsWorld->getDebugRenderer();


    for (auto& Line : Renderer.getLines())
    {
        ::DrawDebugLine(World, RP3D_TO_UE(Line.point1), RP3D_TO_UE(Line.point2), FColor(Line.color1));
    }

    for (auto& Face : Renderer.getTriangles())
    {
        ::DrawDebugLine(World, RP3D_TO_UE(Face.point1), RP3D_TO_UE(Face.point2), FColor(Face.color1));
        ::DrawDebugLine(World, RP3D_TO_UE(Face.point1), RP3D_TO_UE(Face.point3), FColor(Face.color1));
        ::DrawDebugLine(World, RP3D_TO_UE(Face.point2), RP3D_TO_UE(Face.point3), FColor(Face.color1));

    }
    
}
void URp3dWorld::BeginDestroy()
{
    Super::BeginDestroy();

    for (auto& Body : RigidBodies)
    {
        Body->RemoveFromWorld();
    }
    RigidBodies.Reset();

    if (TickHandle.IsValid())
        FTicker::GetCoreTicker().RemoveTicker(TickHandle);

    PhysicsWorld.Reset();

}

void URp3dWorld::AddRigidBody(URp3dRigidBody* RigidBody)
{
    RigidBodies.Add(RigidBody);
}

void URp3dWorld::RemoveRigidBody(URp3dRigidBody* RigidBody)
{
    RigidBodies.RemoveSingleSwap(RigidBody);
}


URp3dWorld* URp3dWorldSubsystem::GetRp3dWorld()
{
    return World;
}

bool URp3dWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    return false;
    //auto Type = Cast<UWorld>(Outer)->WorldType;
    //return Type == EWorldType::Game || Type == EWorldType::PIE;
}

void URp3dWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    //Collection.InitializeDependency(URp3dSystem::StaticClass());

    World = NewObject<URp3dWorld>(this);
    FRp3dWorldSettings Settings;
    Settings.bAutoUpdate = false;
    World->Initialize(Settings);
}

void URp3dWorldSubsystem::Deinitialize()
{
    World = nullptr;
}
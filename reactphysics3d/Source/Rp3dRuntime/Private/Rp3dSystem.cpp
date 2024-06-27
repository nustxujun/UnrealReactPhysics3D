#include "Rp3dSystem.h"
#include "Rp3dWorld.h"

#include "reactphysics3d/engine/PhysicsCommon.h"
#include "reactphysics3d/engine/PhysicsWorld.h"
#include "reactphysics3d/utils/Logger.h"

class FRp3dLogger : public reactphysics3d::Logger
{
	virtual void log(Level level, const std::string& physicsWorldName, Category category, const std::string& message, const char* filename, int lineNumber)
	{
		using namespace reactphysics3d;

		FString Content = FString::Printf(TEXT("[%s]%s. ( %s:%d )"), ANSI_TO_TCHAR(physicsWorldName.c_str()), ANSI_TO_TCHAR(message.c_str()), ANSI_TO_TCHAR(filename), lineNumber);
		switch (level)
		{
		case Level::Error: UE_LOG(LogPhysics, Error, TEXT("%s"), *Content);break;
		case Level::Warning: UE_LOG(LogPhysics, Warning, TEXT("%s"), *Content); break;
		case Level::Information: UE_LOG(LogPhysics, Log, TEXT("%s"), *Content); break;

		}
		
	}
};


URp3dSystem& URp3dSystem::Get()
{
	return *GEngine->GetEngineSubsystem<URp3dSystem>();
}



void URp3dSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	PhysicsCommon = MakeShared<reactphysics3d::PhysicsCommon>();
	static FRp3dLogger Rp3dLogger;
	PhysicsCommon->setLogger(&Rp3dLogger);
}

void URp3dSystem::Deinitialize()
{
	PhysicsCommon.Reset();
}

URp3dWorld* URp3dSystem::CreateWorld(UWorld* World,const FRp3dWorldSettings & Settings)
{
	auto RWorld = NewObject<URp3dWorld>(this);
	Worlds.Add(RWorld);
	RWorld->Initialize(Settings, World);
	WorldMap.Add(World, RWorld);

	return RWorld;
}

URp3dWorld* URp3dSystem::GetRp3dWorld(UWorld* World)
{
	return WorldMap.FindRef(World);
}

URp3dWorld* URp3dSystem::GetRp3dWorld(UObject* Obj)
{
	if (!Obj)
		return nullptr;
	if (Obj->IsA<UWorld>())
		return GetRp3dWorld(Cast<UWorld>(Obj));
	else
		return GetRp3dWorld(Obj->GetWorld());
}	


reactphysics3d::PhysicsCommon& URp3dSystem::GetRp3dPhysicsCommon()
{
	return *PhysicsCommon;
}

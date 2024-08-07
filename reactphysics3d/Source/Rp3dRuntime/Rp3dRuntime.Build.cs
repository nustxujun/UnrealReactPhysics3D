// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class Rp3dRuntime : ModuleRules
{
	public Rp3dRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        ShadowVariableWarningLevel = WarningLevel.Off;
#if UE_5_2_OR_LATER
		IWYUSupport = IWYUSupport.None;
#else
        bEnforceIWYU = false;
#endif

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory,"reactphysics3d/include")
            });


        PublicDependencyModuleNames.AddRange(
			new string[]	
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine"
			}
            );
		
		

	}
}

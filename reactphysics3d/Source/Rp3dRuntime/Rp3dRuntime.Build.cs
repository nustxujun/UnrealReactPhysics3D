// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class Rp3dRuntime : ModuleRules
{
	public Rp3dRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        ShadowVariableWarningLevel = WarningLevel.Off;
        bEnforceIWYU = false;

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

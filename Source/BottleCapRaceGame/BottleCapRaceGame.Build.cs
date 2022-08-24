// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BottleCapRaceGame : ModuleRules
{
	public BottleCapRaceGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HTTP"
															, "Json"
															, "JsonUtilities"
															, "UMG"
															, "OnlineSubsystem"
															, "OnlineSubsystemUtils"});

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	}
}

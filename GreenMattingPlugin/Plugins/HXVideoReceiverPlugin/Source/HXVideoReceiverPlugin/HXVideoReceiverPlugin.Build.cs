// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class HXVideoReceiverPlugin : ModuleRules
{
	public HXVideoReceiverPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"HXVideoReceiverPlugin/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"HXVideoReceiverPlugin/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
                "CoreUObject",
                "Engine",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        LoadHXStreamNetClientLib(Target);

    }




    private string ThirdPartyPath
    {
        get
        {
            return Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty/"));
        }
    }


    public void LoadHXStreamNetClientLib(ReadOnlyTargetRules Target)
    {
        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {

            string LibrariesPath = Path.Combine(ThirdPartyPath, "HXVideoReceiverPluginLibrary", "lib");

            //test your path
            System.Console.WriteLine("... LibrariesPath -> " + LibrariesPath);

            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "HXVideoReceiverPluginLibrary", "include"));

            // System.Console.WriteLine(Path.Combine(ThirdPartyPath, "HXVideoStreamClientLibrary", "include"));

            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "HXStreamNetClient.lib"));
            PrivateIncludePaths.Add(Path.Combine(ThirdPartyPath, "HXVideoReceiverPluginLibrary", "include"));
        }

    }

}

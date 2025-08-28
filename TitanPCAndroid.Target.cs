using UnrealBuildTool;
using System.Collections.Generic;

public class TitanPCAndroidTarget : TargetRules
{
    public TitanPCAndroidTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
        ExtraModuleNames.AddRange(new string[] { "TitanPC" });
        
        // Android-specific optimizations
        bUseUnityBuild = true;
        bUsePCHFiles = true;
        bUseSharedPCHs = true;
        
        // Performance settings for mobile
        WindowsPlatform.PCHMemoryAllocationFactor = 2000;
        AndroidPlatform.bBuildForArm64 = true;
        AndroidPlatform.bBuildForX8664 = false; // Focus on ARM64 for modern devices
    }
}
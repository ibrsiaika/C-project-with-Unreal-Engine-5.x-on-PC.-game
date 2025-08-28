
using UnrealBuildTool;

public class TitanPC : ModuleRules
{
    public TitanPC(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Slate", "SlateCore",
            "GameplayTags", "GameplayTasks", "GameplayAbilities", "AIModule", "NavigationSystem", "Niagara",
            "OnlineSubsystem", "OnlineSubsystemUtils", "CommonUI", "AudioMixer", "MovieScene", "LevelSequence",
            "NetCore", "Networking", "Sockets"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Projects"
        });
    }
}

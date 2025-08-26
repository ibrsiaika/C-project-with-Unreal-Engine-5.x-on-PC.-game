
using UnrealBuildTool;

public class TitanPC : ModuleRules
{
    public TitanPC(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Slate", "SlateCore",
            "GameplayTags", "GameplayTasks", "AIModule", "NavigationSystem", "Niagara"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Projects"
        });
    }
}

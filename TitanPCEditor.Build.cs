
using UnrealBuildTool;

public class TitanPCEditor : ModuleRules
{
    public TitanPCEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "UnrealEd", "Slate", "SlateCore", "TitanPC"
        });
    }
}

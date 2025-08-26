
using UnrealBuildTool;
using System.Collections.Generic;

public class TitanPCTarget : TargetRules
{
    public TitanPCTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
        ExtraModuleNames.AddRange(new string[] { "TitanPC" });
    }
}

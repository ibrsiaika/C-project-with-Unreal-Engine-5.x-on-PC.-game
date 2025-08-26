
using UnrealBuildTool;
using System.Collections.Generic;

public class TitanPCEditorTarget : TargetRules
{
    public TitanPCEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
        ExtraModuleNames.AddRange(new string[] { "TitanPC", "TitanPCEditor" });
    }
}

// Copyright Derek Bardo

using UnrealBuildTool;
using System.Collections.Generic;

public class SlashEditorTarget : TargetRules
{
	public SlashEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "Slash" } );
	}
}

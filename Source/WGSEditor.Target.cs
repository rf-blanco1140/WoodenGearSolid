using UnrealBuildTool;
using System.Collections.Generic;

public class WGSEditorTarget : TargetRules
{
	public WGSEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add( "WGS" );
		//ExtraModuleNames.Add( "WGSEditor" );
	}
}
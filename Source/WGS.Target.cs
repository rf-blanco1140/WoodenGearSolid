using UnrealBuildTool;
using System.Collections.Generic;

public class WGSTarget : TargetRules
{
	public WGSTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add( "WGS" );
	}
}
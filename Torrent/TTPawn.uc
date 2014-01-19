class TTPawn extends UTPawn;

var bool bBlinded;
var bool bShieldDeployed;
var float timeBlind;
var float blindDecay;
var float TimeLastTakeDamage;
var float TimeToHeal;
var float HealTimer;
var int HPPS;
var int HackerCharge;
var AudioComponent RegenSound;
var TTShield shield;
var AudioComponent ShieldSound;

enum RegenSFX_State
{
	RegenSFX_Off,
	RegenSFX_Begin,
	RegenSFX_Playing,
	RegenSFX_Stop
};

enum ShieldSFX_State
{
	ShieldSFX_Off,
	ShieldSFX_Begin,
	ShieldSFX_Playing,
	ShieldSFX_Stop
};

var ShieldSFX_State ShieldState;

var RegenSFX_State Regen;

var(TTPawn) float SprintSpeed;
var(TTPawn) float WalkSpeed;

var Material CloakMaterial;
var MaterialInterface TakeDamageMaterial;
var float HitEffectTime;
var repnotify bool bHitMaterialOn;

simulated function SetCharacterClassFromInfo( class<UTFamilyInfo> info )
{
	local MaterialInterface TeamMaterialHead, TeamMaterialBody;

	super.SetCharacterClassFromInfo( info );

	//Apply the team skins if necessary
	if (WorldInfo.NetMode != NM_DedicatedServer)
	{
		class'TTFamilyInfo'.static.GetTeamMaterials(GetTeamNum(), TeamMaterialHead, TeamMaterialBody);
	}

	// 3P Mesh and materials
	SetCharacterMeshInfo(class'TTFamilyInfo'.default.CharacterMesh, TeamMaterialHead, TeamMaterialBody);

	OverlayMesh.SetSkeletalMesh( class'TTFamilyInfo'.default.CharacterMesh );
	OverlayMesh.AnimSets = class'TTFamilyInfo'.default.AnimSets;
	
	Mesh.SetSkeletalMesh( class'TTFamilyInfo'.default.CharacterMesh );
	Mesh.AnimSets = class'TTFamilyInfo'.default.AnimSets;
}

simulated function PostBeginPlay()
{
	super.PostBeginPlay();
	GroundSpeed = WalkSpeed;

	shield = new(self) class'TTShield';

	if( shield != none )
	{
		AttachComponent( shield );
		shield.SetHidden( true );
	}
}

function Blind( float time )
{
	bBlinded = true;
	timeBlind = 255;
	blindDecay = time / 255.0;
	SetTimer( time );
	if( ROLE==ROLE_Authority )
	{
		ServerBlind( time );
	}
}

reliable server function ServerBlind( float time )
{
	bBlinded = true;
	timeBlind = 255;
	blindDecay = time / 255.0;
	SetTimer( time, false, 'ServerNotBlind' );
}

reliable server function ServerNotBlind()
{
	timeBlind = 255;
	bBlinded = false;
}

simulated function Timer()
{	
	ServerNotBlind();
}

// Logic tick
simulated function Tick( float DeltaTime )
{
	local float DeltaDamageTimer;
	
	if( bBlinded )
	{
		timeBlind -= 0.5;
	}

	if( TTPlayerReplicationInfo( PlayerReplicationInfo ).bShieldDeployed )
	{
		if (ShieldState != ShieldSFX_Playing)
		{
			shield.SetHidden( false );
			ShieldSound = new(self) class'AudioComponent';
			AttachComponent(ShieldSound);		
			ShieldSound.SoundCue = SoundCue'TT_Sounds.TT_Sounds.TT-sfx-cue-shield';
			ShieldSound.Play();
			ShieldState = ShieldSFX_Playing;
		}
		
	}
	else
	{
		shield.SetHidden( true );
		ShieldSound.Stop();
		ShieldState = ShieldSFX_Off;
		
	}
	if (ShieldState == ShieldSFX_Playing)
	{
		
	}
	

	// Regen HP
	if ( Health < HealthMax )
	{
		DeltaDamageTimer = WorldInfo.TimeSeconds - TimeLastTakeDamage;
		HealTimer -= DeltaTime;
		

		if ( DeltaDamageTimer >= TimeToHeal && HealTimer <= 0 )
		{
			if ( Regen == RegenSFX_Off )
				Regen = RegenSFX_Begin;
			++Health;
			Health = Clamp( Health, 0, HealthMax );
			HealTimer = HPPS * DeltaTime;
		}		
	}
	else if ( Health == HealthMax )
	{
		if ( Regen == RegenSFX_Playing )
			Regen = RegenSFX_Stop;
	}

	/*if ( Regen == RegenSFX_Begin )
	{
		if ( PlayerController( Controller ) != none )
		{
			Regen = RegenSFX_Playing;
			//RegenSound = new(self) class'AudioComponent';
			//AttachComponent(RegenSound);
			//RegenSound.SoundCue = SoundCue'TT_Sounds.TT_Player.TT-sfx-cue_regen';
			//RegenSound.Play();
			PlayerController( Controller ).ClientPlaySound( SoundCue'TT_Sounds.TT_Player.TT-sfx-cue_regen' );
		}
		//PlayerController( Controller ).Client;
	}
	else if ( Regen == RegenSFX_Stop )
	{
		//Regen = RegenSFX_Off;
		if ( PlayerController( Controller ) != none )
		{
			//RegenSound.Stop();
			//RegenSound = none;
			TTPlayerController( Controller ).ClientStopSound();
		}
	}*/

	if ( Regen == RegenSFX_Begin )
	{
		Regen = RegenSFX_Playing;
		RegenSound = new(self) class'AudioComponent';
		AttachComponent(RegenSound);
		RegenSound.SoundCue = SoundCue'TT_Sounds.TT_Player.TT-sfx-cue_regen';
		RegenSound.Play();
	}
	else if ( Regen == RegenSFX_Stop )
	{
		Regen = RegenSFX_Off;
		if ( RegenSound != none )
		{
			RegenSound.Stop();
			RegenSound = none;
		}
	}

	super.Tick( DeltaTime );
}

// Collision with another Pawn
function Bump( Actor Other, PrimitiveComponent OtherComp, Vector HitNormal )
{
	super.Bump( Other, OtherComp, HitNormal );
}

/**
 * We override TakeDamage in order to track the last time we took damage.
 */
event TakeDamage(int Damage, Controller EventInstigator, vector HitLocation, vector Momentum, class<DamageType> DamageType, optional TraceHitInfo HitInfo, optional Actor DamageCauser)
{
	if( !bShieldDeployed )
	{
		TimeLastTakeDamage = WorldInfo.TimeSeconds;

		//SetHitMaterial();
		bHitMaterialOn = true;
		SetTimer( HitEffectTime, false, 'HideTakeDamageMesh' );

		Super.TakeDamage(Damage, EventInstigator, HitLocation, Momentum, DamageType, HitInfo, DamageCauser);
	}

	else
	{
		if( TTWeap_MineLauncher( Weapon ).shieldPercentDmgReduction < 1 )
		{
			TimeLastTakeDamage = WorldInfo.TimeSeconds;

			//SetHitMaterial();
			bHitMaterialOn = true;
			SetTimer( HitEffectTime, false, 'HideTakeDamageMesh' );
		}

		//Weapon.AddAmmo( -(TTWeap_MineLauncher( Weapon ).shieldDrainAmount ) );
		//if( !Weapon.HasAnyAmmo() )
		//{
		//	Weapon.WeaponEmpty();
		//}

		Super.TakeDamage( ( 1 - TTWeap_MineLauncher( Weapon ).shieldPercentDmgReduction ) * Damage, EventInstigator, HitLocation, Momentum, DamageType, HitInfo, DamageCauser);
	}
}

simulated event ReplicatedEvent(name VarName)
{
    if( VarName == 'bHitMaterialOn' )
    {
		if (bHitMaterialOn == true)
		{
			SetHitMaterial(true);
			`log("bHitMaterialOn = true");
		}
		else if (bHitMaterialOn == false)
		{
			SetHitMaterial(false);
			`log("bHitMaterialOn = false");
		}
    }
    else
        super.ReplicatedEvent(VarName);
}

simulated function SetHitMaterial(bool on)
{
	if(Role < ROLE_Authority)
		ServerChangeHitMaterial();
	else
	{
		if ( on )
		{
			SetOverlayMaterial( TakeDamageMaterial );
			SetOverlayVisibility( true );
			OverlayMesh.SetHidden( false );
		}
		else
		{
			SetOverlayMaterial( None );
			SetOverlayVisibility( false );
			OverlayMesh.SetHidden( true );
		}
	}
}

reliable server function ServerChangeHitMaterial()
{	
	SetHitMaterial( bHitMaterialOn );
}

function HideTakeDamageMesh()
{
	bHitMaterialOn = false;
}

function bool Died (Controller Killer, class<DamageType> damageType, Object.Vector HitLocation)
{
	SetHitMaterial( false );
	if( Role < ROLE_Authority )
	{
		TurnOffShield();
	}
	bShieldDeployed = false;
	TTPlayerReplicationInfo( PlayerReplicationInfo ).ZoneInside = -1;
	return Super.Died( Killer, damageType, HitLocation );
}

simulated reliable server function TurnOffShield()
{
	bShieldDeployed = false;
}

function OnEnterZone( TTFlagZone FlagZone )
{
	local TTPlayerReplicationInfo PRI;
	

	
	PRI = TTPlayerReplicationInfo( PlayerReplicationInfo );
	`Log( "TTPawn : Entered FlagZone " $FlagZone.MyId );

	PRI.ZoneInside = FlagZone.MyId;
	PRI.bForceNetUpdate = true;

	// Can we hack it?
	if ( HackerCharge > 0 )
	{
		// Is it already hacked?
		if ( !FlagZone.IsHacked() )
		{
			// Hack it!
			`Log( " TTPawn : Trying to hack zone... " );
			UTInventoryManager(InvManager).OwnerEvent( 'HackZone' );
			FlagZone.Hack( self.GetTeamNum() );
		}
	}


}

function OnExitZone( TTFlagZone FlagZone )
{
	TTPlayerReplicationInfo( PlayerReplicationInfo ).ZoneInside = -1;
}



function Cloak( bool enable )
{
	if ( enable )
	{
		`Log( " TTPawn : Cloak enabled. " );
		SetInvisible( true );
		SetSkin( CloakMaterial );
	}
	else
	{
		`Log( " TTPawn : Cloak disabled. " );
		SetInvisible( false );
		SetSkin( None );
	}
}

function WolverineMode( bool CAN_YOU_HANDLE_IT_ )
{
	if ( CAN_YOU_HANDLE_IT_ )
	{
		`Log( " TTPawn : UNLEASHED THE RAGE " );
		TimeToHeal = 0;
	}
	else
	{
		`Log( " TTPawn : CONTAIN THE RAGE " );
		TimeToHeal = default.TimeToHeal;
	}
}

// Disable double jump
function DoDoubleJump( bool bUpdating )
{
}

// Disable dodge
function bool PerformDodge(eDoubleClickDir DoubleClickMove, vector Dir, vector Cross)
{
	return false;
}

// Disable feigndeath
exec simulated function FeignDeath()
{
//	ServerFeignDeath();
}
/*
simulated State Dying
{
	// Disable camera spin when waiting to respawn
	simulated function bool CalcCamera( float fDeltaTime, out vector out_CamLoc, out rotator out_CamRot, out float out_FOV )
	{
		return false;
	}
}
*/
//simulated function FindGoodEndView(PlayerController InPC, out Rotator GoodRotation)
//{}

simulated event PostRenderFor(PlayerController PC, Canvas Canvas, vector CameraPosition, vector CameraDir)
{
	local float TextXL, XL, YL, Dist;
	local vector ScreenLoc;
	local LinearColor TeamColor;
	local Color	TextColor;
	local string ScreenName;
	local UTWeapon Weap;
	local UTPlayerReplicationInfo PRI;
	local UTHUDBase HUD;

	screenLoc = Canvas.Project(Location + GetCollisionHeight()*vect(0,0,1));
	// make sure not clipped out
	if (screenLoc.X < 0 ||
		screenLoc.X >= Canvas.ClipX ||
		screenLoc.Y < 0 ||
		screenLoc.Y >= Canvas.ClipY)
	{
		return;
	}

	PRI = UTPlayerReplicationInfo(PlayerReplicationInfo);
	if ( !WorldInfo.GRI.OnSameTeam(self, PC) )
	{
		// maybe change to action music if close enough
		if ( WorldInfo.TimeSeconds - LastPostRenderTraceTime > 0.5 )
		{
			if ( !UTPlayerController(PC).AlreadyInActionMusic() && (VSize(CameraPosition - Location) < VSize(PC.ViewTarget.Location - Location)) && !IsInvisible() )
			{
				// check whether close enough to crosshair
				if ( (Abs(screenLoc.X - 0.5*Canvas.ClipX) < 0.1 * Canvas.ClipX)
					&& (Abs(screenLoc.Y - 0.5*Canvas.ClipY) < 0.1 * Canvas.ClipY) )
				{
					// periodically make sure really visible using traces
					if ( FastTrace(Location, CameraPosition,, true)
									|| FastTrace(Location+GetCollisionHeight()*vect(0,0,1), CameraPosition,, true) )
					{
						UTPlayerController(PC).ClientMusicEvent(0);;
					}
				}
			}
			LastPostRenderTraceTime = WorldInfo.TimeSeconds + 0.2*FRand();
		}
		return;
	}

	// make sure not behind weapon
	if ( UTPawn(PC.Pawn) != None )
	{
		Weap = UTWeapon(UTPawn(PC.Pawn).Weapon);
		if ( (Weap != None) && Weap.CoversScreenSpace(screenLoc, Canvas) )
		{
			return;
		}
	}
	else if ( (UTVehicle_Hoverboard(PC.Pawn) != None) && UTVehicle_Hoverboard(PC.Pawn).CoversScreenSpace(screenLoc, Canvas) )
	{
		return;
	}

	// periodically make sure really visible using traces
	if ( WorldInfo.TimeSeconds - LastPostRenderTraceTime > 0.5 )
	{
		LastPostRenderTraceTime = WorldInfo.TimeSeconds + 0.2*FRand();
		bPostRenderTraceSucceeded = FastTrace(Location, CameraPosition)
									|| FastTrace(Location+GetCollisionHeight()*vect(0,0,1), CameraPosition);
	}
	if ( !bPostRenderTraceSucceeded )
	{
		return;
	}

	class'TTHUD'.Static.GetTeamColor( GetTeamNum(), TeamColor, TextColor);

	Dist = VSize(CameraPosition - Location);
	if ( Dist < TeamBeaconPlayerInfoMaxDist )
	{
		ScreenName = GetScreenName();
		Canvas.StrLen(ScreenName, TextXL, YL);
		XL = Max( TextXL, 24 * Canvas.ClipX/1024 * (1 + 2*Square((TeamBeaconPlayerInfoMaxDist-Dist)/TeamBeaconPlayerInfoMaxDist)));
	}
	else
	{
		XL = Canvas.ClipX * 16 * TeamBeaconPlayerInfoMaxDist/(Dist * 1024);
		YL = 0;
	}

	Class'UTHUD'.static.DrawBackground(ScreenLoc.X-0.7*XL,ScreenLoc.Y-1.8*YL,1.4*XL,1.9*YL, TeamColor, Canvas);

	if ( (PRI != None) && (Dist < TeamBeaconPlayerInfoMaxDist) )
	{
		Canvas.DrawColor = TextColor;
		Canvas.SetPos(ScreenLoc.X-0.5*TextXL,ScreenLoc.Y-1.2*YL);
		Canvas.DrawText( ScreenName, true, , , class'UTHUD'.default.TextRenderInfo );
	}

	HUD = UTHUDBase(PC.MyHUD);
	if ( (HUD != None) && !HUD.bCrosshairOnFriendly
		&& (Abs(screenLoc.X - 0.5*Canvas.ClipX) < 0.1 * Canvas.ClipX)
		&& (screenLoc.Y <= 0.5*Canvas.ClipY) )
	{
		// check if top to bottom crosses center of screen
		screenLoc = Canvas.Project(Location - GetCollisionHeight()*vect(0,0,1));
		if ( screenLoc.Y >= 0.5*Canvas.ClipY )
		{
			HUD.bCrosshairOnFriendly = true;
		}
	}
}

replication
{
	if (bNetDirty && bNetOwner)
		bBlinded, timeBlind, bHitMaterialOn, bShieldDeployed;	
}

defaultproperties
{
	bBlinded = false
	bShieldDeployed = false
	timeBlind = 0
	blindDecay = 0

	InventoryManagerClass = class'TTInventoryManager'
	TakeDamageMaterial = Material'TT_Character.Materials.character_hit_mat_01'

	SprintSpeed = 800
	WalkSpeed = 440

	AirSpeed=600.0
	WaterSpeed=220.0

	JumpZ = 390.0
	MaxJumpHeight = 500

	TimeToHeal = 6.0f
	HPPS = 6;

	HackerCharge = 0;

	Regen = RegenSFX_Off;

	HitEffectTime = 0.75;

	TransInEffects(0)=class'TTEmit_TransLocateOut'
	TransInEffects(1)=class'TTEmit_TransLocateOut'
}

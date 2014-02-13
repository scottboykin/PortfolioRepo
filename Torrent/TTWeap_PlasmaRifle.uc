class TTWeap_PlasmaRifle extends TTWeapon;

var bool bBurstMode;
var int burstCount;

var( PlasmaRifle ) float delayBetweenBurstShots;
var( PlasmaRifle ) int numberOfShotsPerBurst;
var( PlasmaRifle ) float burstFireRate;
var( PlasmaRifle ) float autoFireRate;
var( PlasmaRifle ) float modeSwitchDelay;

var bool bReadyToFire;

var const TTProj_PlasmaRound roundArchetype;

// Ammo Display Stuff (Chris Burris)
var TextureRenderTarget2D RenderTexture;
var TTRifleAmmoMoviePlayer AmmoDisplay;

simulated function PostBeginPlay()
{
	// Ammo Display Stuff (Chris Burris)
	super.PostBeginPlay();
	RenderTexture = TextureRenderTarget2D 'TT_Weapons.rifleAmmoDisplay_TRT2D';
	AmmoDisplay = new class 'TTRifleAmmoMoviePlayer';
	AmmoDisplay.GetPC();
	AmmoDisplay.RenderTexture = RenderTexture;
	AmmoDisplay.Init();

	FireInterval[0] = autoFireRate;
	FireInterval[1] = burstFireRate;
}

// Ammo Display Stuff (Chris Burris)
simulated function Tick(float deltaTime)
{
	AmmoDisplay.SetAmmoText();

	Super.Tick(deltaTime);
}

simulated reliable server function ServerWeaponReadyToFire( bool ready )
{
	bReadyToFire = ready;
}

function WeaponReadyToFire( bool ready )
{
	bReadyToFire = ready;
}

simulated function SetTeamMaterial( int TeamId )
{
	if ( TeamId == 1 )
	{
		SetSkin( Material'TT_Weapons.Materials.rifle_orange_mat' );
	}
	else
	{
		SetSkin( Material'TT_Weapons.Materials.rifle_cyan_mat' );
	}
}

simulated function Projectile ProjectileFire()
{
	local Projectile p;

	if( !bBurstMode )
	{
		p = self.SpawnArchetypeProjectile( roundArchetype );
	}

	else
	{
		p = self.SpawnArchetypeProjectile( roundArchetype );
		++burstCount;
		if( burstCount < numberOfShotsPerBurst )
		{
			SetTimer( delayBetweenBurstShots );
		}
		else
		{
			burstCount = 0;
			if( Role < ROLE_Authority )
			{
				ServerWeaponReadyToFire( false );
			}
			else
			{
				WeaponReadyToFire( false );
			}
		}
	}

	return p;
	
}

simulated function FireAmmunition()
{
	if( CurrentFireMode == 0 )
	{
		bBurstMode = false;
	}
	else if( CurrentFireMode == 1 )
	{
		bBurstMode = true;
	}
}

simulated function StopFire( byte FireModeNum )
{
	if( FireModeNum == 1 )
	{
		if( Role < ROLE_Authority )
		{
			ServerWeaponReadyToFire( true );
		}
		else
		{
			WeaponReadyToFire( true );
		}
	}

	super.StopFire( FireModeNum );
}

simulated function CustomFire()
{
	bBurstMode = !bBurstMode;
	if( bBurstMode )
	{
		FireInterval[0] = burstFireRate;
	}
	else
	{
		FireInterval[0] = autoFireRate;
	}
}

simulated function Timer()
{
	if( HasAmmo( 0 ) )
	{
		FireAmmunition();
	}
}

replication
{
	if( bNetDirty && bNetOwner )
		bReadyToFire;
}

defaultproperties
{
    // Weapon SkeletalMesh
	Begin Object class=AnimNodeSequence Name=MeshSequenceA
	End Object

	// Weapon SkeletalMesh
	Begin Object Name=FirstPersonMesh
		SkeletalMesh=SkeletalMesh'TT_Weapons.SK_mesh.rifle_mesh_final_01'
		AnimSets(0)=AnimSet'TT_Weapons.Animations.pistol_anim_01'
		AnimSets(1)=AnimSet'TT_Weapons.Animations.pistol_anim_01'
		Animations=MeshSequenceA
		Translation=(X=0,Y=1,Z=-1)
		FOV=60.0
	End Object

	Begin Object Name=PickupMesh
		SkeletalMesh=SkeletalMesh'TT_Weapons.SK_mesh.rifle_mesh_final_01'
	End Object

	InstantHitMomentum(1)=+10000.0

	WeaponFireTypes(0)=EWFT_Projectile
	WeaponProjectiles(0)=class'TTProj_PlasmaRound'
	WeaponFireTypes(1)=EWFT_Projectile
	WeaponProjectiles(1)=class'TTProj_PlasmaRound'

	autoFireRate = 0.25
	burstFireRate = 0.75
	numberOfShotsPerBurst = 3
	delayBetweenBurstShots = 0.125
	modeSwitchDelay = 0.25
	bReadyToFire = true
	//InstantHitDamage(0)=15

	AttachmentClass=class'Torrent.TTAttachment_PlasmaRifle'

	WeaponFireSnd[0]=SoundCue'TT_Sounds.TT_Sounds.TT_sfx_cue_riflefire'
	WeaponFireSnd[1]=SoundCue'TT_Sounds.TT_Sounds.TT_sfx_cue_riflefire'
	WeaponEquipSnd=SoundCue'TT_Sounds.TT_Sounds.TT-sfx_equip_rifle_Cue'
	WeaponPutDownSnd=SoundCue'A_Weapon_ShockRifle.Cue.A_Weapon_SR_LowerCue'
	PickupSound=SoundCue'A_Pickups.Weapons.Cue.A_Pickup_Weapons_Shock_Cue'

	roundArchetype = TTProj_PlasmaRound'TT_Weapons.Archetype.TT_PlasmaRound'

	MaxDesireability=0.65
	AIRating=0.65
	CurrentRating=0.65
	bInstantHit=false
	bSplashJump=false
	bRecommendSplashDamage=false
	bSniping=true
	ShouldFireOnRelease(0)=0
	ShotCost(0) = 1
	ShotCost(1) = 1
	

	bBurstMode = false;
	burstCount = 0;

	bHasRecoil = true
	maxRecoilSpread = 0.1
	recoilSpreadIncreaseRate = 0.02
	recoilSpreadDecreaseRate = 0.0005

	FireOffset=(X=20,Y=5)
	PlayerViewOffset=(X=10,Y=2.5,Z=-8.0)

	AmmoCount=120
	LockerAmmoCount=80
	MaxAmmoCount=200

	MuzzleFlashSocket=MuzzleFlashSocket
	MuzzleFlashPSCTemplate=ParticleSystem'TT_Particles.Particles.muzzle_flash_02'
	MuzzleFlashAltPSCTemplate=ParticleSystem'TT_Particles.Particles.muzzle_flash_02'
	bMuzzleFlashPSCLoops=false
	MuzzleFlashLightClass=class'UTGame.UTLinkGunMuzzleFlashLight'

	MuzzleFlashDuration = 0.33

	CrossHairCoordinates=(U=256,V=0,UL=64,VL=64)
	LockerRotation=(Pitch=32768,Roll=16384)

	IconCoordinates=(U=728,V=382,UL=162,VL=45)

	WeaponColor=(R=160,G=0,B=255,A=255)

	InventoryGroup=2
	GroupWeight=0.5

	IconX=400
	IconY=129
	IconWidth=22
	IconHeight=48

	Begin Object Class=ForceFeedbackWaveform Name=ForceFeedbackWaveformShooting1
		Samples(0)=(LeftAmplitude=90,RightAmplitude=40,LeftFunction=WF_Constant,RightFunction=WF_LinearDecreasing,Duration=0.1200)
	End Object
	WeaponFireWaveForm=ForceFeedbackWaveformShooting1
}

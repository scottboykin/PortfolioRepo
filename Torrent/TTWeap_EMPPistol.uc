class TTWeap_EMPPistol extends TTWeapon;

var(EMPPistol) float empRechargeDelay;
//var(EMPPistol) float empMineLimit;

var bool bEMPReady;
var bool bEMPDetonated;
var bool bReadyToFire;
var SoundCue empRecharged;
var TTProj_EMPBall empBall;

//var array<Projectile> placedMines;

//var const TTProj_EMPMine mineArchetype;

var const TTProj_EMPRound roundArchetype;
var const TTProj_EMPBall ballArchetype;

// Ammo Display Stuff (Chris Burris)
var TextureRenderTarget2D RenderTexture;
var TTPistolAmmoMoviePlayer AmmoDisplay;

replication
{
	if (bNetDirty && bNetOwner )
		bEMPReady, bEMPDetonated, bReadyToFire;
}

simulated function FireAmmunition()
{
	local Vector v;

	if( CurrentFireMode == 1 && !bEMPDetonated )
	{
		empBall.Explode( empBall.Location, v );	
		bEMPDetonated = true;
	}
	else if( ( CurrentFireMode == 0 && bReadyToFire ) || ( CurrentFireMode == 1 && bEMPReady ) )
	{
		super.FireAmmunition();
	}
}

simulated function Projectile ProjectileFire()
{
	local Projectile p;

	if( CurrentFireMode == 1 )
	{
		bEMPReady = false;
		SetTimer( empRechargeDelay - 1.5, false, 'playRechargeSound' );
		p = SpawnArchetypeProjectile( ballArchetype );
		bEMPDetonated = false;
		empBall = TTProj_EMPBall( p );
	}
	else
	{
		p = SpawnArchetypeProjectile( roundArchetype );
		ServerWeaponReadyToFire( false );
		WeaponReadyToFire( false );
	}

	return p;
}

simulated function StopFire( byte FireModeNum )
{
	if( FireModeNum == 0 && Role < ROLE_Authority )
	{
		ServerWeaponReadyToFire( true );
	}

	else if( FireModeNum == 0 )
	{
		WeaponReadyToFire( true );
	}

	super.StopFire( FireModeNum );
}

simulated function playRechargeSound()
{
	PlaySound( SoundCue'TT_Sounds.TT_Sounds.TT_sfx_cue_emprecharge', true );
	PlaySound( SoundCue'TT_Sounds.TT_Sounds.TT_sfx_cue_emprecharge', true ); //Played twice because unreal is skipping the playcue every other time.
	SetTimer( 1.5 );
}

simulated reliable server function ServerWeaponReadyToFire( bool ready )
{
	bReadyToFire = ready;
	ClientWeaponReadyToFire( ready );
}

simulated reliable client function ClientWeaponReadyToFire( bool ready )
{
	bReadyToFire = ready;
}

function WeaponReadyToFire( bool ready )
{
	bReadyToFire = ready;
}

simulated function Timer()
{
	bEMPReady = true;	
}

simulated function Tick( float DeltaTime )
{
	AmmoDisplay.SetAmmoText(); // Ammo Display Stuff (Chris Burris)
	
	if( Owner == none )
	{
		Destroy();
	}

	Super.Tick(deltaTime);
}

function RemoveMine( TTProj_EMPMine mine )
{
	//placedMines.RemoveItem( mine );
}

simulated function PostBeginPlay()
{
	// Ammo Display Stuff (Chris Burris)
	super.PostBeginPlay();
	RenderTexture = TextureRenderTarget2D 'TT_Weapons.pistolAmmoDisplay_TRT2D';
	AmmoDisplay = new class 'TTPistolAmmoMoviePlayer';
	AmmoDisplay.GetPC();
	AmmoDisplay.RenderTexture = RenderTexture;
	AmmoDisplay.Init();

	//FireInterval[0] = 0.1;
}

replication
{
	if( bNetDirty && bNetOwner && Role == ROLE_Authority )
		bReadyToFire;
}

DefaultProperties
{
	WeaponColor=(R=255,G=255,B=0,A=255)
	FireInterval(0)=+0.24
	FireInterval(1)=+0.25
	PlayerViewOffset=(X=35.0,Y=10,Z=-18.0)


	Begin Object class=AnimNodeSequence Name=MeshSequenceA
		bCauseActorAnimEnd=true
	End Object

	// Weapon SkeletalMesh
	Begin Object Name=FirstPersonMesh
		SkeletalMesh=SkeletalMesh'TT_Weapons.SK_mesh.pistol_mesh_final_01'
		AnimSets(0)=AnimSet'TT_Weapons.Animations.pistol_anim_01'
		Animations=MeshSequenceA
		//Rotation=(Yaw=-16384)
		Translation=(X=0,Y=1,Z=1)
		Scale=1.5
		FOV=60.0
	End Object

	// Pickup staticmesh
	Begin Object Name=PickupMesh
		SkeletalMesh=SkeletalMesh'TT_Weapons.SK_mesh.pistol_mesh_final_01'
	End Object

	AttachmentClass=class'Torrent.TTAttachment_EMPPistol'

	roundArchetype = TTProj_EMPRound'TT_Weapons.Archetype.TT_EMPRound'
	ballArchetype = TTProj_EMPBall'TT_Weapons.Archetype.TT_EMP'

	FireOffset=(X=12,Y=10,Z=-10)

	WeaponFireTypes(0)=EWFT_Projectile
	WeaponProjectiles(0)=class'TTProj_EMPRound'
	WeaponFireTypes(1) = EWFT_Projectile
	WeaponProjectiles(1) = class'TTProj_EMPBall'

	MuzzleFlashSocket=MuzzleFlashSocket
	MuzzleFlashPSCTemplate=ParticleSystem'TT_Particles.Particles.muzzle_flash_01'
	MuzzleFlashAltPSCTemplate=ParticleSystem'TT_Particles.Particles.muzzle_flash_01'
	bMuzzleFlashPSCLoops=false
	MuzzleFlashLightClass=class'UTGame.UTLinkGunMuzzleFlashLight'

	MuzzleFlashDuration = 0.33

	//EffectSockets(0)=mflash

	bEMPDetonated = true
	bReadyToFire = true
	bEMPReady = true;
	empRechargeDelay = 5.0

	WeaponEquipSnd=SoundCue'TT_Sounds.TT_Sounds.TT-sfx_equip_pistol_Cue'
	WeaponPutDownSnd=SoundCue'A_Weapon_Link.Cue.A_Weapon_Link_LowerCue'
	WeaponFireSnd(0)=SoundCue'TT_Sounds.TT_Sounds.TT_sfx_cue_pistolfire1'
	WeaponFireSnd(1)=SoundCue'TT_Sounds.TT_Sounds.TT_sfx_cue_pistolemp'
	empRecharged = SoundCue'TT_Sounds.TT_Sounds.TT_sfx_cue_emprecharge'

	MaxDesireability=0.7
	AIRating=+0.3
	CurrentRating=+0.3
	bFastRepeater = true
    bInstantHit = false
    bSplashJump = false
    bRecommendSplashDamage=false
    bSniping=false
	InventoryGroup=1
	GroupWeight=0.5
	ShotCost(1) = 5


	PickupSound=SoundCue'A_Pickups.Weapons.Cue.A_Pickup_Weapons_Link_Cue'

	AmmoCount=100
	LockerAmmoCount=100
	MaxAmmoCount=100

	IconX=412
	IconY=82
	IconWidth=40
	IconHeight=36

	CrossHairCoordinates=(U=384,V=0,UL=64,VL=64)

	LockerRotation=(pitch=0,yaw=0,roll=-16384)
	IconCoordinates=(U=453,V=467,UL=147,VL=41)
}

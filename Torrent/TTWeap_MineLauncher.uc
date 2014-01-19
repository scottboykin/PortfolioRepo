class TTWeap_MineLauncher extends TTWeapon;

var( MineLauncher ) int numOfPellets;
//var( MineLauncher ) int mineLimit;
var( MineLauncher ) float shieldDrainRate;
var( MineLauncher ) float shieldPercentDmgReduction;
var( MineLauncher ) int shieldDrainAmount;
var bool bReadyToFire;

var array< Projectile > placedMines;
var int currentPellet;

var const UTProjectile rocketArchetype;
var const UTProjectile mineArchetype;

// Ammo Display Stuff (Chris Burris)
var TextureRenderTarget2D RenderTexture;
var TTShotgunAmmoMoviePlayer AmmoDisplay;

var StaticMeshComponent shield;

simulated function FireAmmunition()
{
	//if( CurrentFireMode == 0 || ( CurrentFireMode == 1 && placedMines.Length < mineLimit ) )
	if( ( CurrentFireMode == 0 && !TTPawn( Owner ).bShieldDeployed && bReadyToFire ) || CurrentFireMode == 1 )
	{
		// if this is the local player, play the firing effects
		PlayFiringSound();

		// Use ammunition to fire
		ConsumeAmmo( CurrentFireMode );

		// Handle the different fire types
		switch( WeaponFireTypes[CurrentFireMode] )
		{
			case EWFT_InstantHit:
				InstantFire();
				break;

			case EWFT_Projectile:
				ProjectileFire();
				break;

			case EWFT_Custom:
				CustomFire();
				break;
		}

		NotifyWeaponFired( CurrentFireMode );

		if (UTPawn(Instigator) != None)
		{
			UTPawn(Instigator).DeactivateSpawnProtection();
		}

		UTInventoryManager(InvManager).OwnerEvent('FiredWeapon');
	}
}

simulated function Projectile ProjectileFire()
{
	local Projectile p;

	if( CurrentFireMode == 0 )
	{
		p = SpawnArchetypeProjectile( rocketArchetype );
		++currentPellet;
		if( currentPellet < numOfPellets && HasAmmo( 0 ) )
		{
			ProjectileFire();
		}
		else
		{
			currentPellet = 0;
		}
	}

	ServerWeaponReadyToFire( false );
	WeaponReadyToFire( false );
	//else
	//{	
	//	p = SpawnArchetypeProjectile( mineArchetype );
	//	TTProj_ProxiMine( p ).SetWeaponOwner( self );
	//	placedMines.AddItem( p );
	//}

	return p;
}

simulated function CustomFire()
{
	if( !TTPlayerReplicationInfo( TTPawn( Owner ).PlayerReplicationInfo ).bShieldDeployed && ROLE < ROLE_Authority )
	{
		ServerDeployShield( true );
	}
	else if( !TTPlayerReplicationInfo( TTPawn( Owner ).PlayerReplicationInfo ).bShieldDeployed )
	{
		AddAmmo( -shieldDrainAmount );
		TTPawn( Owner ).bShieldDeployed = true;
		TTPlayerReplicationInfo( TTPawn( Owner ).PlayerReplicationInfo ).bShieldDeployed = true;
		SetTimer( shieldDrainRate, true, 'DrainShield' );
	}

	super.CustomFire();
}

simulated function StopFire( byte FireModeNum )
{
	if( FireModeNum == 1 && TTPlayerReplicationInfo( TTPawn( Owner ).PlayerReplicationInfo ).bShieldDeployed && ROLE < ROLE_Authority )
	{
		ServerDeployShield( false );
	}
	else if( FireModeNum == 1 && TTPlayerReplicationInfo( TTPawn( Owner ).PlayerReplicationInfo ).bShieldDeployed )
	{
		TTPawn( Owner ).bShieldDeployed = false;	
		TTPlayerReplicationInfo( TTPawn( Owner ).PlayerReplicationInfo ).bShieldDeployed = false;
		SetTimer( 0, , 'DrainShield' );
	}

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

simulated reliable server function ServerDeployShield( bool deploy )
{
	TTPawn( Owner ).bShieldDeployed = deploy;
	TTPlayerReplicationInfo( TTPawn( Owner ).PlayerReplicationInfo ).bShieldDeployed = deploy;
	if( deploy )
	{
		SetTimer( shieldDrainRate, true, 'DrainShield' );
	}

	else
	{
		SetTimer( 0, , 'DrainShield' );
	}
	ClientDeployShield( deploy );
}

simulated reliable client function ClientDeployShield( bool deploy )
{
	TTPawn( Owner ).bShieldDeployed = deploy;
	TTPlayerReplicationInfo( TTPawn( Owner ).PlayerReplicationInfo ).bShieldDeployed = deploy;
	if( deploy )
	{
		SetTimer( shieldDrainRate, true, 'DrainShield' );
	}

	else
	{
		SetTimer( 0, , 'DrainShield' );
	}
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

simulated function DrainShield()
{
	AddAmmo( -shieldDrainAmount );
	if( self.GetAmmoCount() <= 0 )
	{
		WeaponEmpty();
	}
}

simulated function WeaponEmpty()
{
	if( TTPawn( Owner ).bShieldDeployed )
	{
		ServerDeployShield( false );
	}
	super.WeaponEmpty();

}

simulated function PlayFireEffects( byte FireModeNum, optional vector HitLocation )
{
	// Play Weapon fire animation

	if ( FireModeNum < WeaponFireAnim.Length && WeaponFireAnim[FireModeNum] != '' )
		PlayWeaponAnimation( WeaponFireAnim[FireModeNum], GetFireInterval(FireModeNum) / 4.0 );
	if ( FireModeNum < ArmFireAnim.Length && ArmFireAnim[FireModeNum] != '' && ArmsAnimSet != none)
		PlayArmAnimation( ArmFireAnim[FireModeNum], GetFireInterval(FireModeNum) / 4.0 );

	// Start muzzle flash effect
	CauseMuzzleFlash();

	ShakeView();
}

simulated function PostBeginPlay()
{	
	Super.PostBeginPlay();

	// Ammo Display Stuff (Chris Burris)
	RenderTexture = TextureRenderTarget2D 'TT_Weapons.shotgunAmmoDisplay_TRT2D';
	AmmoDisplay = new class 'TTShotgunAmmoMoviePlayer';
	AmmoDisplay.GetPC();
	AmmoDisplay.RenderTexture = RenderTexture;
	AmmoDisplay.Init();
	
	FireInterval[1] = 0.1;

}

function RemoveMine( TTProj_ProxiMine mine )
{
	placedMines.RemoveItem( mine );
}


replication
{
	if (bNetDirty && bNetOwner && Role == ROLE_Authority)
		bReadyToFire;
}

DefaultProperties
{
	WeaponColor=(R=255,G=0,B=0,A=255)
	FireInterval(0)=+0.75                        
    //FireInterval(1)=+0.75
	PlayerViewOffset=(X=10,Y=2.5,Z=-8.0)

	numOfPellets = 5
	shieldDrainRate = 1.0
	shieldDrainAmount = 1
	shieldPercentDmgReduction = 0.5f
	//mineLimit = 3
	bReadyToFire = true

	Begin Object Name=FirstPersonMesh
		SkeletalMesh=SkeletalMesh'TT_Weapons.SK_mesh.shotgun_mesh_final_01'
		PhysicsAsset=None
		AnimTreeTemplate=AnimTree'WP_RocketLauncher.Anims.AT_WP_RocketLauncher_1P_Base'
		AnimSets(0)=AnimSet'TT_Weapons.Animations.pistol_anim_01'
		Translation=(X=0,Y=2,Z=-2)
		Rotation=(Yaw=0)
		scale=1.0
		FOV=60.0
		bUpdateSkelWhenNotRendered=true
	End Object

	BaseSkelComponent = FirstPersonMesh
	AttachmentClass=class'TTAttachment_MineLauncher'

	// Pickup staticmesh
	Begin Object Name=PickupMesh
		SkeletalMesh=SkeletalMesh'TT_Weapons.SK_mesh.shotgun_mesh_final_01'
	End Object

	WeaponFireSnd[0]=SoundCue'TT_Sounds.TT_Sounds.TT_sfx_cue_shotgunfire'
	//WeaponFireSnd[1]=SoundCue'TT_Sounds.TT_Sounds.TT_sfx_cue_mineshot'
	WeaponEquipSnd=SoundCue'TT_Sounds.TT_Sounds.TT-sfx_equip_launcher_Cue'

	WeaponRange=50
    AmmoCount=24
    LockerAmmoCount=1
    MaxAmmoCount=36

	FireOffset=(X=20,Y=12,Z=-5)

	WeaponFireTypes(0) = EWFT_Projectile
	WeaponProjectiles(0)=class'TTProj_MicroRockets'
	WeaponFireTypes(1)=EWFT_Custom
	ShotCost(1) = 0;
    //WeaponFireTypes(1)=EWFT_Projectile
    //WeaponProjectiles(1)=class'TTProj_ProxiMine'
    
	MuzzleFlashSocket=MuzzleFlashSocket
	MuzzleFlashPSCTemplate=ParticleSystem'TT_Particles.Particles.muzzle_flash_03'
	MuzzleFlashAltPSCTemplate=ParticleSystem'TT_Particles.Particles.muzzle_flash_03'
	bMuzzleFlashPSCLoops=false
	MuzzleFlashLightClass=class'UTGame.UTLinkGunMuzzleFlashLight'

	MuzzleFlashDuration = 0.33

    MaxDesireability=0.7
    AIRating=+0.3
    CurrentRating=+0.3
    bFastRepeater = true
    bInstantHit = false
    bSplashJump = false
    bRecommendSplashDamage=false
    bSniping=false
    ShouldFireOnRelease(0)=0
    ShouldFireOnRelease(1)=0
    InventoryGroup=3
    GroupWeight=0.5

    currentPellet = 0
	
	mineArchetype = TTProj_ProxiMine'TT_Weapons.Archetype.TT_ProxiMine'
	rocketArchetype = TTProj_MicroRockets'TT_Weapons.Archetype.TT_MicroRockets'

    IconX=412
    IconY=82
    IconWidth=40
    IconHeight=36

   
    LockerRotation=(pitch=0,yaw=0,roll=-16384)
    IconCoordinates=(U=453,V=467,UL=147,VL=41)

}

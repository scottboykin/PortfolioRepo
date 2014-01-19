//#include TTWeap_MineLauncher

class TTProj_ProxiMine extends UTProjectile;

var(ProxiMine) float detonationDelay;
var(ProxiMine) float activationRadius;

var TTWeap_MineLauncher weapOwner;
var byte allyTeam;
var bool bTripped;
var bool bPlanted;


var Vector explosionDir;
/**
 * Set the initial velocity and cook time
 */
simulated function PostBeginPlay()
{
	Super.PostBeginPlay();
	SetTimer( 2.5, false );                  //Grenade begins unarmed
	RandSpin( 100000 );
}

function Init(vector Direction)
{
	explosionDir.X = 0.0;
	explosionDir.Y = 0.0;
	explosionDir.Z = 1.0;
	SetRotation(Rotator(Direction));

	Velocity = Speed * Direction;
	Acceleration = AccelRate * Normal(Velocity);
}

function SetWeaponOwner( TTWeap_MineLauncher mineOwner )
{
	weapOwner = mineOwner;
	allyTeam = TTPawn( mineOwner.Owner ).Controller.GetTeamNum();
}

simulated function ProcessTouch( Actor Other, Vector HitLocation, Vector HitNormal )
{}

simulated function Tick( float DeltaTime )
{
	local TTPawn A;

	if( bPlanted && !bTripped )
	{
		foreach CollidingActors( class 'TTPawn', A, activationRadius )
		{
			if( A.Controller.GetTeamNum() != allyTeam )
			{
				MineActivated();
			}
		}
	}

	if( weapOwner == none )
	{
		Destroy();
	}

	super.Tick( DeltaTime );

}

function MineActivated()
{
	bTripped = true;
	SetTimer( detonationDelay );
}

/**
 * Explode
 */
simulated function Timer()
{	
	Explode(Location, explosionDir);
}

simulated function Explode( Vector HitLocation, Vector HitNormal )
{
	super.Explode( HitLocation, HitNormal );
	weapOwner.RemoveMine( self );
}

/**
 * Give a little bounce
 */
simulated event HitWall(vector HitNormal, Actor Wall, PrimitiveComponent WallComp)
{
	bBlockedByInstigator = true;

	if ( WorldInfo.NetMode != NM_DedicatedServer )
	{
		PlaySound(ImpactSound, true);
	}

	explosionDir = HitNormal;
	Speed = 0;
	CustomGravityScaling = 0;
	SetPhysics( PHYS_None );
	if( !bTripped )
	{
		SetTimer( 0 );
	}
	SetRotation(Rotator(HitNormal));
	bCollideWorld = false;
	bPlanted = true;
}

/**
 * When a grenade enters the water, kill effects/velocity and let it sink
 */
simulated function PhysicsVolumeChange( PhysicsVolume NewVolume )
{
	if ( WaterVolume(NewVolume) != none )
	{
		Velocity *= 0.25;
	}

	Super.PhysicsVolumeChange(NewVolume);
}


defaultproperties
{
	ProjFlightTemplate=ParticleSystem'WP_RocketLauncher.Effects.P_WP_RocketLauncher_Smoke_Trail'

	ProjExplosionTemplate=ParticleSystem'WP_RocketLauncher.Effects.P_WP_RocketLauncher_RocketExplosion'
	ExplosionLightClass=class'UTGame.UTRocketExplosionLight'

	speed=700
	MaxSpeed=1000.0
	Damage=100.0
	DamageRadius=200
	MomentumTransfer=50000
	MyDamageType=class'UTDmgType_Grenade'
	LifeSpan=0.0
	ExplosionSound = SoundCue'TT_Sounds.TT_Sounds.TT_sfx_cue_mineexplosion'
	ExplosionDecal=MaterialInstanceTimeVarying'WP_RocketLauncher.Decals.MITV_WP_RocketLauncher_Impact_Decal01'
	DecalWidth=128.0
	DecalHeight=128.0
	bCollideWorld=true
	bCollideActors=true
	bBounce=false
	TossZ=+245.0
	
	CheckRadius=36.0
	bTripped = false
	bPlanted = false
	allyTeam = 0;
	detonationDelay = 0.25;
	activationRadius = 100.0;

	ImpactSound = SoundCue'TT_Sounds.TT_Sounds.TT_sfx_cue_mineset'

	bNetTemporary=False
	bWaitForEffects=false
}

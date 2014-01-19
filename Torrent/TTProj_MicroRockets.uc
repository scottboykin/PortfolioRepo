class TTProj_MicroRockets extends UTProjectile;

var( MicroRockets ) ParticleSystem projtileFlightTemplate;
var( MicroRockets ) float AccelerationRate;

simulated function PostBeginPlay()
{
	// force ambient sound if not vehicle game mode
	SetDrawScale( 0.25 );
	bImportantAmbientSound = !WorldInfo.bDropDetail;
	ProjFlightTemplate = projtileFlightTemplate;
	AccelRate = AccelerationRate;
	ProjectileLightClass = class'Torrent.TTMicroRocketLight';
	Super.PostBeginPlay();
}

simulated event HitWall(vector HitNormal, Actor Wall, PrimitiveComponent WallComp)
{
	MomentumTransfer = 1.0;

	Super.HitWall(HitNormal, Wall, WallComp);
}

defaultproperties
{
	projtileFlightTemplate=ParticleSystem'TT_Particles.Particles.shotgun_particle_01'
	ProjExplosionTemplate=ParticleSystem'TT_Particles.Particles.shotgun_impact_01'
	
	ExplosionDecal=MaterialInstanceTimeVarying'WP_RocketLauncher.Decals.MITV_WP_RocketLauncher_Impact_Decal01'
	DecalWidth=128.0
	DecalHeight=128.0

	speed=500//100.0
	MaxSpeed=3500//5000.0
	AccelRate = 6000//100000.0
	AccelerationRate = 5000
	Damage=15.0
	DamageRadius=0.0
	MomentumTransfer=8500
	//MyDamageType=class'UTDmgType_Instagib'
	MyDamageType=class'TTDmgType'
	LifeSpan=0.5
	AmbientSound=SoundCue'A_Weapon_RocketLauncher.Cue.A_Weapon_RL_Travel_Cue'
	ExplosionSound=none
	RotationRate=(Roll=50000)
	bCollideWorld=true
	CheckRadius=42.0
	bCheckProjectileLight=true
	ExplosionLightClass=none	
	//ProjectileLightClass=class'Torrent.TTMicroRocketLight'
	//ExplosionLightClass=class'UTGame.UTRocketExplosionLight'

	bWaitForEffects=true
	bAttachExplosionToVehicles=false
}
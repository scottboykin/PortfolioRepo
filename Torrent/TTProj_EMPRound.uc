class TTProj_EMPRound extends UTProjectile;

var vector ColorLevel;
var vector ExplosionColor;
var(EMPRound) float AccelerationRate;

simulated function PostBeginPlay()
{
	AccelRate = AccelerationRate;
	ProjectileLightClass = class'Torrent.TTEMPRoundLight';
	Super.PostBeginPlay();
}

simulated function ProcessTouch (Actor Other, vector HitLocation, vector HitNormal)
{
	if ( Other != Instigator )
	{
		if ( !Other.IsA('Projectile') || Other.bProjTarget )
		{
			MomentumTransfer = (UTPawn(Other) != None) ? 0.0 : 1.0;
			Other.TakeDamage(Damage, InstigatorController, HitLocation, MomentumTransfer * Normal(Velocity), MyDamageType,, self);
			Explode(HitLocation, HitNormal);
		}
	}
}

simulated event HitWall(vector HitNormal, Actor Wall, PrimitiveComponent WallComp)
{
	MomentumTransfer = 1.0;

	Super.HitWall(HitNormal, Wall, WallComp);
}

simulated function SpawnFlightEffects()
{
	Super.SpawnFlightEffects();
	if (ProjEffects != None)
	{
		ProjEffects.SetVectorParameter('LinkProjectileColor', ColorLevel);
	}
}


simulated function SetExplosionEffectParameters(ParticleSystemComponent ProjExplosion)
{
	Super.SetExplosionEffectParameters(ProjExplosion);
	ProjExplosion.SetVectorParameter('LinkImpactColor', ExplosionColor);
}

defaultproperties
{
	ProjFlightTemplate=ParticleSystem'TT_Particles.Particles.pistol_particle_01'
	ProjExplosionTemplate=ParticleSystem'TT_Particles.Particles.pistol_impact_01'
	MaxEffectDistance=7000.0

	ExplosionDecal=MaterialInstanceTimeVarying'WP_RocketLauncher.Decals.MITV_WP_RocketLauncher_Impact_Decal01'
	DecalWidth=128.0
	DecalHeight=128.0

	Speed=2000
	//MaxSpeed=5000
	AccelRate=3000.0
	AccelerationRate = 3000

	Damage=26
	DamageRadius=0
	MomentumTransfer=0
	CheckRadius=26.0

	//MyDamageType=class'UTDmgType_Instagib'
	MyDamageType=class'TTDmgType'
	LifeSpan=3.0
	NetCullDistanceSquared=+144000000.0

	bCollideWorld=true
	DrawScale=1.2
	bCheckProjectileLight = true

	ExplosionSound=SoundCue'A_Weapon_Link.Cue.A_Weapon_Link_ImpactCue'
	ColorLevel=(X=1,Y=1.3,Z=1)
	ExplosionColor=(X=1,Y=1,Z=1);
}

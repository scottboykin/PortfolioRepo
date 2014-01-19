class TTProj_EMPBall extends UTProjectile;

var(EMP) float blindDuration;
var(EMP) float blindDistance;
var(EMP) float AccelerationRate;

simulated function PostBeginPlay()
{
	AccelRate = AccelerationRate;
	super.PostBeginPlay();
}

simulated function ProcessTouch(Actor Other, Vector HitLocation, Vector HitNormal)
{
	Explode( HitLocation, HitNormal );
}

simulated function Explode( Vector HitLocation, Vector HitNormal )
{
	local TTPawn A;
	local byte teamNum;

	if( Owner != None && Owner.Owner != None )
	{
		teamNum = TTPawn( Owner.Owner ).GetTeamNum();

		foreach CollidingActors( class 'TTPawn', A, blindDistance )
		{
			if( A.GetTeamNum() != teamNum )
			{
				A.Blind( blindDuration );
			}
		}
	}

	if( ROLE < Role_Authority )
	{
		ServerEMPDetonated();
	}
	else
	{
		EMPDetonated();
	}

	super.Explode( HitLocation, HitNormal );
}

simulated unreliable server function ServerEMPDetonated()
{
	TTWeap_EMPPistol( Owner ).bEMPDetonated = true;
}

function EMPDetonated()
{
	TTWeap_EMPPistol( Owner ).bEMPDetonated = true;
}

DefaultProperties
{
	ProjFlightTemplate=ParticleSystem'TT_Particles.Particles.emp_ball_01'
	ProjExplosionTemplate=ParticleSystem'TT_Particles.Particles.emp_explosion_particle_01'
	Speed=1000
	MaxSpeed=5000
	AccelRate = 1500
	AccelerationRate = 1500
	MaxEffectDistance=7000.0
	bCheckProjectileLight=true
	ProjectileLightClass=class'UTGame.UTShockBallLight'

	MomentumTransfer=20000

	LifeSpan=8.0

	blindDuration = 5.0
	blindDistance = 500

	bCollideWorld=true
	bProjTarget=True

	Begin Object Name=CollisionCylinder
		CollisionRadius=16
		CollisionHeight=16
		AlwaysLoadOnClient=True
		AlwaysLoadOnServer=True
		BlockNonZeroExtent=true
		BlockZeroExtent=true
		BlockActors=true
		CollideActors=true
	End Object

	bNetTemporary=false
	AmbientSound=SoundCue'A_Weapon_ShockRifle.Cue.A_Weapon_SR_AltFireTravelCue'
	ExplosionSound=SoundCue'A_Weapon_ShockRifle.Cue.A_Weapon_SR_AltFireImpactCue'
}

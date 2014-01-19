class BS_Projectile extends Projectile;

var( BSProjectile ) ParticleSystem ProjFlightTemplate;
var( BSProjectile ) SoundCue FlightSound;//Sound to play in flight
var( BSProjectile ) bool bLobbed;

var AudioComponent FlightAudio;

var ParticleSystemComponent	ProjEffects;

var bool bDetonateAtDestination;
var bool bPiercing;
var Vector Destination;
var Vector Start;
var Vector Direction;
var float DistanceToMove;
var int HitCount;

function Vector GetDirection()
{
	return Direction;
}

simulated function PostBeginPlay()
{
	super.PostBeginPlay();

	if( ProjFlightTemplate != none )
	{
		ProjEffects = WorldInfo.MyEmitterPool.SpawnEmitterCustomLifetime(ProjFlightTemplate);
		ProjEffects.SetAbsolute(false, false, false);
		ProjEffects.SetLODLevel(WorldInfo.bDropDetail ? 1 : 0);		
		ProjEffects.bUpdateComponentInTick = true;
		ProjEffects.OnSystemFinished = MyOnParticleSystemFinished;
		AttachComponent( ProjEffects );
	}

	Start = Location;

	FlightAudio = new class'AudioComponent';
	FlightAudio.SoundCue = FlightSound;
	FlightAudio.Play();

	SetCollision( true, false, true );
}

simulated function FireAt( Vector Target, bool bPiercingProjectile, int ProjDamage, optional float Distance )
{
	local BS_Pawn BSPawn;
	local Vector lobbingVelocity;

	BSPawn = BS_Pawn( Owner );

	if( !bLobbed )
	{
		if( BSPawn != none )
		{
			Target.Z = BSPawn.Location.Z;		
		}
		else
		{
			Target.Z = BS_Pawn( Owner.Owner ).Location.Z;
		}
	}

	Destination = Target;

	if( !bPiercingProjectile )
		DistanceToMove = VSize( Destination - Start );
	else
		DistanceToMove = Distance;

	bPiercing = bPiercingProjectile;
	Damage = ProjDamage;

	Direction = Normal( Destination - Start );
	Init( Direction );

	if( bLobbed )
	{
		lobbingVelocity.Z = min( DistanceToMove, 450 ); //TODO Possibly scale based on distance
		lobbingVelocity.X = ( ( Destination.X - Location.X ) * GetGravityZ() ) / ( -2 * lobbingVelocity.Z );
		lobbingVelocity.Y = ( ( Destination.Y - Location.Y ) * GetGravityZ() ) / ( -2 * lobbingVelocity.Z );

		Velocity = lobbingVelocity;
	}
}

simulated function Tick( float DeltaTime )
{
	local BS_Pawn BSPawn;
	local BS_Ability BSAbility;

	if( bLobbed )
	{
		Velocity.Z += GetGravityZ() * DeltaTime;
	}

	if( VSize2D( Location - Start ) >= DistanceToMove )
	{
		BSPawn = BS_Pawn( Owner );
		BSAbility = BS_Ability( Owner );

		if( BSPawn != none )
		{
			BSPawn.TargetHit();
		}
		else
		{
			BSAbility.FireAbility();
		}

		if( ProjEffects != none )
		{
			ProjEffects.DeactivateSystem();
		}
	
		DestoryProjectile();
	}
	else
	{
		if( !FlightAudio.IsPlaying() )
		{
			FlightAudio.Play();
		}
	}

	super.Tick( DeltaTime );
}

simulated function Explode( Vector HitLocation, Vector HitNormal )
{
	local BS_Pawn BSPawn;
	local BS_Ability BSAbility;

	BSPawn = BS_Pawn( Owner );
	BSAbility = BS_Ability( Owner );

	if( BSPawn != none )
	{
		BSPawn.TargetHit();
	}
	else
	{
		BSAbility.FireAbility();
	}

	if( ProjEffects != none )
	{
		ProjEffects.DeactivateSystem();
	}
	
	DestoryProjectile();
}

simulated function ProcessTouch( Actor Other, Vector HitLocation, Vector HitNormal )
{
	local BS_Pawn BSPawn;

	if( bPiercing )
	{
		BSPawn = BS_Pawn( Owner.Owner );

		if( !BS_Pawn( Other ).IsFriendly( BSPawn ) )
			BSPawn.DealDamage( BS_Pawn( Other ), false, Damage, BSPawn.Controller, HitLocation, vect(0,0,0), class'DamageType',, BSPawn );
	}
}

simulated function HitWall( Vector HitNormal, Actor Wall, PrimitiveComponent WallComp )
{
	local BS_Ability BSAbility;

	if( bPiercing )
	{
		if( ProjEffects != none )
		{
			ProjEffects.DeactivateSystem();
		}

		BSAbility = BS_Ability( Owner );

		if( BSAbility != none )
		{
			BSAbility.FireAbility();
		}
	
		DestoryProjectile();
	}
}

function DestoryProjectile()
{
	Destroy();
}

simulated function MyOnParticleSystemFinished(ParticleSystemComponent PSC)
{
	if (PSC == ProjEffects)
	{
		DetachComponent(ProjEffects);
		WorldInfo.MyEmitterPool.OnParticleSystemFinished(ProjEffects);
		ProjEffects = None;
		LifeSpan = 0.01;
	}
}

DefaultProperties
{
	ProjFlightTemplate = ParticleSystem'bs_HudUi.CombatLock.bs_combatLock_HUD_PS'
	Speed = 500
	MaxSpeed = 500
	Damage = 0
	HitCount = 0
	bDetonateAtDestination = false
	bCollideWorld = false
	bPiercing = false
	bLobbed = false
}

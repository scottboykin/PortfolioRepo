class BS_UnitController extends AIController;

var Vector Start;
var Vector Destination;
var Vector NextMoveLocation;
var Vector Direction;
var Vector FocalPoint;
var bool bGhosting;
var float MoveRadius;
var float TimeOutDelay;
var Controller Controller;

var BS_Ability AbilityToFire;
var BS_Pawn unitToAttack;

var bool LeftPathObstructed;

var array< Vector > DetourRoute;

event PostBeginPlay()
{
	super.PostBeginPlay();
}

event Possess( Pawn inPawn, bool bVehicleTransition )
{
	super.Possess( inPawn, bVehicleTransition );
	inPawn.SetMovementPhysics();
	if (Pawn.Physics == PHYS_Walking)
			Pawn.SetPhysics(PHYS_Falling);
}

function UnitKilled() 
{
	local BS_Pawn unit;

	unit = BS_Pawn( Pawn );

	if( unit.bIsNPC )
	{
		BS_AIController( Controller ).RemoveUnit( unit );
	}
	else
	{
		BS_PlayerController( Controller ).RemoveUnit( unit );
		BS_PlayerController( Controller ).NumHerosDied++;
	}

	Destroy();
}

function MoveCommand( Vector Goal, optional BS_Ability AbilityToFireAfterMove = none )
{
	Destination = Goal;
	Start = Pawn.Location;
	AbilityToFire = AbilityToFireAfterMove;

	if( GeneratePathTo( Destination ) )
	{
		if( !BS_GameInfo( WorldInfo.Game ).leeroyJenkinsMode )
		{
			SetTimer( TimeOutDelay,, 'MovementTimedOut' );
		}
		else
		{
			SetTimer( TimeOutDelay * 3.f,, 'MovementTimedOut' );
		}

		if( AbilityToFire != none && !AbilityToFire.RangedAbility )
		{
			MoveRadius = AbilityToFire.Range;
			GotoState( 'MoveThenFireAbility' );
		}
		else
		{
			MoveRadius = BS_Pawn( Pawn ).GetAdjustedMovementRadius(); 
			if( bGhosting )
			{
				Pawn.SetCollision( true, false, true );
			}
			GotoState( 'MoveToFinalDestination' );
		}
	}
	
}

function MoveToAttack( BS_Pawn Target )
{
	Direction = Normal( Pawn.Location - Target.Location );
	Start = Pawn.Location;
	Destination = Target.Location + Direction * ( target.GetCollisionRadius() + Pawn.GetCollisionRadius() );

	unitToAttack = target;

	MoveRadius = BS_Pawn( Pawn ).GetAdjustedMovementRadius();
	
	GeneratePathTo( Destination );

	SetTimer( TimeOutDelay,,'MovementTimedOut');

	GotoState( 'MoveToFinalDestination' );
}

//PH: Used to change the position of the Pawns circle once the move command has completed
function SetSelectedUnitsCirclePos()
{
	local BS_Pawn BSPawn;
	BSPawn = BS_Pawn( Pawn );
	if( BSPawn != none )
		BSPawn.SetPositionOfUnitCircle();
}

function bool GeneratePathTo( Vector Goal, optional float WithinDistance, optional bool bAllowPartialPath )
{
	local bool HasDirectPath;

	HasDirectPath = false;
	LeftPathObstructed = false;

	DetourRoute.Length = 0;
	RouteCache.Length = 0;

	if( IsPointReachable( Goal, false ) )
	{
		HasDirectPath = true;
	}

	else
	{
		BS_GameInfo( WorldInfo.Game ).TurnManager.GetPath( Pawn, Goal, RouteCache );
	}

	return HasDirectPath || RouteCache.Length != 0;
}

function NavigationPoint GetNextNavPoint()
{
	local NavigationPoint NextPoint;
	local int RouteIndex;

	NextPoint = none;

	if( DetourRoute.Length == 0 )
	{
		if( IsPointReachable( Destination, false ) )
		{
			RouteCache.Length = 0;
		}
		else
		{
			for( RouteIndex = RouteCache.Length - 1; RouteIndex >= 0; --RouteIndex )
			{
				if( IsPointReachable( RouteCache[RouteIndex].Location, false ) )
				{
					NextPoint = RouteCache[RouteIndex];
					RouteCache.Length = RouteIndex + 1;
				}
			}
		}
	}

	return NextPoint;
}

function Vector GetNextMoveLocation()
{
	local NavigationPoint NextNavPoint;
	local Vector MoveLocation;

	NextNavPoint = GetNextNavPoint();

	if( DetourRoute.Length != 0 )
	{
		if( HasReachedDestination( DetourRoute[ DetourRoute.Length - 1 ], Pawn.GetCollisionRadius() ) )
		{
			DetourRoute.Length = DetourRoute.Length - 1;
		}

		if( DetourRoute.Length > 0 )
		{
			MoveLocation = DetourRoute[ DetourRoute.Length - 1 ];
		}
	}

	if( DetourRoute.Length == 0 )
	{
		if( NextNavPoint != none )
		{
			MoveLocation = NextNavPoint.Location;		
		}
		else
		{
			MoveLocation = Destination;
		}
	}

	return MoveLocation;
}

function bool IsPointReachable( Vector Point, bool bCollideUnits )
{
	local Actor HitActor;
	local Vector HitLocation, HitNormal;

	//Check if any static actors or blocking volumes are in the way
	foreach TraceActors( class'Actor', HitActor, HitLocation, HitNormal, Point, Pawn.Location, Pawn.GetCollisionExtent() )
	{
		if( bCollideUnits && BS_Pawn( HitActor ) != none )
		{
			return false;
		}

		if( HitActor != none && ( StaticMeshActor( HitActor ) != none || BlockingVolume( HitActor ) != none || InterpActor( HitActor ) != none ) )
		{
			return false;
		}
	}

	return true;
}

function bool IsTargetReachable( Actor Target, bool bCollideUnits )
{
	local Actor HitActor;
	local Vector HitLocation, HitNormal;

	//Check if any static actors or blocking volumes are in the way
	foreach TraceActors( class'Actor', HitActor, HitLocation, HitNormal, Target.Location, Pawn.Location, Pawn.GetCollisionExtent() )
	{
		if( HitActor != none )
		{
			if( bCollideUnits && ( BS_Pawn( HitActor ) != none && BS_Pawn( HitActor ) != target && Normal( Target.Location - Pawn.Location ) dot HitNormal < -0.85f ) )
				return false;

			else if( StaticMeshActor( HitActor ) != none || BlockingVolume( HitActor ) != none || InterpActor( HitActor ) != none )
				return false;
		}
	}

	return true;
}

function bool HasReachedDestination( Vector Goal, float DestinationOffset )
{
	return VSize2D( Pawn.Location - Goal ) < DestinationOffset;
}

function RanIntoUnit( Actor Unit, PrimitiveComponent OtherComp, Vector HitNormal )
{
	local Vector PushMomentum;
	local float PushStrength;

	PushStrength = 5000;

	if( IsInState( 'Moving' ) && HitNormal dot Direction < -0.5f )
	{
		if( VSize2D( Destination - Pawn.Location ) <= ( Pawn.GetCollisionRadius() * 2.05f ) )
		{
			Pawn.Acceleration = vect( 0,0,0 );
			bPreciseDestination = false;
			GotoState( 'Idle' );
		}
		else //Try to path around the pawns
		{
			Detour( Unit, HitNormal );
		}
	}
	else if( IsInState( 'MoveThenFireAbility' ) )
	{
		if( VSize2D( Destination - Pawn.Location ) <= ( Pawn.GetCollisionRadius() * 2.05f ) )
		{
			Pawn.Acceleration = vect( 0,0,0 );
			bPreciseDestination = false;
			GotoState( 'Idle' );
		}
		else if( HitNormal dot Direction < -0.9f )
		{
			PushMomentum = ( Direction cross vect( 0,0,1 ) ) - Direction;
			PushMomentum.Z = 0;
			PushMomentum = Normal( PushMomentum );

			if( Unit.FastTrace( Unit.Location + ( PushMomentum * PushStrength ) ) )
			{
				PushMomentum *= PushStrength;
				Unit.TakeDamage( 0, self, Unit.Location, PushMomentum, class'DamageType' );
			}
			else
			{
				PushMomentum *= -PushStrength;
				Unit.TakeDamage( 0, self, Unit.Location, PushMomentum, class'DamageType' );
			}
		}
		else
		{
			PushMomentum = -HitNormal;
			PushMomentum *= PushStrength;
			Unit.TakeDamage( 0, self, Unit.Location, PushMomentum, class'DamageType' );
		}
	}
}

function Detour( Actor Unit, Vector HitNormal )
{
	local Vector LeftDetourPoint, RightDetourPoint, Up;

	Up = vect( 0, 0, 1 );
	LeftDetourPoint = vect( 0, 0, 0 );
	RightDetourPoint = vect( 0, 0, 0 );

	DetourRoute.Length = 0;

	DetourRoute.AddItem( Pawn.Location - ( Direction * ( Pawn.GetCollisionRadius() * 1.25f ) ) );

	LeftDetourPoint = Unit.Location + ( Direction cross up ) * Pawn.GetCollisionRadius() * 2.5f;
	LeftDetourPoint.Z = Pawn.Location.Z;

	RightDetourPoint = Unit.Location + ( -Direction cross up ) * Pawn.GetCollisionRadius() * 2.5f;
	RightDetourPoint.Z = Pawn.Location.Z;

	if( !LeftPathObstructed && IsPointReachable( LeftDetourPoint, false ) )
	{
		DetourRoute.InsertItem( 0, LeftDetourPoint );
		Pawn.Acceleration = vect( 0,0,0 );
		StopLatentExecution();
		GotoState( 'MoveToFinalDestination' );
	}
	else if( IsPointReachable( RightDetourPoint, false ) )
	{
		LeftPathObstructed = true;
		DetourRoute.InsertItem( 0, RightDetourPoint );
		Pawn.Acceleration = vect( 0,0,0 );
		StopLatentExecution();
		GotoState( 'MoveToFinalDestination' );
	}
	else
	{
		Pawn.Acceleration = vect( 0,0,0 );
		bPreciseDestination = false;
		GotoState( 'Idle' );
	}
}

function MovementTimedOut()
{
	Pawn.Acceleration = vect(0,0,0);
	GotoState( 'Idle' );
}

auto state Idle
{

simulated function Tick( float DeltaTime )
{
	SetSelectedUnitsCirclePos();
}

simulated function BeginState( Name PreviousStateName )
{
	if( bGhosting )
	{
		if( Pawn != none )
		{
			Pawn.SetCollision( true, true, true );

			if( BS_Pawn( Pawn ).actionPoints == 0 )
			{
				BS_Pawn( Pawn ).StoppedMoving();
			}
		}
	}
}

Begin:
	
	if( VSize( BS_Pawn( Pawn ).CurrentDestination - BS_Pawn( Pawn ).Location ) <= BS_Pawn( Pawn ).AttackRadius )
	{
	//	BS_Pawn( Pawn ).WorldState.AddOrModifyBoolState( IS_IN_RANGE, true );
		BS_Pawn( Pawn ).WorldState.AddOrModifyBoolState( HAS_DESTINATION, false );
		BS_Pawn( Pawn ).WorldState.AddOrModifyBoolState( HAS_REACHED_DESTINATION, false );
	}
	else
	{
	//	BS_Pawn( Pawn ).WorldState.AddOrModifyBoolState( IS_IN_RANGE, false );
	}
	
    if( VSize2D( Pawn.Location - Start ) <= 1.0f && ( VSize2D( Pawn.Location - Destination ) > Pawn.GetCollisionRadius() ||
									( unitToAttack != none && BS_Pawn( Pawn ).ValidTarget( unitToAttack ) ) ) )
	{
		if( AbilityToFire != none || unitToAttack != none )
		{
			AbilityToFire = none;
			unitToAttack = none;
		}
	}

	else
	{
		if( AbilityToFire != none )
		{
			AbilityToFire.UseAbility( AbilityToFire.targetedPawn, AbilityToFire.targetedLocation );
			AbilityToFire = none;
		}

		else if( unitToAttack != none )
		{
			BS_Pawn( Pawn ).AttackTarget( unitToAttack );
			unitToAttack = none;
		}
	}
}

state MoveToFinalDestination
{
	simulated function BeginState( name PreviousStateName )
	{
	    if( !HasReachedDestination( Destination, Pawn.GetCollisionRadius() ) )
		{
			NextMoveLocation = GetNextMoveLocation();
			GotoState( 'Moving' );
		}

		else
		{
			StopLatentExecution();
			Pawn.Acceleration = vect(0,0,0);
			GotoState( 'Idle' );
		}
	}
}

state Moving
{

/***************DUMB PATHING*******************/
//simulated function Tick( float DeltaTime )
//{
//	if( VSize( Pawn.Location - Start ) >= MoveRadius )
//	{
//		Pawn.Acceleration = vect( 0, 0, 0 );
//		GoToState( 'Idle' );
//	}
//}

//Begin:
//	Moveto( Destination );
//	GotoState( 'Idle' );
/*********************************************/

	simulated function Tick( float DeltaTime )
	{
		local int i;

		if( VSize( Pawn.Location - Start ) >= MoveRadius && !BS_GameInfo( WorldInfo.Game ).leeroyJenkinsMode )
		{
			bPreciseDestination = false;
			StopLatentExecution();
			Pawn.Acceleration = vect( 0, 0, 0 );
			GotoState( 'Idle' );
		}

		else
		{
			if( BS_GameInfo( WorldInfo.Game ).debugPathing )
			{
				if( DetourRoute.Length == 0 )
				{
					for( i = 0; i < RouteCache.Length; ++i )
					{
						DrawDebugSphere( RouteCache[i].Location, 25.f, 10, 255, 255, 0 );
					}
				}
				else
				{
					for( i = 0; i < DetourRoute.Length; ++i )
					{
						DrawDebugSphere( DetourRoute[i], 25.f, 10, 0, 255, 0 );
					}
				}

				DrawDebugSphere( Destination, 25.f, 10, 255, 0, 0 );
			}
		
			StopLatentExecution();
			Acceleration = vect( 0,0,0 );
			GotoState( 'MoveToFinalDestination' );
		}
	}

Begin:

	NextMoveLocation.Z = Pawn.Location.Z;

	FocalPoint = NextMoveLocation;
	Direction = FocalPoint - Pawn.Location;
	Direction.Z = 0;
	Direction = Normal( Direction );
	BS_Pawn( Pawn ).CurrentRotation = Rotator( Direction );
	
	Pawn.Acceleration = Direction * BS_Pawn( Pawn ).UnitVelocityMag/0.2f;
	MoveTo( NextMoveLocation );
	
	GotoState( 'MoveToFinalDestination' );

}

state MoveThenFireAbility
{

	simulated function Tick( float DeltaTime )
	{
		if( VSize2D( Destination - Pawn.Location ) < BS_Pawn( Pawn ).GetAdjustedAttackRadius() )
		{
			StopLatentExecution();
			Pawn.Acceleration = vect(0,0,0);
			GotoState( 'Idle' );
		}
	}

Begin:

	FocalPoint = Destination;
	Direction = FocalPoint - Pawn.Location;
	Direction.Z = 0;
	Direction = Normal( Direction );
	BS_Pawn( Pawn ).CurrentRotation = Rotator( Direction );
	
	Pawn.Acceleration = Direction * BS_Pawn( Pawn ).UnitVelocityMag/0.2f;
	MoveTo( Destination );
	GotoState( 'Idle' );

}

DefaultProperties
{
	AbilityToFire = none
	bGhosting = true
	TimeOutDelay = 5.f
}

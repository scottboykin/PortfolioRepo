class BS_BouncingAbility extends BS_TargetedAbility;

var( BouncingAbility ) float BounceDegree; //This determines the degree of the bounce. The smaller the degree the straighter the bounce
var( BouncingAbility ) float PercentOfAttackPower;
var( BouncingAbility ) int MaximumNumberOfBounces; //negative is infinite
var( BouncingAbility ) bool CanOnlyHitSameUnitOnce;
var( BouncingAbility ) float PercentReductionPerBounce;
var( BouncingAbility ) float MinPercentOfPower;

var BS_Projectile firedProj;

var float BounceRadian;
var int CurrentNumberOfBounces;
var array< BS_Pawn > UnitsHit;

simulated function PostBeginPlay()
{
	super.PostBeginPlay();
	BounceRadian = BounceDegree * PI / 180;
}

function int GetAdjustedDamage()
{
	local float CurrentPercentOfPower;
	CurrentPercentOfPower = 1.0 - PercentReductionPerBounce * CurrentNumberOfBounces;

	if( MinPercentOfPower > CurrentPercentOfPower )
	{
		BS_Pawn( Owner ).GetAdjustedAttackPower() * PercentOfAttackPower * MinPercentOfPower;
	}
	else
	{
		return BS_Pawn( Owner ).GetAdjustedAttackPower() * PercentOfAttackPower * CurrentPercentOfPower;
	}

	return 0;
}

function UseAbility( optional BS_Pawn targetPawn, optional Vector targetLocation )
{
	CurrentNumberOfBounces = 0;
	UnitsHit.Length = 0;

	SetLocation( Owner.Location );

	super.UseAbility( targetPawn, targetLocation );
}

function FireAbility()
{
	local array< BS_Pawn > UnitsInBounceRadius; 
	local BS_Pawn ClosestUnitInRange;
	local Bs_Pawn UnitInRadius;

	BS_Pawn( Owner ).DealDamage( targetedPawn, false, GetAdjustedDamage(), BS_Pawn( Owner ).Controller, targetedPawn.Location, vect(0,0,0), class'DamageType' );

	++CurrentNumberOfBounces;
	UnitsHit.AddItem( targetedPawn );

	SpawnParticleSystemOnTarget();

	if( AbilitySound != none )
	{
		PlaySound( AbilitySound );
	}

	if( CurrentNumberOfBounces < MaximumNumberOfBounces || MaximumNumberOfBounces < 0 )
	{
		ClosestUnitInRange = none;
		UnitsInBounceRadius = GetUnitsInEffectRadius( targetedPawn.Location );

		foreach UnitsInBounceRadius( UnitInRadius )
		{
			if( ValidTarget( UnitInRadius ) )
			{
				if( ClosestUnitInRange == none || Vsize2D( targetedPawn.Location - ClosestUnitInRange.Location ) > Vsize2D( targetedPawn.Location - UnitInRadius.Location ) )
				{
					ClosestUnitInRange = UnitInRadius;
				}
			}
		}

		if( ClosestUnitInRange != none )
		{
			SetLocation( targetedPawn.Location );
			targetedPawn = ClosestUnitInRange;
			SpawnProjectile();
		}
		else
		{
			BS_Pawn( Owner ).bAbilityStillFiring = false;
			FiredProj = none;
		}
	}
	else
	{
		BS_Pawn( Owner ).bAbilityStillFiring = false;
		FiredProj = none;
	}
}

function SpawnProjectile()
{
	if( FiredProj != none )
	{
		FiredProj.Destroy();
		FiredProj = none;
	}
	else
	{
		if( FireSound != none )
		{
			PlaySound( FireSound );
		}
	}

	FiredProj = Spawn( class'BS_Projectile', self,, Location,, ProjArchetypeToFire );
	FiredProj.FireAt( targetedPawn.Location, false, 0, GetAbilityRange() );
}

function bool ValidTarget( BS_Pawn Target )
{
	local bool valid;
	local Vector previousDirection, newDirection;
	local float angle;

	valid = !CanOnlyHitSameUnitOnce || ( CanOnlyHitSameUnitOnce && UnitsHit.Find( Target ) == -1 );
	
	if( !valid )
	{
		return false;
	}

	valid = !BS_Pawn( Owner ).IsFriendly( Target );

	if( !valid )
	{
		return false;
	}

	previousDirection = FiredProj.Direction;
	newDirection = Normal( Target.Location - targetedPawn.Location );
	
	angle = ( ( previousDirection dot newDirection ) + 1 ) * PI;

	valid = angle >= ( 2 * PI ) - BounceRadian;

	return valid; 
}

function bool CanUseAbility( optional BS_Pawn targetPawn, optional Vector targetLocation )
{
	local bool canUseAbility;
	local Vector MoveLocation;

	MoveLocation = targetLocation;
	MoveLocation.Z = Owner.Location.Z;

	canUseAbility = true;

	canUseAbility = RangedAbility || BS_Pawn( Owner ).IsPointReachable( MoveLocation, false );
	if( !canUseAbility ) 
		return canUseAbility;

	canUseAbility = canUseAbility && super.CanUseAbility( targetPawn, targetLocation );
	if( !canUseAbility ) 
		return canUseAbility;

	canUseAbility = canUseAbility && TargetInRange( none, targetLocation );
	
	return canUseAbility;
}

DefaultProperties
{
	EffectRadius = 150
	BounceDegree = 180
	PercentOfAttackPower = 1.0
	MaximumNumberOfBounces = -1
	PercentReductionPerBounce = 0.1
	MinPercentOfPower = 0.25
	CurrentNumberOfBounces = 0
	CanOnlyHitSameUnitOnce = true
	InstantCast = false
	TargetingPositionType = TARGET_POSITION_UNIT
}

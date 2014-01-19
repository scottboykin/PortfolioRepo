class BS_AbilityTeleportStrike extends BS_Ability;

function UseAbility( optional BS_Pawn targetPawn, optional Vector targetLocation )
{
	BS_Pawn( Owner ).bAbilityStillFiring = true;

	if( targetPawn == none || ( targetPawn != none && targetPawn.TeamNumber != BS_Pawn( Owner ).TeamNumber ) )
		BS_Pawn( Owner ).CurrentTarget = targetPawn;

	targetedPawn = targetPawn;
	targetedLocation = targetLocation;

	if( PercentOfDamageAsHealthCost > 0 )
	{
		Owner.TakeDamage( GetAdjustedDamage() * PercentOfDamageAsHealthCost, BS_Pawn( Owner ).Controller, Owner.Location, vect(0,0,0), class'DamageType',, Owner );
	}

	if( BS_Pawn( Owner ).combatLockedWith != none )
	{
		BS_Pawn( Owner ).combatLockedWith.BreakCombatLock();
	}
	BS_Pawn( Owner ).BreakCombatLock();

	if( ChargesPerMatch > 0 )
		--ChargesPerMatch;

	if( PlayWindUpSound && WindUpSound != none )
	{
		PlaySound( WindUpSound );
	}

	if( PlayWindUpPS && AbilityPSOnCasterTemplate != none )
	{
		SpawnParticleSystemOnCaster();
	}

	BS_Pawn( Owner ).PlayAbilityAnim( AbilityIndex );
		
	if( WindUpDelay == 0 )
		FireAbility();
	else
	{
		SetTimer( WindUpDelay,, 'FireAbility' );
	}
}

function FireAbility()
{
	local int adjustedDamage;
	local Vector Direction, TeleportLocation;
	local array< BS_Pawn > UnitsInRadius;
	local BS_Pawn Unit;

	Direction = targetedPawn.Location - Owner.Location;
	Direction.Z = 0;
	Direction = Normal( Direction );
	TeleportLocation = targetedPawn.Location + Direction * ( BS_Pawn( Owner ).GetCollisionRadius() + targetedPawn.GetCollisionRadius() + 10 );
	EffectRadius = BS_Pawn( Owner ).GetCollisionRadius();
	UnitsInRadius = self.GetUnitsInEffectRadius( TeleportLocation );

	foreach UnitsInRadius( Unit )
	{
		if( Unit == Owner || Unit == targetedPawn )
			continue;

		Unit.TakeDamage( 0, Controller( Owner.Owner ), targetedPawn.Location, Direction * 30000, class'DamageType' );
	}

	BS_Pawn( Owner ).SetCollision( false, false, true );
	Owner.SetLocation( TeleportLocation );
	SetTimer( 1.0 );
	BS_Pawn( Owner ).LookAtLocation( targetedPawn.Location );
	SpawnParticleSystemOnCaster();

	adjustedDamage = GetAdjustedDamage();

	BS_Pawn( Owner ).DealDamage( targetedPawn, true, adjustedDamage, Controller( Owner.Owner ), targetedPawn.Location, vect(0,0,0), class'DamageType',, Owner );

	super.FireAbility();
}

function bool CanUseAbility( optional BS_Pawn targetPawn, optional Vector targetLocation )
{
	local bool canUseAbility;

	canUseAbility = true;

	canUseAbility = canUseAbility && targetPawn != none;
	if( !canUseAbility ) 
		return canUseAbility;

	canUseAbility = canUseAbility && super.CanUseAbility( targetPawn, targetLocation );
	if( !canUseAbility ) 
		return canUseAbility;

	canUseAbility = canUseAbility && ( TargetType == TARGET_TYPE_ALL || targetPawn.teamNumber != BS_Pawn( Owner ).teamNumber && TargetType == TARGET_TYPE_ENEMIES ||
										targetPawn.teamNumber == BS_Pawn( Owner ).teamNumber && TargetType == TARGET_TYPE_ALLIES );
	if( !canUseAbility ) 
		return canUseAbility;

	canUseAbility = canUseAbility && TargetInRange( targetPawn, targetPawn.Location );
	if( !canUseAbility ) 
		return canUseAbility;

	return canUseAbility;
}

event Timer()
{  
	BS_Pawn( Owner ).SetCollision( true, true, true );
}

DefaultProperties
{
}

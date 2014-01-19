class BS_PiercingAbility extends BS_Ability;

function SpawnProjectile()
{
	Spawn( class'BS_Projectile', self,, Owner.Location,, ProjArchetypeToFire ).FireAt( targetedLocation, true, GetAdjustedDamage(), GetAbilityRange() );
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

	canUseAbility = canUseAbility && super.CanUseAbility( targetPawn, targetLocation );;
	if( !canUseAbility ) 
		return canUseAbility;

	canUseAbility = canUseAbility && TargetInRange( none, targetLocation );
	
	return canUseAbility;
}

DefaultProperties
{
	AbilityType = ABILITY_TYPE_TARGETED
	TargetingPositionType = TARGET_POSITION_VECTOR
	AbilityAnimName=bs_chara_attack
}

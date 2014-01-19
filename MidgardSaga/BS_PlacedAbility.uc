class BS_PlacedAbility extends BS_Ability;

var( PlacedAbility ) float PercentOfDamageAtCenter;
var( PlacedAbility ) float PercentOfDamageAtEdge;

simulated function PostBeginPlay()
{
	super.PostBeginPlay();
}

function FireAbility()
{
	local Vector momentum;
	local Vector CollisionOffset;
	local array< BS_Pawn > HitUnits;
	local BS_Pawn unit;
	local int friendlyFireDamage;
	local int adjustedDamage;
	local float distanceFromCenter, percentOfDamage;

	adjustedDamage = GetAdjustedDamage();

	if( FriendlyFirePercentDamage < 0 )
		friendlyFireDamage = Damage * FriendlyFirePercentDamage;
	else
		friendlyFireDamage = adjustedDamage * FriendlyFirePercentDamage;

	if( ImpulseLocation == IMPULSE_FROM_MOUSE )
	{
		SetLocation( targetedLocation );
	}
	else
	{
		SetLocation( Owner.Location );
	}

	if( GetEffectRadius() > 0 )
	{
		HitUnits = GetUnitsInEffectRadius( targetedLocation );
		foreach HitUnits( Unit )
		{
			CollisionOffset = Normal( targetedLocation - unit.Location ) * unit.GetCollisionRadius();
			distanceFromCenter = max( 0, VSize2D( targetedLocation - ( unit.Location + CollisionOffset ) ) ) / GetEffectRadius();
			percentOfDamage = PercentOfDamageAtCenter * ( 1 - distanceFromCenter ) + PercentOfDamageAtEdge * distanceFromCenter;
			
			if( percentOfDamage > 1.0 )
			{
				percentOfDamage = 1.0;
			}

			momentum = Normal( unit.Location - Location ) * Impulse * MomentumBias;

			if( unit == BS_Pawn( Owner )  && ( TargetType == TARGET_TYPE_ALLIES || TargetType == TARGET_TYPE_ALL ) )
			{
				unit.TakeDamage( friendlyFireDamage * percentOfDamage, Controller( Owner.Owner ), unit.Location, momentum * int( ImpulseAffectsSelf ), class'DamageType',, Owner );

				if( StatusEffectArchetype != none )
				{
					unit.ApplyStatusEffect( Spawn( StatusEffectArchetype.Class, Owner,,,, StatusEffectArchetype ) );
				}

				if( ImpulseAffectsSelf && Impulse != 0 )
				{
					if( unit.combatLockedWith != none )
					{
						unit.combatLockedWith.BreakCombatLock();
					}

					unit.BreakCombatLock();
				}
			}

			else if( unit.teamNumber == BS_Pawn( Owner ).teamNumber && ( TargetType == TARGET_TYPE_ALLIES || TargetType == TARGET_TYPE_ALL ) )
			{
				unit.TakeDamage( friendlyFireDamage * percentOfDamage, Controller( Owner.Owner ), unit.Location, momentum * int( ImpulseAffectsAllies ), class'DamageType',, Owner );

				if( StatusEffectArchetype != none )
				{
					unit.ApplyStatusEffect( Spawn( StatusEffectArchetype.Class, Owner,,,, StatusEffectArchetype ) );
				}

				if( ImpulseAffectsAllies && Impulse != 0 )
				{
					if( unit.combatLockedWith != none )
					{
						unit.combatLockedWith.BreakCombatLock();
					}

					unit.BreakCombatLock();
				}
			}
			else if( unit.teamNumber != BS_Pawn( Owner ).teamNumber && ( TargetType == TARGET_TYPE_ENEMIES || TargetType == TARGET_TYPE_ALL ) )
			{
				BS_Pawn( Owner ).DealDamage( unit, !RangedAbility, adjustedDamage * percentOfDamage, Controller( Owner.Owner ), unit.Location, momentum * int( ImpulseAffectsEnemies ), class'DamageType',, Owner );

				if( StatusEffectArchetype != none )
				{
					unit.ApplyStatusEffect( Spawn( StatusEffectArchetype.Class, Owner,,,, StatusEffectArchetype ) );
				}

				if( ImpulseAffectsEnemies && Impulse != 0 )
				{
					if( unit.combatLockedWith != none )
					{
						unit.combatLockedWith.BreakCombatLock();
					}

					unit.BreakCombatLock();
				}
			}			
		}
	}

	else
	{
		//No Idea what a non AoE placed ability would do...

		//momentum = Normal( target.Location - Location ) * Impulse * MomentumBias;

		//target.TakeDamage( Damage, BS_PlayerController( Owner.Owner ), target.Location, momentum, class'DamageType',, Owner );
		//if( Impulse != 0 )
		//{
		//	target.BreakCombatLock();
		//	BS_Pawn(Owner).BreakCombatLock();
		//}
	}	

	super.FireAbility();
}

function bool CanUseAbility( optional BS_Pawn targetPawn, optional Vector targetLocation = vect( -5000, -5000, -5000 ) )
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
	AbilityType = ABILITY_TYPE_PLACED
	TargetingPositionType = TARGET_POSITION_VECTOR
	AbilityAnimName=bs_chara_attack

	PercentOfDamageAtCenter = 1.f
	PercentOfDamageAtEdge = 1.f
}

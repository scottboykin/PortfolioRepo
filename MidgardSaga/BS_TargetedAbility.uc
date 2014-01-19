class BS_TargetedAbility extends BS_Ability;

var( TargetedAbility ) float LifeStealPercent;

function FireAbility()
{
	local Vector momentum;
	local array< BS_Pawn > HitUnits;
	local BS_Pawn unit;
	local int adjustedDamage;
	local int friendlyFireDamage;

	adjustedDamage = GetAdjustedDamage();

	if( FriendlyFirePercentDamage < 0 )
		friendlyFireDamage = Damage * FriendlyFirePercentDamage;
	else
		friendlyFireDamage = adjustedDamage * FriendlyFirePercentDamage;

	if( ImpulseLocation == IMPULSE_FROM_MOUSE )
	{
		SetLocation( targetedPawn.Location );
	}
	else
	{
		SetLocation( Owner.Location );
	}

	if( !RangedAbility && BS_Pawn( Owner ).bIsMeleeUnit )
	{
		BS_Pawn( Owner ).InitiateCombatLock( targetedPawn );
	}

	if( GetEffectRadius() > 0 )
	{
		HitUnits = GetUnitsInEffectRadius( targetedPawn.Location );
		foreach HitUnits( Unit )
		{
			momentum = Normal( unit.Location - Location ) * Impulse * MomentumBias;

			if( unit == BS_Pawn( Owner ) && ( TargetType == TARGET_TYPE_ALLIES || TargetType == TARGET_TYPE_ALL ) )
			{
				unit.TakeDamage( friendlyFireDamage, Controller( Owner.Owner ), unit.Location, momentum * int( ImpulseAffectsSelf ), class'DamageType',, Owner );

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
				unit.TakeDamage( friendlyFireDamage, Controller( Owner.Owner ), unit.Location, momentum * int( ImpulseAffectsAllies ), class'DamageType',, Owner );

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
				BS_Pawn( Owner ).DealDamage( unit, !RangedAbility, adjustedDamage, Controller( Owner.Owner ), unit.Location, momentum * int( ImpulseAffectsEnemies ), class'DamageType',, Owner );

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
		momentum = Normal( targetedPawn.Location - Location ) * Impulse * MomentumBias;
		
		if( targetedPawn == BS_Pawn( Owner ) && ( TargetType == TARGET_TYPE_ALLIES || TargetType == TARGET_TYPE_ALL ) )
		{
			targetedPawn.TakeDamage( friendlyFireDamage, Controller( Owner.Owner ), targetedPawn.Location, momentum * int( ImpulseAffectsSelf ), class'DamageType',, Owner );

			if( StatusEffectArchetype != none )
			{
				targetedPawn.ApplyStatusEffect( Spawn( StatusEffectArchetype.Class, Owner,,,, StatusEffectArchetype ) );
			}

			if( ImpulseAffectsSelf && Impulse != 0 )
			{
				if( targetedPawn.combatLockedWith != none )
				{
					targetedPawn.combatLockedWith.BreakCombatLock();
				}

				targetedPawn.BreakCombatLock();
			}
		}

		else if( targetedPawn.teamNumber == BS_Pawn( Owner ).teamNumber && ( TargetType == TARGET_TYPE_ALLIES || TargetType == TARGET_TYPE_ALL ) )
		{
			targetedPawn.TakeDamage( friendlyFireDamage, Controller( Owner.Owner ), targetedPawn.Location, momentum * int( ImpulseAffectsAllies ), class'DamageType',, Owner );

			if( StatusEffectArchetype != none )
			{
				targetedPawn.ApplyStatusEffect( Spawn( StatusEffectArchetype.Class, Owner,,,, StatusEffectArchetype ) );
			}

			if( ImpulseAffectsAllies && Impulse != 0 )
			{
				if( targetedPawn.combatLockedWith != none )
				{
					targetedPawn.combatLockedWith.BreakCombatLock();
				}

				targetedPawn.BreakCombatLock();
			}
		}
		else if( TargetType == TARGET_TYPE_ENEMIES || TargetType == TARGET_TYPE_ALL )
		{
			BS_Pawn( Owner ).DealDamage( targetedPawn, !RangedAbility, adjustedDamage, Controller( Owner.Owner ), targetedPawn.Location, momentum * int( ImpulseAffectsEnemies ), class'DamageType',, Owner );

			if( StatusEffectArchetype != none )
			{
				targetedPawn.ApplyStatusEffect( Spawn( StatusEffectArchetype.Class, Owner,,,, StatusEffectArchetype ) );
			}

			if( ImpulseAffectsEnemies && Impulse != 0 )
			{
				if( targetedPawn.combatLockedWith != none )
				{
					targetedPawn.combatLockedWith.BreakCombatLock();
				}

				targetedPawn.BreakCombatLock();
			}

			if( ImpulseAffectsSelf && Impulse > 0 )
			{
				momentum = Normal( Location - targetedPawn.Location ) * Impulse * MomentumBias;

				BS_Pawn( Owner ).TakeDamage( 0, BS_Pawn( Owner ).Controller, Owner.Location, momentum, class'DamageType',, Owner );
			}
		}
	}

	if( LifeStealPercent > 0 )
	{
		Owner.TakeDamage( -GetAdjustedDamage() * LifeStealPercent, BS_Pawn( Owner ).Controller, Owner.Location, vect(0,0,0), class'DamageType',, Owner );
	}

	super.FireAbility();
}

function int GetLifeSteal()
{
	return GetAdjustedDamage() * LifeStealPercent;
}

function bool CanUseAbility( optional BS_Pawn targetPawn, optional Vector targetLocation )
{
	local bool canUseAbility;

	canUseAbility = true;

	canUseAbility = RangedAbility || ( targetPawn != none && BS_Pawn( Owner ).IsTargetReachable( targetPawn, false ) );
	if( !canUseAbility ) 
		return canUseAbility;

	canUseAbility = canUseAbility && super.CanUseAbility( targetPawn, targetLocation );
	if( !canUseAbility ) 
		return canUseAbility;

	canUseAbility = canUseAbility && targetPawn != none;
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

DefaultProperties
{
	AbilityType = ABILITY_TYPE_TARGETED
	AbilityAnimName=bs_chara_attack
	LifeStealPercent = 0.0
}

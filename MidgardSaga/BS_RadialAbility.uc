class BS_RadialAbility extends BS_Ability;

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

	SetLocation( Owner.Location );

	if( GetEffectRadius() > 0 )
	{
		HitUnits = GetUnitsInEffectRadius( Owner.Location );
		foreach HitUnits( Unit )
		{
			momentum = Normal( unit.Location - Location ) * Impulse * MomentumBias;

			if( unit == Owner && ( TargetType == TARGET_TYPE_ALLIES || TargetType == TARGET_TYPE_ALL ) )
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

			if( unit.teamNumber == BS_Pawn( Owner ).teamNumber && ( TargetType == TARGET_TYPE_ALLIES || TargetType == TARGET_TYPE_ALL ) )
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
				
				if( AggroHitEnemies )
				{
					unit.CurrentTarget = BS_Pawn( Owner );
				}
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
		Owner.TakeDamage( friendlyFireDamage, Controller( Owner.Owner ), Owner.Location, momentum, class'DamageType',, Owner );

		if( StatusEffectArchetype != none )
		{
			BS_Pawn( Owner ).ApplyStatusEffect( Spawn( StatusEffectArchetype.Class, Owner,,,, StatusEffectArchetype ) );
		}
	}

	super.FireAbility();
}

DefaultProperties
{
	AbilityType = ABILITY_TYPE_RADIAL
	AbilityAnimName=bs_chara_attack
	InstantCast = true
}

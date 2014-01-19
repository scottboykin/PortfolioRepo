class BS_AttackOverrideAbility extends BS_Ability;

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
			if( unit == Owner )
				continue;

			momentum = Normal( unit.Location - Location ) * Impulse * MomentumBias;

			if( unit.teamNumber == BS_Pawn( Owner ).teamNumber )
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
			else
			{
				BS_Pawn( Owner ).DealDamage( unit, true, adjustedDamage, Controller( Owner.Owner ), unit.Location, momentum * int( ImpulseAffectsEnemies ), class'DamageType',, Owner );

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
		BS_Pawn( Owner ).DealDamage( targetedPawn, true, adjustedDamage, Controller( Owner.Owner ), unit.Location, momentum * int( ImpulseAffectsEnemies ), class'DamageType',, Owner );

		if( StatusEffectArchetype != none )
		{
			targetedPawn.ApplyStatusEffect( Spawn( StatusEffectArchetype.Class, Owner,,,, StatusEffectArchetype ) );
		}
	}

	super.FireAbility();
}

function bool CanUseAbility( optional BS_Pawn targetPawn, optional Vector targetLocation )
{
	return false;
}

function bool Clickable()
{
	return false;
}

DefaultProperties
{
	AbilityType = ABILITY_TYPE_ATTACKOVERRIDE
	AbilityAnimName=bs_chara_attack
	InstantCast = true
}

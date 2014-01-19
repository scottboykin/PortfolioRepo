class BS_AttribBoostAbility extends BS_Ability;

enum ApplyTypes
{
	PASSIVE,
	STANCE
};

var( AttributeBoost ) ApplyTypes ApplyType;
var( AttributeBoost ) bool DisablesOtherAbilities;

var bool bInStance;
var BS_StatusEffect StanceEffect;

function simulated PostBeginPlay()
{
	if( StatusEffectArchetype != none )
	{
		if( ApplyType == PASSIVE )
		{
			BS_Pawn( Owner ).ApplyStatusEffect( Spawn( StatusEffectArchetype.Class, Owner,,,, StatusEffectArchetype ) );
			AbilityType = ABILITY_TYPE_PASSIVE;
		}
		else if( ApplyType == STANCE )
		{
			self.InstantCast = true;
		}
	}
	else
	{
		`log( Name $ " WAS NOT GIVEN A STATUS EFFECT!" );
	}

	super.PostBeginPlay();
}

function FireAbility()
{
	local int i;
	local BS_Pawn BSOwner;

	if( ApplyType == STANCE )
	{
		BSOwner = BS_Pawn( Owner );
		
		if( bInStance )
		{
			BSOwner.RemoveStatusEffect( StanceEffect );
			StanceEffect.KillPS();
			StanceEffect.Destroy();
			StanceEffect = none;

			if( DisablesOtherAbilities )
			{
				for( i = 0; i < BSOwner.Abilities.Length; ++i )
				{
					BSOwner.Abilities[i].bEnabled = true;
				}
			}

			bInStance = false;
		}
		else
		{
			if( StanceEffect != none )
			{
				BSOwner.RemoveStatusEffect( StanceEffect );
				StanceEffect.KillPS();
				StanceEffect.Destroy();
				StanceEffect = none;
			}
			StanceEffect = Spawn( StatusEffectArchetype.Class, Owner,,,, StatusEffectArchetype );
			BSOwner.ApplyStatusEffect( StanceEffect );

			if( DisablesOtherAbilities )
			{
				for( i = 0; i < BSOwner.Abilities.Length; ++i )
				{
					if( BSOwner.Abilities[i] == self || !BSOwner.Abilities[i].Clickable() )
						continue;

					BSOwner.Abilities[i].bEnabled = false;
				}
			}
			bInStance = true;
		}
	}

	else
	{
		targetedPawn.ApplyStatusEffect( Spawn( StatusEffectArchetype.Class, Owner,,,, StatusEffectArchetype ) );
	}

	super.FireAbility();
}

function bool CanUseAbility( optional BS_Pawn targetPawn, optional Vector targetLocation )
{
	local bool useAbility;

	useAbility = super.CanUseAbility();

	switch( ApplyType )
	{
		case PASSIVE:
			useAbility = false;
			break;

		case STANCE:
			break;
	}

	return useAbility;
}

function bool Clickable()
{
	return super.Clickable() && ApplyType != PASSIVE;
}

DefaultProperties
{
	ApplyType = PASSIVE
	bInStance = false
	DisablesOtherAbilities = true
	TargetType = TARGET_TYPE_ALLIES
}

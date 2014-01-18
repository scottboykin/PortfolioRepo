class BS_StatusEffect extends Actor
	ClassGroup( Bjeardsoft );

enum UpdateTime
{
	START_OF_TURN,
	END_OF_TURN
};

enum TargetTypes
{
	TARGET_TYPE_ALLIES,
	TARGET_TYPE_ENEMIES,
	TARGET_TYPE_ALL
};

var( StatusEffect ) name StatusEffectName;
var( StatusEffect ) string StatusEffectDescription;
var( StatusEffect ) string StatusEffectHudIcon;
var( StatusEffect ) float MaxHPBounsPercent; //This will scale up their current health.
var( StatusEffect ) float MaxAPBounsPercent;
var( StatusEffect ) float AttackBonusPercent; //In terms of %. 1.0 = 100%
var( StatusEffect ) float DefenseBonusPercent; //In terms of %. 1.0 = 100%
var( StatusEffect ) float AttackRadiusBonusPercent; //In terms of %. 1.0 = 100%
var( StatusEffect ) float MoveRadiusBonusPercent; //In terms of %. 1.0 = 100%
var( StatusEffect ) float ThornPercentDamage; //Percent of the attacker's damage should be dealt to the attacker.
var( StatusEffect ) float ProtectionPercent;
var( StatusEffect ) bool IgnoresCombatLock;
var( StatusEffect ) int DamageToDeal;
var( StatusEffect ) float PercentOfDamageToTake;
var( StatusEffect ) float FriendlyFirePercent;
var( StatusEffect ) float EffectRadius;
var( StatusEffect ) TargetTypes TargetType;
var( StatusEffect ) int MaximumNumberOfStacks;
var( StatusEffect ) int TurnsToLast; //negative number means permenant
var( StatusEffect ) bool IncrementStackPerTurn;
var( StatusEffect ) array< ParticleSystem > StatusEffectPSTemplate;
var( StatusEffect ) ParticleSystem PSTemplateToPlayAtTick;
var( StatusEffect ) ParticleSystem PSTemplateToPlayWhenHit;
var( StatusEffect ) ParticleSystem PSTemplateOnHitUnitsInRange;
var( StatusEffect ) ParticleSystem ProtectorPSTemplate;
var( StatusEffect ) UpdateTime WhenToTick;

var ParticleSystemComponent StatusEffectPS;

var BS_Pawn AfflictedUnit;

var int CurrentParticle;
var int CurrentNumberOfStacks;
var float AttackPowerModifer;

function int GetDamage()
{
	return DamageToDeal * AttackPowerModifer * CurrentNumberOfStacks;
}

function int GetHealAmount()
{
	local int Damage;

	Damage = GetDamage();

	if( Damage < 0 && PercentOfDamageToTake > 0 )
	{
		return Damage * PercentOfDamageToTake;
	}
	else if( Damage > 0 && PercentOfDamageToTake < 0 )
	{
		return Damage * PercentOfDamageToTake;
	}

	return 0;
}

function bool IsHoT()
{
	return GetHealAmount() > 0;
}

event PostBeginPlay()
{
	super.PostBeginPlay();

	MaximumNumberOfStacks = max( MaximumNumberOfStacks, 1 );

	if( StatusEffectPSTemplate.Length != 0 && StatusEffectPSTemplate[CurrentParticle] != none )
	{
		StatusEffectPS = WorldInfo.MyEmitterPool.SpawnEmitterCustomLifetime( StatusEffectPSTemplate[CurrentParticle] );
		StatusEffectPS.SetAbsolute(false, false, false);
		StatusEffectPS.SetLODLevel(WorldInfo.bDropDetail ? 1 : 0);		
		StatusEffectPS.bUpdateComponentInTick = true;
		StatusEffectPS.OnSystemFinished = MyOnParticleSystemFinished;	
		AttachComponent( StatusEffectPS );
	}
}

event Tick( float DeltaTime )
{
	if( AfflictedUnit != none )
	{
		SetLocation( AfflictedUnit.Location );
	}

	super.Tick( DeltaTime );
}

function UpdateStatusEffect()
{
	local BS_Pawn Unit;
	local int FriendlyFireDamage;

	if( TurnsToLast < 0 )
	{
		AttackPowerModifer = 1 + BS_Pawn( Owner ).GetAttackBonusPercent();
	}

	if( TurnsToLast > 0 )
		--TurnsToLast;

	if( PSTemplateToPlayAtTick != none )
	{
		WorldInfo.MyEmitterPool.SpawnEmitter( PSTemplateToPlayAtTick, Location,,,AfflictedUnit );
	}

	AfflictedUnit.TakeDamage( GetDamage() * PercentOfDamageToTake, BS_Pawn( Owner ).Controller, AfflictedUnit.Location, vect( 0,0,0 ), class'DamageType',, Owner );

	if( EffectRadius > 0 )
	{
		FriendlyFireDamage = GetDamage() * FriendlyFirePercent;

		foreach CollidingActors( class'BS_Pawn', Unit, EffectRadius, AfflictedUnit.Location )
		{
			if( Unit == AfflictedUnit )
				continue;

			if( Unit.IsFriendly( BS_Pawn( Owner ) ) && ( TargetType == TARGET_TYPE_ALLIES || TargetType == TARGET_TYPE_ALL ) )
			{
				Unit.TakeDamage( FriendlyFireDamage, Controller( Owner.Owner ), unit.Location, vect( 0, 0, 0 ), class'DamageType',, Owner );
				if( PSTemplateOnHitUnitsInRange != none )
				{
					WorldInfo.MyEmitterPool.SpawnEmitter( PSTemplateOnHitUnitsInRange, Unit.Location );
				}
			}

			else if( !Unit.IsFriendly( BS_Pawn( Owner ) ) && ( TargetType == TARGET_TYPE_ENEMIES || TargetType == TARGET_TYPE_ALL ) )
			{
				BS_Pawn( Owner ).DealDamage( unit, false, GetDamage(), Controller( Owner.Owner ), unit.Location, vect( 0, 0, 0 ), class'DamageType',, Owner );
				if( PSTemplateOnHitUnitsInRange != none )
				{
					WorldInfo.MyEmitterPool.SpawnEmitter( PSTemplateOnHitUnitsInRange, Unit.Location );
				}
			}	
		}
	}

	if( TurnsToLast == 0 )
	{
		AfflictedUnit.RemoveStatusEffect( self );
		KillPS();
		Destroy();
	}
	else
	{
		if( IncrementStackPerTurn )
		{
			if( CurrentNumberOfStacks < MaximumNumberOfStacks )
				++CurrentNumberOfStacks;
		}

		else if( StatusEffectPSTemplate.Length > 1 )
		{
			KillPS();
			++CurrentParticle;
			if( CurrentParticle < StatusEffectPSTemplate.Length && StatusEffectPSTemplate[CurrentParticle] != none )
			{
				StatusEffectPS = WorldInfo.MyEmitterPool.SpawnEmitterCustomLifetime( StatusEffectPSTemplate[CurrentParticle] );
				StatusEffectPS.SetAbsolute(false, false, false);
				StatusEffectPS.SetLODLevel(WorldInfo.bDropDetail ? 1 : 0);		
				StatusEffectPS.bUpdateComponentInTick = true;
				StatusEffectPS.OnSystemFinished = MyOnParticleSystemFinished;	
				AttachComponent( StatusEffectPS );
			}
		}
	}
}

function ActivatePS()
{
	if( StatusEffectPS != none )
	{
		StatusEffectPS.ActivateSystem();
	}
}

function KillPS()
{
	if( StatusEffectPS != none )
	{
		StatusEffectPS.DeactivateSystem();
	}
}

simulated function MyOnParticleSystemFinished( ParticleSystemComponent PSC )
{
	if ( PSC == StatusEffectPS )
	{
		DetachComponent( StatusEffectPS );
		WorldInfo.MyEmitterPool.OnParticleSystemFinished( StatusEffectPS );
	}
}

function SpawnHitPS()
{
	if( PSTemplateToPlayWhenHit != none )
	{
		WorldInfo.MyEmitterPool.SpawnEmitter( PSTemplateToPlayWhenHit, Location,,,AfflictedUnit );
	}
}

function SpawnProtectorPS()
{
	if( ProtectorPSTemplate != none )
	{
		WorldInfo.MyEmitterPool.SpawnEmitter( ProtectorPSTemplate, Owner.Location,,, Owner );
	}
}

DefaultProperties
{
	MaxHPBounsPercent = 0 
	MaxAPBounsPercent = 0
	AttackBonusPercent = 0
	AttackRadiusBonusPercent = 0
	DefenseBonusPercent = 0
	MoveRadiusBonusPercent = 0
	ThornPercentDamage = 0
	ProtectionPercent = 0
	DamageToDeal = 0
	PercentOfDamageToTake = 1.0
	CurrentParticle = 0
	MaximumNumberOfStacks = 1
	CurrentNumberOfStacks = 1
	TurnsToLast = 1
	AttackPowerModifer = 1
	WhenToTick = END_OF_TURN
	FriendlyFirePercent = 0.0
	TargetType = TARGET_TYPE_ENEMIES
	IgnoresCombatLock = false
	IncrementStackPerTurn = false
}

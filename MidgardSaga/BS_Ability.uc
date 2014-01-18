class BS_Ability extends Actor
	ClassGroup( Bjeardsoft )
	HideCategories( Debug, Mobile, Object, Advanced );

enum ImpulseLocations
{
	IMPULSE_FROM_CASTER,
	IMPULSE_FROM_MOUSE
};

enum AbilityTypes
{
	ABILITY_TYPE_TARGETED,
	ABILITY_TYPE_PLACED,
	ABILITY_TYPE_RADIAL,
	ABILITY_TYPE_ATTACKOVERRIDE,
	ABILITY_TYPE_PASSIVE
};

enum TargetTypes
{
	TARGET_TYPE_ALLIES,
	TARGET_TYPE_ENEMIES,
	TARGET_TYPE_ALL
};

enum TargetingPositionTypes
{
	TARGET_POSITION_UNIT,
	TARGET_POSITION_VECTOR
};

enum ParticleSystemPlacement
{
	ON_UNIT,
	AT_TARGET_LOCATION
};

enum RangeTypes
{
	UNIT_ATTACK_RANGE,
	UNIT_MOVE_RANGE,
	ABILITY_RANGE
};


var( Ability ) name AbilityName;
var( Ability ) string AbilityDescription;
var( Ability ) name AbilityAnimName;
var( Ability ) SoundCue WindUpSound;//Sound to play before the abiliy fires
var( Ability ) SoundCue FireSound;//Sound to play when the ability is fired
var( Ability ) SoundCue AbilitySound;//Sound to play when abiliy hits
var( Ability ) float WindUpDelay;
var( Ability ) bool RangedAbility; //Set to false if the unit needs to move into melee range first.
var( Ability ) int Damage;
var( Ability ) float PercentOfDamageAsHealthCost;
var( Ability ) byte APCost;
var( Ability ) byte APCostWhenCombatLocked;

var( Ability ) ImpulseLocations ImpulseLocation;
var( Ability ) bool ImpulseAffectsSelf;
var( Ability ) bool ImpulseAffectsAllies;
var( Ability ) bool ImpulseAffectsEnemies;
var( Ability ) float Impulse; //Impulse to be applied to hit pawns. Negative is toward the target location.
var( Ability ) float EffectRadius; //Area of effct of the ability.
var( Ability ) RangeTypes RangeType;
var( Ability ) float Range; //Range of the ability. Used as a range multipler if RangeType is not ABILITY_RANGE.
var( Ability ) float FriendlyFirePercentDamage; //value from 0-1. 0 Meaning Allies will take no damage if hit.
var( Ability ) TargetTypes TargetType;
var( Ability ) int ChargesPerMatch; // Any negative number means infinite charges
var( Ability ) const BS_StatusEffect StatusEffectArchetype;
var( Ability ) bool AggroHitEnemies;

var( Ability ) Projectile ProjArchetypeToFire;
var( Ability ) ParticleSystem AbilityPSTemplate;
var( Ability ) ParticleSystem AbilityPSOnCasterTemplate;
var( Ability ) Vector ParticleSystemOffset;
var( Ability ) Vector ParticleSystemOffsetOnCaster;

var( Ability ) float CameraShakeIntensity;

//var EmitterSpawnable AbilityPS

var const int MomentumBias;
var int AbilityIndex;
var AbilityTypes AbilityType;
var TargetingPositionTypes TargetingPositionType;
var bool InstantCast;
var bool bEnabled;
var bool PlayWindUpPS;
var bool PlayWindUpSound;

var BS_Pawn targetedPawn;
var Vector targetedLocation;

simulated function PostBeginPlay()
{
	
}

function UseAbility( optional BS_Pawn targetPawn, optional Vector targetLocation )
{
	BS_Pawn( Owner ).bAbilityStillFiring = true;

	if( targetPawn == none || ( targetPawn != none && targetPawn.TeamNumber != BS_Pawn( Owner ).TeamNumber ) )
		BS_Pawn( Owner ).CurrentTarget = targetPawn;

	targetedPawn = targetPawn;
	targetedLocation = targetLocation;

	switch( TargetingPositionType )
	{
		case TARGET_POSITION_UNIT:
			if( !InstantCast )
				BS_Pawn( Owner ).LookAtLocation( targetPawn.Location );

			if( RangedAbility || InstantCast || BS_Pawn( Owner ).TargetIsInAttackRange( targetPawn ) )
			{
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
		
				if( ProjArchetypeToFire == none )
				{
					if( WindUpDelay == 0 )
						FireAbility();
					else
					{
						SetTimer( WindUpDelay,, 'FireAbility' );
					}
				}
				else
				{
					if( WindUpDelay == 0 )
						SpawnProjectile();
					else
					{
						SetTimer( WindUpDelay,, 'SpawnProjectile' );
					}
				}
			}
			else
			{
				if( WindUpSound != none )
				{
					PlaySound( WindUpSound );
					PlayWindUpSound = false;
				}

				if( AbilityPSOnCasterTemplate != none )
				{
					SpawnParticleSystemOnCaster();
					PlayWindUpPS = false;
				}

				if( BS_Pawn( Owner ).combatLockedWith != none )
				{
					BS_Pawn( Owner ).combatLockedWith.BreakCombatLock();
				}
				BS_Pawn( Owner ).BreakCombatLock();

				BS_Pawn( Owner ).MoveCommand( targetPawn.Location,, self );
			}
			break;

		case TARGET_POSITION_VECTOR:
			if( !InstantCast )
				BS_Pawn( Owner ).LookAtLocation( targetLocation );

			if( RangedAbility || InstantCast || BS_Pawn( Owner ).HasReachDestination( targetLocation, 5.0f ) )
			{
				if( ChargesPerMatch > 0 )
					--ChargesPerMatch;

				if( RangedAbility && WindUpSound != none )
				{
					PlaySound( WindUpSound );
				}

				if( RangedAbility && AbilityPSOnCasterTemplate != none )
				{
					SpawnParticleSystemOnCaster();
				}

				BS_Pawn( Owner ).PlayAbilityAnim( AbilityIndex );
		
				if( ProjArchetypeToFire == none )
				{
					if( WindUpDelay == 0 )
						FireAbility();
					else
					{
						SetTimer( WindUpDelay,, 'FireAbility' );
					}
				}
				else
				{
					if( WindUpDelay == 0 )
						SpawnProjectile();
					else
					{
						SetTimer( WindUpDelay,, 'SpawnProjectile' );
					}
				}
			}
			else
			{
				if( WindUpSound != none )
				{
					PlaySound( WindUpSound );
				}

				if( AbilityPSOnCasterTemplate != none )
				{
					SpawnParticleSystemOnCaster();
				}

				if( BS_Pawn( Owner ).combatLockedWith != none )
				{
					BS_Pawn( Owner ).combatLockedWith.BreakCombatLock();
				}
				BS_Pawn( Owner ).BreakCombatLock();

				BS_Pawn( Owner ).MoveCommand( targetLocation,, self );
			}
			break;
	}
}

function FireAbility()
{
	local PlayerController PC;

	SpawnParticleSystemOnTarget();

	if( AbilitySound != none )
	{
		PlaySound( AbilitySound );
	}

	if( PercentOfDamageAsHealthCost > 0 )
	{
		Owner.TakeDamage( GetAdjustedDamage() * PercentOfDamageAsHealthCost, BS_Pawn( Owner ).Controller, Owner.Location, vect(0,0,0), class'DamageType',, Owner );
	}

	if( CameraShakeIntensity > 0 )
	{
		PC = BS_PlayerController( BS_GameInfo( WorldInfo.Game ).TurnManager.Controllers[0] );
		 BS_Camera( PC.PlayerCamera ).StartCameraShake( CameraShakeIntensity );
	}

	BS_Pawn( Owner ).bAbilityStillFiring = false;
}

function SpawnProjectile()
{
	if( FireSound != none )
	{
		PlaySound( FireSound );
	}

	switch( TargetingPositionType )
	{
		case TARGET_POSITION_UNIT: 
			Spawn( class'BS_Projectile', self,, Owner.Location,, ProjArchetypeToFire ).FireAt( targetedPawn.Location, false, 0 );
			break;

		case TARGET_POSITION_VECTOR: 
			Spawn( class'BS_Projectile', self,, Owner.Location,, ProjArchetypeToFire ).FireAt( targetedLocation, false, 0 );
			break;
	}
}

function SpawnParticleSystemOnTarget()
{
	local Vector LocToSpawnPS;

	if( TargetingPositionType == TARGET_POSITION_VECTOR || targetedPawn == none )
		LocToSpawnPS = targetedLocation + ParticleSystemOffset;
	else
		LocToSpawnPS = targetedPawn.Location + ParticleSystemOffset;

	if( AbilityPSTemplate != none )
	{
		WorldInfo.MyEmitterPool.SpawnEmitter( AbilityPSTemplate, LocToSpawnPS,,,self );
	}
}

function SpawnParticleSystemOnCaster()
{
	local Vector LocToSpawnPS;

	LocToSpawnPS = Owner.Location + ParticleSystemOffsetOnCaster;

	if( AbilityPSOnCasterTemplate != none )
	{
		WorldInfo.MyEmitterPool.SpawnEmitter( AbilityPSOnCasterTemplate, LocToSpawnPS,,,self );
	}
}

function bool BreaksCombatLock()
{
	return Impulse != 0;
}

function bool HasCharge()
{
	return ChargesPerMatch != 0 || ChargesPerMatch == -1;
}

function BS_StatusEffect GetStatusEffectArchetype()
{
	return StatusEffectArchetype;
}

function bool CanUseAbility( optional BS_Pawn targetPawn, optional Vector targetLocation = vect( -5000, -5000, -5000 ) )
{
	local bool HasLineOfSight;

	PlayWindUpPS = true;
	PlayWindUpSound = true;

	HasLineOfSight = true;

	if( targetPawn != none )
		HasLineOfSight = ProjArchetypeToFire == none || BS_Pawn( Owner ).FastTrace( targetPawn.Location );
	else
		HasLineOfSight = ProjArchetypeToFire == none || BS_Pawn( Owner ).FastTrace( targetLocation );

	return HasCharge() && bEnabled && HasLineOfSight;
}

function int GetAdjustedDamage()
{
	return max( Damage * ( 1 + BS_Pawn( Owner ).GetAttackBonusPercent() ), 0.0f );
}

function float GetAbilityRange()
{
	switch( RangeType )
	{
		case UNIT_ATTACK_RANGE:
			return BS_Pawn( Owner ).GetAdjustedAttackRadius() * Range;
			break;

		case UNIT_MOVE_RANGE:
			return BS_Pawn( Owner ).GetAdjustedMovementRadius() * Range;
			break;

		case ABILITY_RANGE:
			return Range;
			break;
	}
}

function float GetEffectRadius()
{
	return EffectRadius;
}

function byte GetAPCost()
{
	if( BS_Pawn( Owner ).IsCombatLocked() && !( !BS_Pawn( Owner ).bIsMeleeUnit && BS_Pawn( Owner ).IgnoresCombatLock() ) )
	{
		return APCostWhenCombatLocked;
	}

	return self.APCost;
}

function float GetImpulse()
{
	return Impulse;
}

function int GetTotalHealAmount()
{
	local int healAmt;

	healAmt = 0;

	if( Damage < 0 )
	{
		healAmt = Damage;
	}
	else if( FriendlyFirePercentDamage < 0 )
	{
		healAmt = FriendlyFirePercentDamage * Damage;
	}
	
	if( GetStatusEffectArchetype() != none /*&& GetStatusEffectArchetype().IsHoT()*/ )
	{
		healAmt += self.StatusEffectArchetype.DamageToDeal * self.StatusEffectArchetype.TurnsToLast;
	}

	return healAmt;
}

function bool IsHealAbility()
{
	return Damage < 0 || ( Damage > 0 && FriendlyFirePercentDamage < 0 ) || ( StatusEffectArchetype != none && StatusEffectArchetype.DamageToDeal < 0 );
}

function bool TargetInRange( BS_Pawn TargetPawn, Vector TargetLocation )
{
	local float CollisionRadiusOffset;

	CollisionRadiusOffset = BS_Pawn( Owner ).GetCollisionRadius();

	if( TargetPawn != none )
	{
		CollisionRadiusOffset += TargetPawn.GetCollisionRadius();
	}

	return VSize2D( Owner.Location - TargetLocation ) < ( GetAbilityRange() + CollisionRadiusOffset );
}

function bool TargetInEffectRadius( BS_Pawn Target, Vector CastPosition )
{
	return VSize2D( CastPosition - Target.Location ) < ( GetEffectRadius() + Target.GetCollisionRadius() );
}

function array< BS_Pawn > GetUnitsInEffectRadius( Vector CastPosition )
{
	local array< BS_Pawn > HitUnits;
	local BS_Pawn Unit;

	if( GetEffectRadius() > 0 )
	{
		foreach AllActors( class'BS_Pawn', Unit )
		{
			if( TargetInEffectRadius( Unit, CastPosition ) )
			{
				HitUnits.AddItem( Unit );
			}
		}
	}

	return HitUnits;
}

function int GetNumberOfTargetsInEffectRadius( Vector CastPosition )
{
	return GetUnitsInEffectRadius( CastPosition ).Length;
}

function Vector GetCenter()
{
	return BS_Pawn( Owner ).CurrentCirclePos;
}

function bool Clickable()
{
	return bEnabled;
}

function bool IsAbilityWithCharges()
{
	return ChargesPerMatch >= 0;
}

DefaultProperties
{
	AbilityName = ""
	AbilityDescription = ""
	RangedAbility = true
	AbilityType = ABILITY_TYPE_TARGETED
	ImpulseLocation = IMPULSE_FROM_CASTER
	TargetType = TARGET_TYPE_ENEMIES
	TargetingPositionType = TARGET_POSITION_UNIT

	RangeType = ABILITY_RANGE

	ImpulseAffectsSelf = true
	ImpulseAffectsAllies = true
	ImpulseAffectsEnemies = true
	InstantCast = false
	PlayWindUpPS = true
	bEnabled = true
	AggroHitEnemies = false

	AbilityAnimName=bs_chara_attack

	WindUpDelay = 0.0f
	Damage = 1
	Impulse = 0
	EffectRadius = 0
	Range = 100
	FriendlyFirePercentDamage = 0
	PercentOfDamageAsHealthCost = 0
	APCost = 1

	APCostWhenCombatLocked = 2

	ChargesPerMatch = -1

	CameraShakeIntensity = 0.0

	MomentumBias = 165
	StatusEffectArchetype = none
}

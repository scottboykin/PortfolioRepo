/**
 * Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
 */
class BS_Pawn extends Pawn
	HideCategories( Actor, Object )
	dependson( BS_StatusEffect )
	ClassGroup( Bjeardsoft );


var( Unit ) name UnitName;

var( Unit ) int MaxHealth;
var( Unit ) float PercentageLowHealth;
var( Unit ) int NumberOfDesirableTargets;
var( Unit ) int DesirableMinDistanceFromAllies;
var( Unit ) byte MaxActionPoints;
var( Unit ) int MoveRadius;
var( Unit ) int AttackRadius;
var( Unit ) int AttackPower;
var( Unit ) float WindUpDelay;
var( Unit ) float AttackSoundDelay;
var( Unit ) float DefenseRating;
var( Unit ) byte TeamNumber;
var( Unit ) bool bIsMeleeUnit;
var( Unit ) Projectile ProjArchetypeToFire;
var( Unit ) Vector ProjStartPosOffset;
var( Unit ) float ProjSpawnDelay;
var( Unit ) bool bIsNPC;
var( Unit ) array< BS_Ability > AbilityArchetypes;
var( Unit ) float UnitVelocityMag;

var( Unit ) name AttackAnimName;
var( Unit ) name DamagedAnimName;
var( Unit ) name DeathAnimName;

var( Unit ) SoundCue AttackImpactSound;
var( Unit ) SoundCue AttackVocalSound;
var( Unit ) SoundCue HitSound;
var( Unit ) SoundCue DeathSound;
var( Unit ) SoundCue MovementSound;
var( Unit ) SoundCue ChargeSound;

var( Unit ) float HeightForHealthBarPos;

var SkeletalMeshComponent OccludedMesh;

var( Unit ) bool bShouldSkipTurn;
var( Unit ) array< const BS_State > Goals;
var( Unit ) array< BS_Action > ActionArchetypes;

var byte actionPoints;
var int lastHealth;

var bool bJustEnteredCombatLock;
var bool bCombatLock;
var bool bIsSelected;
var bool bIsUsingAbility;
var bool bIsAttacking;
var bool bIsBeingHit;
var bool bAbilityStillFiring;

var array< BS_Ability > Abilities;
var array< BS_StatusEffect > UnitStatusEffects;

var BS_Pawn CombatLockedWith;
var BS_Pawn CurrentTarget;
var BS_Pawn CurrentAllyTarget;
var Vector CurrentDestination;

var array< BS_Action > ActionSet;
var array< BS_Action > Actions;
var BS_Action CurrentAction;
var BS_State WorldState;
var BS_State CurrentGoal;
var( Objective ) Actor Objective;
var( Objective ) int DesiredMaxDistanceFromObjective;

var BS_CircleDecalPainter CirclePainter;

var Vector CurrentCirclePos;

var BS_PlayerController PlayerController;

var AnimNodePlayCustomAnim AttackAnimNode;
var AnimNodePlayCustomAnim HitAnimNode;
var AnimNodePlayCustomAnim AbilityAnimNode;
var AnimNodePlayCustomAnim DeathAnimNode;

var MaterialInstanceConstant SelectedMaterial;
var MaterialInstanceConstant DeselectedMaterial;

var MaterialInstanceConstant EnemySelectedMaterial;
var MaterialInstanceConstant EnemyDeselectedMaterial;

var EmitterSpawnable CombatLockShieldPS;
var( Unit ) ParticleSystem CombatLockShieldPSTemplate;
var( Unit ) Vector CombatLockPSOffset;
var( Unit ) bool IsStationary;

var EmitterSpawnable CombatLockBeamPS;
var( Unit ) ParticleSystem CombatLockBeamPSTemplate;

var EmitterSpawnable HoverOverBeamPS;
var EmitterSpawnable HoverOverRingPS;
var( Unit ) ParticleSystem HoverOverBeamPSTemplate;
var( Unit ) ParticleSystem HoverOverRingPSTemplate;
var( Unit ) Vector HoverOverPSOffset;
var( Unit ) Vector HoverOverRingPSOffset;
var bool bIsRingPSActiveRegardlessOfHoverOver;

var float CurrentHoverOverRingHeightOffset;
var const float HoverOverRingAnimStartHeight;

var float CurrentRingAnimScale;
var const float MaxRingAnimScale;
var float RingAnimScaleDir;

var float CurrentHoverOverBeamHeightOffset;
var const float HoverOverBeamAnimStartHeight;

var float HoverOverAnimTimer;
var const float HoverOverAnimMaxTime;

var bool bIsBeingHoveredOver;

var( Unit ) ParticleSystem AttackOnUnitPSTemplate;
var( Unit ) Vector AttackOnUnitPSOffset;
var( Unit ) ParticleSystem AttackAtTargetPSTemplate;
var( Unit ) Vector AttackAtTargetPSOffset;

var( Unit ) ParticleSystem BloodSplatterPSTemplate;
var( Unit ) Vector BloodSplatterPSOffset;
var( Unit ) Rotator BloodSplatterPSRotation;

var( Unit ) StaticMesh MeshToSpawnOnDeath;
var( Unit ) float DeadMeshLifespan;
var( Unit ) Vector DeadMeshOffset;

var( Unit ) bool bUseHealthBar;
var( Unit ) bool bUseDamagePopups;
var( Unit ) bool bAffectedByImpulse;
var( Unit ) bool bCanBeCombatLocked;
var Rotator CurrentRotation;

var MaterialInterface DefaultMaterial;
var( Unit ) MaterialInterface OutOfActionPointsMaterial;
var( Unit ) MaterialInterface InRangeMaterial;

var BS_GFxHUD_HealthBar UnitHealthBar;
var BS_GFxHUD_AIThinking AIThinkingBar;
var BS_GFxHUD_DamagePopup DamageDealtPopup;


var Vector LocationToFireProjAt;

//temp stuff for rendering damage dealt to the HUD
var bool bRecievedDamageThatHasntBeenShown;
var float DamageRecieved;

var bool bIsInFutureMode;

//Used for Kismet nodes to be able to disable these for the tutorial
var bool bCanMove;
var bool bCanAttack;

var( DesctrubleObject ) bool bIsDesctrubleObject;
var Vector StartingPosition;
var Rotator StartingRotation;

var DecalComponent AttackDecal;

var bool bDrawingHoveredAbilityRange;

var BS_AbilityMasterBall MasterBallArchetype;

var bool bIsInPokeBall;

simulated event PreBeginPlay()
{
	local BS_ObjectivePropertiesActor objActor;

	super.PreBeginPlay();
	if( !bIsNPC )
	{
		TeamNumber = 0;
	}
	else
	{
		TeamNumber = 1;
	}
	if( OccludedMesh != none )
	{
		OccludedMesh.SetSkeletalMesh( Mesh.SkeletalMesh );
		if( !bIsNPC )
		{
			OccludedMesh.SetMaterial( 0, DeselectedMaterial );
			OccludedMesh.SetMaterial( 1, DeselectedMaterial );
			OccludedMesh.SetMaterial( 2, DeselectedMaterial );
		}
		else
		{
			OccludedMesh.SetMaterial( 0, EnemyDeselectedMaterial );
			OccludedMesh.SetMaterial( 1, EnemyDeselectedMaterial );	
			OccludedMesh.SetMaterial( 2, EnemyDeselectedMaterial );	
		}
		OccludedMesh.SetHidden( true );
	}
	DefaultMaterial = Mesh.GetMaterial( 0 );

	foreach AllActors( class'BS_ObjectivePropertiesActor', objActor )
	{
		if( objActor.Objective != none )
		{
			Objective = objActor.Objective;
			if( DesiredMaxDistanceFromObjective == 0 )
				DesiredMaxDistanceFromObjective = objActor.DesiredMaxDistanceFromObjective;
		}
	}
	OutOfActionPointsMaterial = none;
}

simulated function PostBeginPlay()
{
	local int i;
	local BS_AIController AIController;
	local BS_PlayerController BSPC;
	local BS_GameInfo BSGI;

	super.PostBeginPlay();

	// Init actions
	for( i = 0; i < ActionArchetypes.Length; ++i )
	{
		ActionSet[i] = Spawn( ActionArchetypes[i].Class, self,,,,ActionArchetypes[i] );
		ActionSet[i].ActionOwner = self;
	}

	if( Owner == none )
	{
		SpawnDefaultController();
	}

	Health = MaxHealth;

   // Mesh.SetSkeletalMesh( UnitSkeletalMesh );

	CirclePainter = Spawn( class'BS_CircleDecalPainter' );

	

	CurrentCirclePos = self.Location;	
	BSGI = BS_GameInfo( WorldInfo.Game );
	AIController = BS_AIController( BSGI.TurnManager.GetController( TeamNumber ) );
	BSPC = BS_PlayerController( BSGI.TurnManager.GetController( 0 ) );
	
	if( bIsNPC && AIController != none )
	{
		AIController.AddUnit( self );
		BS_UnitController( Owner ).Controller = AIController;
	}

	CombatLockShieldPS = Spawn( class'EmitterSpawnable' );
	CombatLockShieldPS.SetTemplate( CombatLockShieldPSTemplate );
	CombatLockShieldPS.ParticleSystemComponent.DeactivateSystem();

	CombatLockBeamPS = Spawn( class'EmitterSpawnable' );
	CombatLockBeamPS.SetTemplate( CombatLockBeamPSTemplate );
	CombatLockBeamPS.ParticleSystemComponent.DeactivateSystem();

	HoverOverBeamPS = Spawn( class'EmitterSpawnable' );
	HoverOverRingPS = Spawn( class'EmitterSpawnable' );
	HoverOverBeamPS.SetTemplate( HoverOverBeamPSTemplate );
	HoverOverRingPS.SetTemplate( HoverOverRingPSTemplate );
	HoverOverBeamPS.ParticleSystemComponent.DeactivateSystem();
	if( bIsNPC && !BS_HUD( BSPC.myHUD ).TheHUDMovie.bPlayerHUDEnabled && !bHidden )
	{
		HoverOverRingPS.ParticleSystemComponent.ActivateSystem();
	}
	else
	{
		HoverOverRingPS.ParticleSystemComponent.DeactivateSystem();
		bIsRingPSActiveRegardlessOfHoverOver = false;
	}

	CreateHealthBar();
	CreateAIThinkingBar();  
	CreateDamageDealtPopup();

	// Init state
	WorldState = new class'BS_State';
	for( i = 0; i < class'BS_State'.static.GetBoolPropertyCount(); ++i )
	{
		WorldState.AddOrModifyBoolState( EBoolPropertyKey( i ), false );
	}
	for( i = 0; i < class'BS_State'.static.GetIntPropertyCount(); ++i )
	{
		WorldState.AddOrModifyIntState( EIntPropertyKey( i ), 0 );
	}
	WorldState.AddOrModifyIntState( HIT_POINTS, MaxHealth );
	WorldState.AddOrModifyIntState( ACTION_POINTS, 2 );
	
	CurrentRotation = Rotation;
	lastHealth = MaxHealth;
	
	InitAbilities();	
	
	if( self.bIsMeleeUnit )
	{
		WorldState.AddOrModifyBoolState( IS_MELEE, true );
	}
	else
	{
		WorldState.AddOrModifyBoolState( IS_MELEE, false );
	}
	StartingPosition = Location;
	StartingRotation = Rotation;
}

function InitAbilities()
{
	local int i;
	local bool hasImpulseAbilities;

	Abilities.Length = 0;
	UnitStatusEffects.Length = 0;
	for( i = 0; i < AbilityArchetypes.Length; ++i )
	{
		if( AbilityArchetypes[i] != none )
		{
			Abilities[i] = Spawn( AbilityArchetypes[i].Class,self,,,,AbilityArchetypes[i] );
			Abilities[i].AbilityIndex = i;
		}
	}
	hasImpulseAbilities = false;
	for( i = 0; i < self.Abilities.Length; ++i )
	{
		if( Abilities[i].Impulse != 0 )
		{
			hasImpulseAbilities = true;
		}
	}
	WorldState.AddOrModifyBoolState( HAS_IMPULSE_ABILITIES, hasImpulseAbilities );
}

function AssignAbilityArchetype( BS_Ability archetype, int index )
{
	AbilityArchetypes.InsertItem( index, archetype );	
}

function AssignAbilityArchetypeArray( array<BS_Ability> archetype )
{
	AbilityArchetypes = archetype;	
}

event Bump( Actor Other, PrimitiveComponent OtherComp, Vector HitNormal )
{
	if( BS_Pawn( Other ) != none )
	{
		BS_UnitController( Controller ).RanIntoUnit( Other, OtherComp, HitNormal );
	}

	super.Bump( Other, OtherComp, HitNormal );
}

function bool HasPlan()
{
	if( Actions.Length > 0 )
	{
		return true;
	}
	return false;
}

function bool IsFriendly( BS_Pawn otherPawn )
{
	return TeamNumber == otherPawn.TeamNumber;
}

function UpdateStates()
{
	local BS_Pawn unit;
	local int i;
	local int numEnemies;

	numEnemies = 0;
	foreach WorldInfo.AllPawns( class'BS_Pawn', unit )
	{
		if( !self.IsFriendly( unit ) /*&& VSize( unit.Location - CurrentTarget.Location ) < 300*/ )
		{
			++numEnemies;
		}
	}
	//-----------------------------------------
	WorldState.AddOrModifyBoolState( HAS_DESTINATION, false );
	//-----------------------------------------
	WorldState.AddOrModifyBoolState( IS_BUFFED, false );
	//-----------------------------------------
	if( Objective != none )
	{
		WorldState.AddOrModifyBoolState( HAS_OBJECTIVE, true );
		//if( VSize( self.Location - Objective.Location ) < self.DesiredMaxDistanceFromObjective ) 
		//{
		//	WorldState.AddOrModifyBoolState( IS_IN_RANGE_OF_OBJECTIVE, true );
		//}
		//else
		//{
			WorldState.AddOrModifyBoolState( IS_IN_RANGE_OF_OBJECTIVE, false );
		//}
	}
	else
	{
		WorldState.AddOrModifyBoolState( HAS_OBJECTIVE, false );
		WorldState.AddOrModifyBoolState( IS_IN_RANGE_OF_OBJECTIVE, true );
	}
	//-----------------------------------------
	if( ( float( self.Health ) / self.MaxHealth ) > self.PercentageLowHealth )
	{
		WorldState.AddOrModifyBoolState( HAS_LOW_HP, false );
	}
	else
	{
		WorldState.AddOrModifyBoolState( HAS_LOW_HP, true );
	}
	//-----------------------------------------
	if( VSize( self.CurrentDestination - self.Location ) < self.GetCollisionRadius() )
	{
		WorldState.AddOrModifyBoolState( HAS_REACHED_DESTINATION, true );
	}
	else
	{
		WorldState.AddOrModifyBoolState( HAS_REACHED_DESTINATION, false );
	}
	//-----------------------------------------
	if( self.bCombatLock )
	{
		WorldState.AddOrModifyBoolState( IS_LOCKED, true );
		//bJustEnteredCombatLock = false;
	}
	else
	{
		WorldState.AddOrModifyBoolState( IS_LOCKED, false );
		bJustEnteredCombatLock = false;
	}
	//-----------------------------------------
	//if( self.CurrentTarget == none )
	//{
	//	unit = self.getClosestEnemy();
	//}
	//else
	//{
	//	unit = CurrentTarget;
	//}
	//if( VSize( unit.Location - self.Location ) <= self.GetAdjustedAttackRadius() )
	//{
	//	WorldState.AddOrModifyBoolState( IS_IN_RANGE, true );
	//	if( self.CurrentAllyTarget != none )
	//	{
	//		WorldState.AddOrModifyBoolState( IS_IN_RANGE, false );
	//	}
	//}
	//else
	//{
	//	WorldState.AddOrModifyBoolState( IS_IN_RANGE, false );
	//	CurrentTarget = none;
	//	WorldState.AddOrModifyBoolState( HAS_TARGET, false );
	//}
	if( self.CurrentTarget == none && !IsStationaryType() )
	{
		WorldState.AddOrModifyBoolState( IS_IN_RANGE, false );
		WorldState.AddOrModifyBoolState( HAS_TARGET, false );
	}
	else
	{
		if( IsStationaryType() )
		{
			unit = self.getClosestEnemy();
		}
		unit = CurrentTarget;
		if( VSize( unit.Location - self.Location ) <= self.GetAdjustedAttackRadius() && self.ValidTarget( unit ) )
		{
			WorldState.AddOrModifyBoolState( IS_IN_RANGE ,true );
			if( self.CurrentAllyTarget != none )
			{
				WorldState.AddOrModifyBoolState( IS_IN_RANGE, false );
			}
		}
		else
		{
			WorldState.AddOrModifyBoolState( IS_IN_RANGE, false );
			CurrentTarget = none;
			WorldState.AddOrModifyBoolState( HAS_TARGET, false );
		}
	}
	//-----------------------------------------	
	if( self.CurrentTarget != none )
	{
		i = 1;
		foreach WorldInfo.AllPawns( class'BS_Pawn', unit )
		{
			if( unit != CurrentTarget && unit.TeamNumber != self.TeamNumber )
			{
				if( VSize( unit.Location - CurrentTarget.Location ) < 150 )
				{
					++i;
				}
			}
		}
		if( i >= min( self.NumberOfDesirableTargets, numEnemies ) )
		{
			WorldState.AddOrModifyBoolState( CAN_HIT_MULTIPLE_TARGETS, true );
		}
		else
		{
			WorldState.AddOrModifyBoolState( CAN_HIT_MULTIPLE_TARGETS, false );
		}
	}
	else
	{
		i = 0;
		foreach WorldInfo.AllPawns( class'BS_Pawn', unit )
		{
			if( unit.TeamNumber != self.TeamNumber )
			{
				if( VSize( unit.Location - self.Location ) < self.GetAdjustedAttackRadius() )
				{
					++i;
				}
			}
		}
		if( i >= min( self.NumberOfDesirableTargets, numEnemies ) )
		{
			WorldState.AddOrModifyBoolState( CAN_HIT_MULTIPLE_TARGETS, true );
		}
		else
		{
			WorldState.AddOrModifyBoolState( CAN_HIT_MULTIPLE_TARGETS, false );
		}
	}
	//-----------------------------------------
	if( CurrentAllyTarget != none )
	{
		WorldState.AddOrModifyBoolState( HAS_ALLY_TARGET, false );
		//WorldState.AddOrModifyBoolState( HAS_ALLY_TARGET, true );

		if( ( float( CurrentAllyTarget.Health ) / CurrentAllyTarget.MaxHealth ) > CurrentAllyTarget.PercentageLowHealth )
		{
			WorldState.AddOrModifyBoolState( ALLY_LOW_HP, false );
		}
		else
		{
			WorldState.AddOrModifyBoolState( ALLY_LOW_HP, true );
		}

		if( VSize( CurrentAllyTarget.Location - Location ) < self.DesirableMinDistanceFromAllies )
		{
			WorldState.AddOrModifyBoolState( IS_IN_RANGE_OF_ALLY, true );
		}
		else
		{
			WorldState.AddOrModifyBoolState( IS_IN_RANGE_OF_ALLY, false );
		}
		
		if( CurrentAllyTarget.bCombatLock == true )
		{
			WorldState.AddOrModifyBoolState( ALLY_LOCKED, true );
		}
		else
		{
			WorldState.AddOrModifyBoolState( ALLY_LOCKED, false );
		}
		if( CurrentAllyTarget.CurrentTarget != none )
		{
			if( CurrentAllyTarget.WorldState.GetBoolState( IS_IN_RANGE ).Value )
			{
				WorldState.AddOrModifyBoolState( ALLY_ATTACKING, true );
			}
			else
			{
				WorldState.AddOrModifyBoolState( ALLY_ATTACKING, false );
			}
		}
		else
		{
			WorldState.AddOrModifyBoolState( ALLY_ATTACKING, false );
		}
		//---------------------------------------------------
		if( CurrentAllyTarget.WorldState.GetBoolState( IS_ATTACKED ).Value )
		{
			WorldState.AddOrModifyBoolState( ALLY_ATTACKED, true );
		}
		else
		{
			WorldState.AddOrModifyBoolState( ALLY_ATTACKED, false );
		}
		//---------------------------------------------------
	}
	else
	{
		WorldState.AddOrModifyBoolState( ALLY_LOW_HP, false );
		WorldState.AddOrModifyBoolState( IS_IN_RANGE_OF_ALLY, false );
		WorldState.AddOrModifyBoolState( ALLY_LOCKED, false );
		WorldState.AddOrModifyBoolState( ALLY_ATTACKING, false );
		WorldState.AddOrModifyBoolState( HAS_ALLY_TARGET, false );
		WorldState.AddOrModifyBoolState( ALLY_ATTACKED, false );
	}
	//-----------------------------------------
	//if( CurrentTarget == none )
	//{
		WorldState.AddOrModifyBoolState( IS_TARGET_DEAD, false );
		WorldState.AddOrModifyBoolState( HAS_TARGET, false );
	//	WorldState.AddOrModifyBoolState( IS_IN_RANGE, false );
	//}
	//else 
	//{
	//	WorldState.AddOrModifyBoolState( HAS_TARGET, true );
	//}
	//-----------------------------------------
	if( self.Health != lastHealth )
	{
		WorldState.AddOrModifyBoolState( IS_ATTACKED, true );
	}
	else
	{
		WorldState.AddOrModifyBoolState( IS_ATTACKED, false );
	}
	lastHealth = self.Health;
	//----------------------------------------
}

function CreateHealthBar()
{
	if( bUseHealthBar )
	{
		UnitHealthBar = new class'BS_GFxHUD_HealthBar';
		UnitHealthBar.SetTimingMode( TM_Real );
		UnitHealthBar.Init();
		UnitHealthBar.PawnOwner = self;
	}
}

function CreateAIThinkingBar()
{
	if( bIsNPC )
	{
		AIThinkingBar = new class'BS_GFxHUD_AIThinking';
		AIThinkingBar.SetTimingMode( TM_Real );
		AIThinkingBar.Init();
	}
}

function CreateDamageDealtPopup()
{
	DamageDealtPopup = new class'BS_GFxHUD_DamagePopup';
	DamageDealtPopup.SetTimingMode( TM_Real );
	DamageDealtPopup.Init();
}

simulated event PostInitAnimTree( SkeletalMeshComponent SkelComp )
{
	super.PostInitAnimTree( SkelComp );

	if( SkelComp == Mesh )
	{
		AttackAnimNode = AnimNodePlayCustomAnim( SkelComp.FindAnimNode( 'AttackAnim' ) );
		HitAnimNode = AnimNodePlayCustomAnim( SkelComp.FindAnimNode( 'HitAnim' ) );
		AbilityAnimNode = AnimNodePlayCustomAnim( SkelComp.FindAnimNode( 'AbilityAnim' ) );
		DeathAnimNode = AnimNodePlayCustomAnim( SkelComp.FindAnimNode( 'DeathAnim' ) );
	}
}

simulated function MoveCommand( Vector WorldMoveLocation, optional bool IgnoreCombatLock = false, optional BS_Ability AbilityToFireAfterMove = none )
{    
	WorldMoveLocation.Z = Location.Z;
	
	if( !bCombatLock || IgnoreCombatLock ) 
	{		
		BS_UnitController( Controller ).MoveCommand( WorldMoveLocation, AbilityToFireAfterMove );

		if( MovementSound != none )
		{
			PlaySound( MovementSound );
		}
	}
}

simulated function StoppedMoving()
{
	local BS_Pawn TouchingActor;
	local Vector direction;
	TouchingActor = none;

	foreach TouchingActors( class'BS_Pawn', TouchingActor )
	{
		direction = Location - TouchingActor.Location;
		direction.Z = 0;

		direction = Normal( direction );

		Velocity = direction * 5000;
	}
}

function Tick( float DeltaTime )
{
	//if( !bIsSelected && PlayerController( Owner ) != none )
	//{
	//	BS_HUD( PlayerController( Owner ).myHUD ).SetCurrentUnitSelection( self );
	//	bIsSelected = true;
	//}
	local Vector distanceFromMouse;
	local BS_HUD bsHUD;
	if( PlayerController != none )
		bsHUD = BS_HUD( PlayerController.myHUD );


	super.Tick( DeltaTime );
	self.SetRotation( CurrentRotation );
	if( UnitHealthBar != none )
		UnitHealthBar.TickHUD();

	SetPositionOfUnitCircle();
	if( bIsSelected && !bDrawingHoveredAbilityRange )
	{
		//CirclePainter.ActivateCirclePS();
		
		CirclePainter.DeactivateAllDecals();
		if( PlayerController != none && bsHUD != none && PlayerController.CurrentAction == ACTION_ABILITY )
		{			
			distanceFromMouse = bsHUD.currentMouseLocationOnTerrain - Location;

			if( Abilities[PlayerController.CurrentAbilityIndex].TargetingPositionType == TARGET_POSITION_VECTOR && VSize2D( distanceFromMouse ) <= Abilities[PlayerController.CurrentAbilityIndex].GetAbilityRange() + GetCollisionRadius() )
			{
				CirclePainter.DrawCircle( GREEN, 10.0f, bsHUD.currentMouseLocationOnTerrain );	
				CirclePainter.DrawCircle( GREEN_2, Abilities[PlayerController.CurrentAbilityIndex].GetEffectRadius(), bsHUD.currentMouseLocationOnTerrain );	
				if( Abilities[PlayerController.CurrentAbilityIndex].AbilityType == ABILITY_TYPE_PLACED )
					ActivateInRangeOfPlacedAbility( bsHUD.currentMouseLocationOnTerrain, Abilities[ PlayerController.CurrentAbilityIndex ].GetEffectRadius(), Abilities[ PlayerController.CurrentAbilityIndex ].TargetType );
				CirclePainter.DrawCircle( RED, Abilities[PlayerController.CurrentAbilityIndex].GetAbilityRange(), Abilities[PlayerController.CurrentAbilityIndex].GetCenter() );	
			}
			else
			{
				CirclePainter.DeactivateAllDecals();
				if( Abilities[PlayerController.CurrentAbilityIndex].AbilityType == ABILITY_TYPE_PLACED )
					ActivateInRangeOfPlacedAbility( bsHUD.currentMouseLocationOnTerrain, 0, Abilities[ PlayerController.CurrentAbilityIndex ].TargetType );
				CirclePainter.DrawCircle( RED, Abilities[PlayerController.CurrentAbilityIndex].GetAbilityRange(), Abilities[PlayerController.CurrentAbilityIndex].GetCenter() );	
			}
		}
		else if( bIsInFutureMode )
		{
			if( bsHUD != none )
			{
				distanceFromMouse = bsHUD.currentMouseLocationOnTerrain - Location;
			
				if( VSize( distanceFromMouse ) <= MoveRadius )
				{
					CirclePainter.DrawCircle( BLUE_2, GetAdjustedMovementRadius(), bsHUD.currentMouseLocationOnTerrain );	 
					CirclePainter.DrawCircle( RED_2, GetAdjustedAttackRadius(), bsHUD.currentMouseLocationOnTerrain );							
				}
				CirclePainter.DrawCircle( BLUE, GetAdjustedMovementRadius(), CurrentCirclePos );	 
			}
		}
		else
		{
			if( bsHUD != none )
				CirclePainter.DrawCircle( WHITE, 20.0f, bsHUD.currentMouseLocationOnTerrain );
			CirclePainter.DrawCircle( BLUE, GetAdjustedMovementRadius(), CurrentCirclePos );	 
			CirclePainter.DrawCircle( RED, GetAdjustedAttackRadius(), Location );
		}
	}
	//else
	//{
	//	CirclePainter.DeactivateCirclePS();
	//}

	if( IsCombatLocked() && ( CombatLockedWith == none || ( CombatLockedWith != none && CombatLockedWith.GetAdjustedHealth() <= 0 ) ) )
	{
		BreakCombatLock();
		CombatLockedWith = none;
	}

	UpdateHoverOverPS( DeltaTime );
	UpdateCombatLockPS();
	
	if( bIsDesctrubleObject )
	{		
		SetPawnToInitState();
	}
}

function DrawAbilityRange( int idx )
{
	local BS_StatusEffect BSSE;
	local float AdjMoveRadius;
	local float AdjAttackRadius;
	local BS_AttribBoostAbility BSABA;
	if( idx >= 0 && idx < Abilities.Length  )
	{
		BSABA = BS_AttribBoostAbility( Abilities[ idx ] );
		BSSE = Abilities[ idx ].GetStatusEffectArchetype();
		if( BSSE != none && Abilities[ idx ].TargetType != TARGET_TYPE_ENEMIES )
		{
			AdjMoveRadius =  GetAdjustedMovementRadius();
			AdjAttackRadius = GetAdjustedAttackRadius();
			AdjMoveRadius = AdjMoveRadius + MoveRadius * BSSE.MoveRadiusBonusPercent;
			
			if( BSABA != none )
			{
				if( BSABA.bInStance )
				{	
					AdjAttackRadius = AdjAttackRadius - AttackRadius * BSSE.AttackRadiusBonusPercent;
				}
				else
				{
					AdjAttackRadius = AdjAttackRadius + AttackRadius * BSSE.AttackRadiusBonusPercent;
				}
				CirclePainter.DrawCircle( BLUE, Abilities[idx].GetAbilityRange(), Abilities[idx].GetCenter() );
				CirclePainter.DrawCircle( RED, AdjAttackRadius, Abilities[idx].GetCenter() );
			}
			else
			{
				CirclePainter.DrawCircle( RED, Abilities[idx].GetAbilityRange(), Abilities[idx].GetCenter() );
				CirclePainter.DrawCircle( BLUE, AdjMoveRadius, Abilities[idx].GetCenter() );
			}

		}
		else
		{
			if( BSABA != none )
			{
				CirclePainter.DrawCircle( BLUE, Abilities[idx].GetEffectRadius(), Abilities[idx].GetCenter() );
			}
			else
			{
				if( Abilities[idx].AbilityType == ABILITY_TYPE_RADIAL )
					CirclePainter.DrawCircle( RED, Abilities[idx].GetEffectRadius(), Abilities[idx].GetCenter() );
				else
					CirclePainter.DrawCircle( RED, Abilities[idx].GetAbilityRange(), Abilities[idx].GetCenter() );
			}	
		}
	}
}

simulated function AttackTarget( BS_Pawn target )
{
	local int i;
	local bool attackOverriden;
	local bool isInRange;

	attackOverriden = false;
	isInRange = TargetIsInAttackRange( target );
	CurrentTarget = target;
	LookAtLocation( target.Location );

	for( i = 0; i < Abilities.Length; ++i )
	{
		if( Abilities[i].AbilityType == ABILITY_TYPE_ATTACKOVERRIDE )
		{
			Abilities[i].UseAbility( target );
			attackOverriden = true;
			break;
		}
	}

	LookAtLocation( target.Location );
	isInRange = TargetIsInAttackRange( target );

	if( !attackOverriden && isInRange )
	{
		if( AttackVocalSound != none )
		{
			PlaySound( AttackVocalSound );
		}

		PlayAttackAnim();
		SpawnAttackParticles();

		if( bIsMeleeUnit || ProjArchetypeToFire == none )
		{ 
			DealDamage( target, bIsMeleeUnit, GetAdjustedAttackPower(), Controller( Owner ), target.Location, vect( 0, 0, 0 ), class'DamageType',, self ); 

			if( AttackImpactSound != none )
			{
				if( AttackSoundDelay == 0.0 )
					PlaySound( AttackImpactSound );
				else
					SetTimer( AttackSoundDelay,, 'PlayAttackSound' );
			}
		}
		else
		{
			SetTimer( ProjSpawnDelay,,'SpawnProj' );
			LocationToFireProjAt = target.Location;
		}
	}

	if( bIsMeleeUnit && isInRange )
	{
		InitiateCombatLock( Target );
	}
	
}

function PlayAttackSound()
{
	PlaySound( AttackImpactSound );
}

function DealDamage( BS_Pawn Target, bool IsMeleeAttack, int Damage, Controller InstigatedBy, Vector HitLocation, Vector Momentum, class<DamageType> DamageType, optional TraceHitInfo HitInfo, optional Actor DamageCauser )
{
	local float ThornPercentDamage;
	local float ProtectionPercent;
	local BS_Pawn Protector;
	local int i;
	local BS_PlayerController BSPC;

	
	ThornPercentDamage = Target.GetThornDamagePercent();
	ProtectionPercent = Target.GetProtectionPercent( Protector );	

	if( ThornPercentDamage > 0 )//IsMeleeAttack && ThornPercentDamage > 0 )
	{
		TakeDamage( Damage * ThornPercentDamage, Target.Controller, Location, vect(0,0,0), class'DamageType',, self );
	}

	if( Protector != none && !Protector.IsDead() )
	{
		Target.TakeDamage( Damage * ( 1 - ProtectionPercent ), InstigatedBy, HitLocation, Momentum, DamageType, HitInfo, DamageCauser );
		Protector.TakeDamage( Damage * ProtectionPercent, InstigatedBy, HitLocation, vect(0,0,0), DamageType, HitInfo, DamageCauser );

		for( i = 0; i < Target.UnitStatusEffects.Length; ++i )
		{
			if( Target.UnitStatusEffects[i].ProtectionPercent != 0.0 )
			{
				Target.UnitStatusEffects[i].SpawnHitPS();
				Target.UnitStatusEffects[i].SpawnProtectorPS();
				break;
			}
		}
	}
	else
	{
		Target.TakeDamage( Damage, InstigatedBy, HitLocation, Momentum, DamageType, HitInfo, DamageCauser );
	}
	
	BSPC = BS_PlayerController( BS_UnitController( self.Owner ).Owner );
	if( BSPC != none )
	{
		BSPC.TotalDamageDealt += Damage;
	}
}


function TargetHit()
{
	DealDamage( CurrentTarget, bIsMeleeUnit, GetAdjustedAttackPower(), Controller( Owner ), CurrentTarget.Location, vect( 0, 0, 0 ), class'DamageType',, self ); 

	if( AttackImpactSound != none )
	{
		PlaySound( AttackImpactSound );
	}
}

function bool TargetIsInAttackRange( BS_Pawn Target )
{
	local bool isInRange;

	isInRange = VSize( Target.Location - Location ) < GetAdjustedAttackRadius() + GetCollisionRadius() + Target.GetCollisionRadius();

	return isInRange;	
}

function bool LocationIsInMovementRange( Vector loc )
{
	return BS_GameInfo( WorldInfo.Game ).leeroyJenkinsMode || BS_GameInfo( WorldInfo.Game ).Fixed || VSize( loc - Location ) <= GetAdjustedMovementRadius() + GetCollisionRadius();
}

event TakeDamage( int Damage, Controller InstigatedBy, Vector HitLocation, Vector Momentum, class<DamageType> DamageType, optional TraceHitInfo HitInfo, optional Actor DamageCauser )
{
	local int damageDealt;
	local float MomentumLength;
	local BS_PlayerController BSPC;
	BSPC = BS_PlayerController( BS_UnitController( self.Owner ).Owner );
	
	if( Damage > 0 )
	{
		damageDealt = Damage - ( Damage * ( DefenseRating + GetDefenseBonusPercent() ) );
	
		if( BS_GameInfo( WorldInfo.Game ).bInstantKill )
		{
			damageDealt = GetAdjustedHealth();
		}

		if( !BS_GameInfo( WorldInfo.Game ).greenLanternsMight )
			DamageAdjustedHealth( max( 0, damageDealt ) );
		
		bRecievedDamageThatHasntBeenShown = true;
		DamageRecieved = damageDealt;

		if( DamageCauser != self )
		{
			PlayHitAnimation();
			SpawnStatusEffectHitPS();
			SpawnBloodSplatter();
		}

		
		if( BSPC != none )
			BSPC.TotalDamageTaken += Abs( damageDealt );

		if( GetAdjustedHealth() <= 0 )
		{
			HandlePawnDeath( BS_UnitController( InstigatedBy ), HitLocation, DamageType, true );
		}
		else
		{

			if( HitSound != none && DamageCauser != self )
			{
				PlaySound( HitSound );
			}
		}		
	}

	else if( Damage == 0 )
	{
		if( DamageCauser != self )
		{
			SpawnStatusEffectHitPS();
		}
	}

	else
	{
		DamageAdjustedHealth( Damage );
		bRecievedDamageThatHasntBeenShown = true;
		DamageRecieved = Damage;
		if( BSPC != none )
			BSPC.TotalHealthHealed += Abs( Damage );
	}

	if( !bAffectedByImpulse )
		Momentum *= 0;

	MomentumLength = VSize2D( Momentum );
	if( MomentumLength < -0.5 || MomentumLength > 0.5 )
	{
		if( IsCombatLocked() )
		{
			if( CombatLockedWith != none )
			{
				CombatLockedWith.BreakCombatLock();
				CombatLockedWith = none;
			}
		}
		BreakCombatLock();
	}

	super.TakeDamage( 0, InstigatedBy, HitLocation, Momentum, DamageType, HitInfo, DamageCauser );
}

function BS_Pawn getClosestEnemy()
{
	local BS_Pawn enemy;
	local BS_Pawn closestEnemy;

	closestEnemy = none;

	foreach AllActors( class'BS_Pawn', enemy )
	{
		if( !enemy.bHidden && enemy.teamNumber != teamNumber && ( closestEnemy == none || VSize( closestEnemy.Location - Location ) > VSize( enemy.Location - Location ) ) )
		{
			closestEnemy = enemy; 
		}
	}

	return closestEnemy;
}

function bool IsCombatLocked()
{
    return bCombatLock;
}

function InitiateCombatLock( BS_Pawn target )
{
	if( !bCombatLock )
	{
		bJustEnteredCombatLock = true;
	}
	if( bIsMeleeUnit )
	{
		bCombatLock = false;
		if( combatLockedWith != none )
		{
			combatLockedWith.BreakCombatLock();
			combatLockedWith = none;
		}

		if( target.Health > 0 && !target.IsCombatLocked() )
		{
			if( target.WasCombatLockedBy( self ) )
			{
				bCombatLock = true;
				combatLockedWith = target;
			}
		}
	}
	WorldState.AddOrModifyBoolState( IS_LOCKED, true );
	LookAtLocation( target.Location );
	//ResetPlan();
}

function bool WasCombatLockedBy( BS_Pawn attacker )
{
	if( combatLockedWith == none && bCanBeCombatLocked )
	{
		combatLockedWith = attacker;
		if( !bCombatLock )
		{
			bJustEnteredCombatLock = true;
		}
		bCombatLock = true; 

		LookAtLocation( attacker.Location );
		WorldState.AddOrModifyBoolState( IS_LOCKED, true );

		return true;
	//	ResetPlan();
	}

	return false;
}

function BreakCombatLock()
{
	bJustEnteredCombatLock = false;
	bCombatLock = false;
	combatLockedWith = none;
	WorldState.AddOrModifyBoolState( IS_LOCKED, false );
	WorldState.AddOrModifyBoolState( HAS_TARGET, false );
	self.CurrentTarget = none;
	//ResetPlan();
}

function ResetPlan()
{
	local int i;

	while( Actions.Length > 0 )
		Actions.Remove( 0, 1 );	
	CurrentAction = none;

	for( i = 0; i < class'BS_State'.static.GetBoolPropertyCount(); ++i )
	{
		WorldState.AddOrModifyBoolState( EBoolPropertyKey( i ), false );
	}
	for( i = 0; i < class'BS_State'.static.GetIntPropertyCount(); ++i )
	{
		WorldState.AddOrModifyIntState( EIntPropertyKey( i ), 0 );
	}
}

function bool IsDead()
{
	return Health <= 0;
}

function bool CanMove()
{
	return bCanMove && GetAdjustedMovementRadius() > 0;
}

// Used so that the Turn Manager does not end the players turn until the last action the player issed is completed
function bool IsPerformingAction()
{
	return bIsAttacking || Owner.IsInState( 'Moving' ) || Owner.IsInState( 'MoveThenFireAbility' ) || Owner.IsInState( 'MoveToFinalDestination' ) || bIsUsingAbility || bAbilityStillFiring;
}

function DoAction()
{
	if( CurrentAction != none && !IsPerformingAction() )
	{
		if( CurrentAction.ActivateAction() )
		{
			Actions.Remove( 0, 1 );
			if( HasPlan() )
			{
				CurrentAction = Actions[0];
			}
		}
	}
}

//PH: Used to change the position of the Pawns circle once the move command has completed
function SetPositionOfUnitCircle()
{
	if( bIsSelected )
	{
		if( CurrentCirclePos != self.Location )
		{
			ActivateDeactivatePawnsInRange();

			CurrentCirclePos = self.Location;
		}		
	}
}

event OnAnimEnd( AnimNodeSequence SeqNode, float PlayedTime, float ExcessTime )
{
	local int i;

	super.OnAnimEnd( SeqNode, PlayedTime, ExcessTime );

	for( i = 0; i < Abilities.Length; ++i )
	{
		if( SeqNode.AnimSeqName == Abilities[i].AbilityAnimName )
		{
			bIsUsingAbility = false;
			bIsAttacking = false;
			break;
		}
	}

	if( SeqNode.AnimSeqName == AttackAnimName )
	{
		bIsAttacking = false;
	}

	if( SeqNode.AnimSeqName == DamagedAnimName )
	{
		bIsBeingHit = false;
	}

	//switch( SeqNode.AnimSeqName )
	//{
	//case Abilities[0].AbilityAnimName:
	//case Abilities[1].AbilityAnimName:
	//case Abilities[2].AbilityAnimName:
	//case Abilities[3].AbilityAnimName:
	//	bIsUsingAbility = false;
	//	bIsAttacking = false;
	//	break;
	//case AttackAnimName:
	//	bIsAttacking = false;
	//	break;
	//case DamagedAnimName:
	//	bIsBeingHit = false;
	//	break;
	//}
}

function PlayAttackAnim()
{
	if( AttackAnimNode != none )
	{
		AttackAnimNode.PlayCustomAnim( AttackAnimName, 1.f );
		bIsAttacking = true;
		SetTimer( 5.0f );
	}
}

function PlayHitAnimation()
{
	if( HitAnimNode != none )
	{
		HitAnimNode.PlayCustomAnim( DamagedAnimName, 1.f,,0.5f,,true );
		bIsBeingHit = true;
		SetTimer( 5.0f );
	}
}

function PlayAbilityAnim( int index )
{
	if( index < Abilities.Length )
	{
		AbilityAnimNode.PlayCustomAnim( Abilities[ index ].AbilityAnimName, 1.f );
		bIsUsingAbility = true;
		SetTimer( 5.0f );
	}
}

function SpawnAttackParticles()
{
	if( AttackOnUnitPSTemplate != none )
	{
		WorldInfo.MyEmitterPool.SpawnEmitter( AttackOnUnitPSTemplate, self.Location + AttackOnUnitPSOffset,,,self );
	}
	SetTimer( WindUpDelay, false, 'SpawnAttackParticleOnTarget' );
}

function SpawnAttackParticleOnTarget()
{
	if( AttackAtTargetPSTemplate != none )
	{
		WorldInfo.MyEmitterPool.SpawnEmitter( AttackAtTargetPSTemplate, CurrentTarget.Location + AttackAtTargetPSOffset,,,self );
	}
}

function SpawnBloodSplatter()
{
	if( BloodSplatterPSTemplate != none )
	{
		WorldInfo.MyEmitterPool.SpawnEmitter( BloodSplatterPSTemplate, Location + BloodSplatterPSOffset,BloodSplatterPSRotation,,self );
	}
}

function SpawnStatusEffectHitPS()
{
	local int i;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		if( UnitStatusEffects[i].PSTemplateToPlayWhenHit != none )
			UnitStatusEffects[i].SpawnHitPS();
	}
}

function bool IsAttackAnimPlaying()
{
	return bIsAttacking;
}

function bool IsBeingHitAnimPlaying()
{
	return bIsBeingHit;
}

function SelectPawn()
{
	bIsSelected = true;
	TriggerGlobalEventClass( class'BS_SeqEvent_PawnSelected', self );
	CirclePainter.DrawCircle( BLUE, GetAdjustedMovementRadius(), CurrentCirclePos );	 
	CirclePainter.DrawCircle( RED, GetAdjustedAttackRadius(), Location );
	ActivateDeactivatePawnsInRange();
}

function DeselectPawn()
{
	if( bIsSelected )
	{
		bIsSelected = false;   
		bIsInFutureMode = false;
		DeactivateAllPawnsInRange();
		CirclePainter.DeactivateAllDecals();	
	}
}

function UpdateCombatLockPS()
{	
	if( bCombatLock )
	{
		//DrawDebugSphere( Location ,16 ,20 ,0 ,255 , 0, false );
		if( CombatLockShieldPS != none )
		{
			if( !CombatLockShieldPS.ParticleSystemComponent.bIsActive )
				CombatLockShieldPS.ParticleSystemComponent.ActivateSystem();
			CombatLockShieldPS.SetLocation( self.Location + CombatLockPSOffset );
		}

		if( combatLockedWith != none )
		{
			if( CombatLockBeamPS != none )
			{
				if( !CombatLockBeamPS.ParticleSystemComponent.bIsActive )
				{
					CombatLockBeamPS.ParticleSystemComponent.ActivateSystem();
				}
				CombatLockBeamPS.SetLocation( self.Location );
				//CombatLockBeamPS.ParticleSystemComponent.SetVectorParameter( 'Name of End Pos', engagedWith.Location );
				//Direction = engagedWith.Location - self.Location;
				CombatLockBeamPS.SetRotation( Rotator( combatLockedWith.Location - self.Location ) );
			}
		}
	}
	else
	{
		if( CombatLockShieldPS != none )
		{
			if( CombatLockShieldPS.ParticleSystemComponent.bIsActive )
				CombatLockShieldPS.ParticleSystemComponent.DeactivateSystem();	
		}
			
		if( CombatLockBeamPS != none )
		{
			if( CombatLockBeamPS.ParticleSystemComponent.bIsActive )
			{
				CombatLockBeamPS.ParticleSystemComponent.DeactivateSystem();
			}
		}
	}
}

function UpdateHoverOverPS( float dt )
{
	local Vector animOffset;
	if( HoverOverRingPS != none )
	{
		animOffset.Z = CurrentHoverOverBeamHeightOffset;
		HoverOverBeamPS.SetLocation( self.Location + HoverOverPSOffset + animOffset );
		
		animOffset.Z = CurrentHoverOverRingHeightOffset;
		HoverOverRingPS.SetLocation( self.Location + HoverOverRingPSOffset + animOffset );
		HoverOverRingPS.ParticleSystemComponent.SetScale( CurrentRingAnimScale );
		
		if( bIsBeingHoveredOver )
		{
			//Ring offset anim
			if( CurrentHoverOverRingHeightOffset > 0.0f )
			{
				CurrentHoverOverRingHeightOffset -= 4.0f * HoverOverRingAnimStartHeight * dt;
			}
			else
			{
				CurrentHoverOverRingHeightOffset = 0.0f;
				
				//if( HoverOverAnimTimer < 0.0f )
				//	CurrentHoverOverRingHeightOffset = HoverOverRingAnimStartHeight;
			}

			//Ring scale anim
			if( CurrentRingAnimScale > 1.0f )
			{
				CurrentRingAnimScale -= 4.0f * ( MaxRingAnimScale  - 1.0f )* dt;
			}
			else
			{
				CurrentRingAnimScale = 1.0f;
				//if( HoverOverAnimTimer < 0.0f )
				//{
				//	CurrentRingAnimScale = MaxRingAnimScale;
				//}
			}

			//Anim timer for npc hover over anim
			//if( HoverOverAnimTimer > 0.0f )
			//	HoverOverAnimTimer -= dt;
			//else
			//{
			//	HoverOverAnimTimer = HoverOverAnimMaxTime;
			//	RingAnimScaleDir = -RingAnimScaleDir;
			//}
			
			//Beam offset anim
			if( CurrentHoverOverBeamHeightOffset > 0.0f )
			{
				CurrentHoverOverBeamHeightOffset -= 5.0f * HoverOverBeamAnimStartHeight * dt;
			}
			else
			{
				CurrentHoverOverBeamHeightOffset = 0.0f;
			}
		}
	}
}

function EnableHoverOverPS()
{
	local BS_GameInfo BSGI;
	local BS_PlayerController BSPC;
	local BS_HUD BSHUD;
	BSGI = BS_GameInfo( class'WorldInfo'.static.GetWorldInfo().Game );
	BSPC = BS_PlayerController( BSGI.TurnManager.GetController( 0 ) );
	BSHUD = BS_HUD( BSPC.myHUD );

	if( !bIsInPokeBall && HoverOverRingPS != none )
	{
		if( !HoverOverBeamPS.ParticleSystemComponent.bIsActive && BSHUD.TheHUDMovie.bPlayerHUDEnabled )
		{
			CurrentHoverOverRingHeightOffset = HoverOverRingAnimStartHeight;
			CurrentHoverOverBeamHeightOffset = HoverOverBeamAnimStartHeight;
			CurrentRingAnimScale = MaxRingAnimScale;
			RingAnimScaleDir = -1.0f;
			bIsBeingHoveredOver = true;
			HoverOverBeamPS.ParticleSystemComponent.ActivateSystem();
			
			if( !bIsNPC || !bIsRingPSActiveRegardlessOfHoverOver )
				HoverOverRingPS.ParticleSystemComponent.ActivateSystem();
		}
	}
}


function DisableHoverOverPS()
{
	if( HoverOverBeamPS.ParticleSystemComponent.bIsActive )
	{
		bIsBeingHoveredOver = false;
		HoverOverAnimTimer = HoverOverAnimMaxTime;
		HoverOverBeamPS.ParticleSystemComponent.DeactivateSystem();
		CurrentHoverOverRingHeightOffset = 0.0f;
		if( !bIsNPC || !bIsRingPSActiveRegardlessOfHoverOver )
			HoverOverRingPS.ParticleSystemComponent.DeactivateSystem();
	}
}

function ShowHealthBar()
{
	if( !bIsInPokeBall )
	{
		if( UnitHealthBar != none )
			UnitHealthBar.Show();
	}
}

function HideHealthBar()
{
	if( UnitHealthBar != none )
		UnitHealthBar.Hide();
}

function ShowHideHealthBar( bool show )
{
	bIsRingPSActiveRegardlessOfHoverOver = show;
	if( show )
	{
		ShowHealthBar();
		if( bIsNPC )
			HoverOverRingPS.ParticleSystemComponent.ActivateSystem();
	}
	else
	{
		HideHealthBar();
		if( bIsNPC )
			HoverOverRingPS.ParticleSystemComponent.DeactivateSystem();
	}
}

function ShowThinkingBar()
{
	if( AIThinkingBar != none )
		AIThinkingBar.Show();
}

function HideThinkingBar()
{
	if( AIThinkingBar != none )
		AIThinkingBar.Hide();
}

function ShowStatusEffects()
{
	local int i;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		UnitStatusEffects[i].KillPS();
	}
}

function HideStatusEffects()
{
	local int i;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		UnitStatusEffects[i].ActivatePS();
	}
}

function ApplyStatusEffect( BS_StatusEffect StatusEffectToApply )
{
	local int i;
	local bool AffectingUnit;
 
	StatusEffectToApply.AfflictedUnit = self;
	StatusEffectToApply.AttackPowerModifer = 1 + BS_Pawn( StatusEffectToApply.Owner ).GetAttackBonusPercent();
	AffectingUnit = false;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		if( UnitStatusEffects[i].StatusEffectName == StatusEffectToApply.StatusEffectName && UnitStatusEffects[i].Owner == StatusEffectToApply.Owner )
		{ 
			AffectingUnit = true;
			break;
		}
	}

	if( AffectingUnit )
	{
		StatusEffectToApply.CurrentNumberOfStacks = UnitStatusEffects[i].CurrentNumberOfStacks;
		UnitStatusEffects[i].KillPS();

		if( StatusEffectToApply.CurrentNumberOfStacks < StatusEffectToApply.MaximumNumberOfStacks )
		{
			++StatusEffectToApply.CurrentNumberOfStacks;
			UnitStatusEffects[i] = StatusEffectToApply;
		}
		else
		{
			UnitStatusEffects[i] = StatusEffectToApply;
		}
	}
	else
	{
		UnitStatusEffects[i] = StatusEffectToApply;
	}
}

function RemoveStatusEffect( BS_StatusEffect StatusEffectToRemove )
{
	UnitStatusEffects.RemoveItem( StatusEffectToRemove );
}

function bool OwnsStatusEffectOnUnit( BS_Pawn AfflictedUnit, name StatusEffectName )
{
	local int i;

	for( i = 0; i < AfflictedUnit.UnitStatusEffects.Length; ++i )
	{
		if( AfflictedUnit.UnitStatusEffects[i].StatusEffectName == StatusEffectName && AfflictedUnit.UnitStatusEffects[i].Owner == self )
			return true;
	}

	return false;
}

function ProcessStatusEffects( UpdateTime TurnState )
{
	local int i;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		if( UnitStatusEffects[i].WhenToTick == TurnState )
			UnitStatusEffects[i].UpdateStatusEffect();
	}
}

function bool HasDoT()
{
	local int i;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		return !UnitStatusEffects[i].IsHoT();
	}
}

function bool HasHoT()
{
	local int i;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		return UnitStatusEffects[i].IsHoT();
	}
}

function bool IsExhausted()
{
	return false;
}

function bool IgnoresCombatLock()
{
	local int i;

	if( bIsMeleeUnit )
		return true;
	else
	{
		for( i = 0; i < UnitStatusEffects.Length; ++i )
		{
			if( UnitStatusEffects[i].IgnoresCombatLock )
				return true;
		}		
	}

	return false;
}

function float GetMaxHealthBonusPercent()
{
	local int i;
	local float MaxHealthBonusPercent;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		MaxHealthBonusPercent += UnitStatusEffects[i].MaxHPBounsPercent * UnitStatusEffects[i].CurrentNumberOfStacks;
	}

	return MaxHealthBonusPercent;
}

function int GetAdjustedMaxHealth()
{
	local int i;
	local float MaxHealthBonusPercent;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		MaxHealthBonusPercent += UnitStatusEffects[i].MaxHPBounsPercent * UnitStatusEffects[i].CurrentNumberOfStacks;
	}

	return Max( MaxHealth + ( MaxHealth * MaxHealthBonusPercent ), 0.0f );
}

function int GetAdjustedHealth()
{
	local int i;
	local float MaxHealthBonusPercent;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		MaxHealthBonusPercent += UnitStatusEffects[i].MaxHPBounsPercent * UnitStatusEffects[i].CurrentNumberOfStacks;
	}

	return Health + ( Health * MaxHealthBonusPercent );
}

function DamageAdjustedHealth( int DamageTaken )
{
	local int AdjustedHealth;
	local float BonusHealthPercent;

	BonusHealthPercent = 1.0f + GetMaxHealthBonusPercent();

	AdjustedHealth = Health * BonusHealthPercent;

	AdjustedHealth -= DamageTaken;

	AdjustedHealth = min( AdjustedHealth, MaxHealth * BonusHealthPercent );

	Health = AdjustedHealth * ( 1.0f / BonusHealthPercent );
}

function int GetAdjustedMaxAP()
{
	local int i;
	local float MaxAPBonusPercent;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		MaxAPBonusPercent += UnitStatusEffects[i].MaxAPBounsPercent * UnitStatusEffects[i].CurrentNumberOfStacks;
	}

	return Max( MaxActionPoints * ( 1 + MaxAPBonusPercent ), 0 );
}

function int GetAdjustedAttackPower()
{
	local int i;
	local float AttackBonusPercent;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		AttackBonusPercent += UnitStatusEffects[i].AttackBonusPercent * UnitStatusEffects[i].CurrentNumberOfStacks;
	}

	return Max( AttackPower + ( AttackPower * AttackBonusPercent ), 0.0f );
}

function float GetAttackBonusPercent()
{
	local int i;
	local float AttackBonusPercent;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		AttackBonusPercent += UnitStatusEffects[i].AttackBonusPercent * UnitStatusEffects[i].CurrentNumberOfStacks;
	}

	return AttackBonusPercent;
}

function float GetDefenseBonusPercent()
{
	local int i;
	local float DefenseBonusPercent;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		DefenseBonusPercent += UnitStatusEffects[i].DefenseBonusPercent * UnitStatusEffects[i].CurrentNumberOfStacks;
	}

	return DefenseBonusPercent;
}

function float GetAdjustedAttackRadius()
{
	local int i;
	local float AttackRadiusBonusPercent;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		AttackRadiusBonusPercent += UnitStatusEffects[i].AttackRadiusBonusPercent * UnitStatusEffects[i].CurrentNumberOfStacks;
	}

	return Max( AttackRadius + ( AttackRadius * AttackRadiusBonusPercent ), 0.0f ); //50.0
}

function float GetAdjustedMovementRadius()
{
	local int i;
	local float MoveRadiusBonusPercent;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		MoveRadiusBonusPercent += UnitStatusEffects[i].MoveRadiusBonusPercent * UnitStatusEffects[i].CurrentNumberOfStacks;
	}

	return Max( MoveRadius + ( MoveRadius * MoveRadiusBonusPercent ), 0.0f );
}

function float GetThornDamagePercent()
{
	local int i;
	local float ThornPercentDamage;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		ThornPercentDamage += UnitStatusEffects[i].ThornPercentDamage * UnitStatusEffects[i].CurrentNumberOfStacks;
	}

	return ThornPercentDamage;
}

function float GetProtectionPercent( out BS_Pawn Protector )
{
	local int i;
	local float ProtectionPercent;

	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		if( UnitStatusEffects[i].ProtectionPercent > 0 )
		{
			Protector = BS_Pawn( UnitStatusEffects[i].Owner );
			ProtectionPercent = UnitStatusEffects[i].ProtectionPercent;
			break;
		}
	}

	return ProtectionPercent;
}

function bool BelongsToController( Controller controllerInQuestion )
{
	return BS_UnitController( Controller ).Controller == controllerInQuestion;
}

function bool IsPointReachable( Vector Point, bool bCollideUnits )
{
	return BS_UnitController( Controller ).IsPointReachable( Point, bCollideUnits );
}

function bool IsTargetReachable( Actor Target, bool bCollideUnits )
{
	return BS_UnitController( Controller ).IsTargetReachable( Target, bCollideUnits );
}

function bool HasReachDestination( Vector Point, float DestinationTolerance )
{
	return BS_UnitController( Controller ).HasReachedDestination( Point, DestinationTolerance );
}

function bool CanMoveTo( Vector MoveLocation )
{
	local bool ValidPosition;
	local BlockingVolume HitActor;

	ValidPosition = true;

	foreach OverlappingActors( class'BlockingVolume', HitActor, 17.0f, MoveLocation )
	{
		ValidPosition = false;
		break;
	}

	return ValidPosition;
}

function bool ValidDestination( Vector Destination )
{
	Destination.Z = Location.Z;
	return IsPointReachable( Destination, false ) || BS_UnitController( Controller ).GeneratePathTo( Destination,, true );
}

function bool ValidTarget( BS_Pawn Target )
{
	local bool targetValid;

	targetValid = Target.Health > 0 && TargetIsInAttackRange( Target );

	if( !bIsMeleeUnit && ProjArchetypeToFire != none )
	{
		targetValid = targetValid && FastTrace( Target.Location );
	}

	return targetValid;
}

function CrushedBy( Pawn OtherPawn )
{

}

function TakeFallingDamage()
{

}

function IWantToBeTheVeryBest()
{
	Abilities.Length = 0;
	Abilities.AddItem( Spawn( class'BS_AbilityMasterBall', self,,,, MasterBallArchetype ) );
	Abilities.AddItem( Spawn( class'BS_AbilityMasterBall', self,,,, MasterBallArchetype ) );
	Abilities.AddItem( Spawn( class'BS_AbilityMasterBall', self,,,, MasterBallArchetype ) );
	Abilities.AddItem( Spawn( class'BS_AbilityMasterBall', self,,,, MasterBallArchetype ) );
}

function Captured( BS_Pawn CapturedBy )
{
	TeamNumber = CapturedBy.TeamNumber;
	DisableHoverOverPS();
	HoverOverBeamPS.SetTemplate( CapturedBy.HoverOverBeamPSTemplate );
	HoverOverRingPS.SetTemplate( CapturedBy.HoverOverRingPSTemplate );
	HideHealthBar();
	bIsRingPSActiveRegardlessOfHoverOver = false;
	bIsNPC = false;
	if( IsCombatLocked() )
	{
		if( CombatLockedWith != none )
		{
			CombatLockedWith.BreakCombatLock();
			CombatLockedWith = none;
		}
	}
	BreakCombatLock();
	Withdraw();
}

function SendOut()
{
	bIsInPokeBall = false;
	EnableHoverOverPS();
	ShowHealthBar();
}

function Withdraw()
{
	bIsInPokeBall = true;
	DisableHoverOverPS();
	HideHealthBar();	
}

function LookAtLocation( Vector locToLookAt )
{
	local Vector direction;

	direction = locToLookAt - Location;	
	direction.Z = 0;
	direction = Normal( direction );	
	CurrentRotation = Rotator( direction );
}

function SetOcclusionMeshHidden( bool hidden )
{
	OccludedMesh.SetHidden( hidden );
}

state Dying
{
	
	event TakeDamage(int Damage, Controller EventInstigator, vector HitLocation, vector Momentum, class<DamageType> DamageType, optional TraceHitInfo HitInfo, optional Actor DamageCauser)
	{
		if( !bIsDesctrubleObject )
			super.TakeDamage( Damage, EventInstigator, HitLocation, Momentum, DamageType, HitInfo, DamageCauser );
	}

	event BeginState( Name PreviousStateName )
	{
		super.BeginState( PreviousStateName );
		LifeSpan = 2.0;
	}
}

event Timer()
{
	bIsUsingAbility = false;
	bIsAttacking = false;
	bIsBeingHit = false;
}

function bool IsStationaryType()
{
	return IsStationary;
}

function SpawnProj()
{
	Spawn( class'BS_Projectile', self,,self.Location + ProjStartPosOffset,, ProjArchetypeToFire ).FireAt( LocationToFireProjAt, false, 0 );	
}

function SetPawnToInitState()
{
	SetLocation( StartingPosition );
	SetRotation( StartingRotation );
}

function HandlePawnDeath( BS_UnitController InstigatedBy, Vector HitLocation, class<DamageType> DamageType, bool shouldSpawnDeadMesh )
{
	local int i;
	local BS_PlayerController BSPC;

	DeathAnimNode.PlayCustomAnim( DeathAnimName, 0.5f,2.0f,0.f,,true );

	TriggerGlobalEventClass( class'SeqEvent_Death', self );
	if( bIsDesctrubleObject )
		TearOffMomentum = vect(0,0,0);

	DeselectPawn();
	Died( InstigatedBy, DamageType, HitLocation );
	UnitHealthBar.Close();
	if( shouldSpawnDeadMesh && !IsTimerActive( 'SpawnDeadMesh' ) )
	{
		SetTimer( 2.0f, false, 'SpawnDeadMesh' );
	}

	BSPC = BS_PlayerController( InstigatedBy.Controller );
	BS_UnitController( Owner ).UnitKilled();
	if( InstigatedBy != none && BSPC != none )
		BSPC.NumEnemiesKilled++;

	if( combatLockedWith != none )
	{
		combatLockedWith.BreakCombatLock();
	}

	if( DeathSound != none )
	{
		PlaySound( DeathSound );
	}
	CombatLockShieldPS.ParticleSystemComponent.DeactivateSystem();
	CombatLockBeamPS.ParticleSystemComponent.DeactivateSystem();
	CombatLockShieldPS.Destroy();
	CombatLockBeamPS.Destroy();
	HoverOverRingPS.ParticleSystemComponent.DeactivateSystem();
	HoverOverRingPS.Destroy();
	for( i = 0; i < UnitStatusEffects.Length; ++i )
	{
		UnitStatusEffects[i].KillPS();
	}
}

function SetDyingPhysics()
{
	if( !bIsDesctrubleObject )
	{
		if( Physics != PHYS_RigidBody )
			SetPhysics(PHYS_RigidBody);
		super.SetDyingPhysics();
	}
}

//http://forums.epicgames.com/threads/732162-setting-up-ragdolls-for-death?p=27381057#post27381057
simulated function PlayDying( class<DamageType> DamageType, vector HitLoc )
{
  local vector ApplyImpulse, ShotDir;
  
  bReplicateMovement = false;
  bTearOff = true;
  Velocity += TearOffMomentum;
  SetDyingPhysics();
  bPlayedDeath = true;
  HitDamageType = DamageType; // these are replicated to other clients
  TakeHitLocation = HitLoc;
  if ( WorldInfo.NetMode == NM_DedicatedServer )
  {
    GotoState('Dying');
    return;
  }
  InitRagdoll();
  mesh.MinDistFactorForKinematicUpdate = 0.f;
  
  if (Physics == PHYS_RigidBody)
  {
    //@note: Falling instead of None so Velocity/Acceleration don't get cleared
    setPhysics(PHYS_Falling);
  }
  PreRagdollCollisionComponent = CollisionComponent;
  CollisionComponent = Mesh;
  if( Mesh.bNotUpdatingKinematicDueToDistance )
  {
    Mesh.ForceSkelUpdate();
    Mesh.UpdateRBBonesFromSpaceBases(TRUE, TRUE);
  }
  if( Mesh.PhysicsAssetInstance != None )
    Mesh.PhysicsAssetInstance.SetAllBodiesFixed(FALSE);
  Mesh.SetRBChannel(RBCC_Pawn);
  Mesh.SetRBCollidesWithChannel(RBCC_Default,TRUE);
  Mesh.SetRBCollidesWithChannel(RBCC_Pawn,TRUE);
  Mesh.SetRBCollidesWithChannel(RBCC_Vehicle,TRUE);
  Mesh.SetRBCollidesWithChannel(RBCC_Untitled3,FALSE);
  Mesh.SetRBCollidesWithChannel(RBCC_BlockingVolume,TRUE);
  Mesh.ForceSkelUpdate();
  Mesh.UpdateRBBonesFromSpaceBases(TRUE, TRUE);
  Mesh.PhysicsWeight = 1.0;
  Mesh.bUpdateKinematicBonesFromAnimation=false;
  // mesh.bPauseAnims=True;
  Mesh.SetRBLinearVelocity(Velocity, false);
  mesh.SetTranslation(vect(0,0,1) * 6);
  Mesh.ScriptRigidBodyCollisionThreshold = MaxFallSpeed;
  Mesh.SetNotifyRigidBodyCollision(true);
  Mesh.WakeRigidBody();
  if( TearOffMomentum != vect(0,0,0) )
  {
    ShotDir = normal(TearOffMomentum);
    ApplyImpulse = ShotDir * DamageType.default.KDamageImpulse;
    // If not moving downwards - give extra upward kick
    if ( Velocity.Z > -10 )
    {
      ApplyImpulse += Vect(0,0,1)*2;
    }
    Mesh.AddImpulse(ApplyImpulse, TakeHitLocation,, true);
  }
  GotoState('Dying');
}

function ForceRagdoll()
{
	mesh.MinDistFactorForKinematicUpdate = 0.f;

    InitRagdoll();
    Mesh.PhysicsAssetInstance.SetAllBodiesFixed(FALSE);
    Mesh.SetRBChannel(RBCC_Pawn);
    Mesh.SetRBCollidesWithChannel(RBCC_Default,TRUE);
    Mesh.SetRBCollidesWithChannel(RBCC_Pawn,TRUE);
    Mesh.SetRBCollidesWithChannel(RBCC_Vehicle,TRUE);
    Mesh.SetRBCollidesWithChannel(RBCC_Untitled3,FALSE);
    Mesh.SetRBCollidesWithChannel(RBCC_BlockingVolume,TRUE);
    Mesh.ForceSkelUpdate();
    Mesh.UpdateRBBonesFromSpaceBases(TRUE, TRUE);
	Mesh.PhysicsWeight = 1.0;
	Mesh.PhysicsAssetInstance.SetAllBodiesFixed(FALSE);
	Mesh.bUpdateKinematicBonesFromAnimation=false;
	mesh.bPauseAnims=True;

	Mesh.SetRBLinearVelocity(Velocity, false);

	mesh.SetTranslation(vect(0,0,1) * 6);
	Mesh.ScriptRigidBodyCollisionThreshold = MaxFallSpeed;
	Mesh.SetNotifyRigidBodyCollision(true);
	Mesh.WakeRigidBody();
}

function SpawnDeadMesh()
{
	local BS_DeadPawn SMATemp;
	SMATemp = Spawn( class'BS_DeadPawn',self,,Location + DeadMeshOffset );
	SMATemp.StaticMeshComponent.SetStaticMesh( MeshToSpawnOnDeath );
	SMATemp.LifeSpan = DeadMeshLifespan;	
	DamageDealtPopup.Close();
}

function OutOfAP()
{
	if( !bIsDesctrubleObject )
	{
		SetTimer( 1.0f, false, 'OutOfAPTimerFunc' );
	}
}

function OutOfAPTimerFunc()
{
	local BS_PlayerController BSPC;
	BSPC = BS_PlayerController( BS_UnitController( Owner ).Controller );
	if( OutOfActionPointsMaterial != none )
	{
		Mesh.SetMaterial( 0, OutOfActionPointsMaterial );
		Mesh.SetMaterial( 1, OutOfActionPointsMaterial );
	}
	if( BSPC != none && BSPC.bSelectNearestUnitWithAPWhenUnitIsOutOfAP )
	{
		BSPC.SelectNearestHeroToOtherHeroWithAP( self );
	}
	if( BSPC != none && BSPC.SelectedHero == self )
	{
		BSPC.SelectHero( none );
	}
	
	TriggerGlobalEventClass( class'BS_SeqEvent_PawnOutOfAP', self );	
}


function ActivatePawnInRange()
{
	if( InRangeMaterial != none && !bIsDesctrubleObject )
	{
		Mesh.SetMaterial( 0, InRangeMaterial );
		Mesh.SetMaterial( 1, InRangeMaterial );	
	}
}

function DeactivatePawnInRange()
{
	if( actionPoints > 0 && !bIsDesctrubleObject )
	{
		Mesh.SetMaterial( 0, DefaultMaterial );
		Mesh.SetMaterial( 1, DefaultMaterial );			
	}
	else
	{
		if( OutOfActionPointsMaterial != none && !bIsDesctrubleObject )
		{
			Mesh.SetMaterial( 0, OutOfActionPointsMaterial );
			Mesh.SetMaterial( 1, OutOfActionPointsMaterial );
		}
		else
		{
			Mesh.SetMaterial( 0, DefaultMaterial );
			Mesh.SetMaterial( 1, DefaultMaterial );		
		}
	}
}

function ActivateDeactivatePawnsInRange( optional BS_Ability AbilityToUse )
{
	local BS_Pawn BSPawn;
	local BS_PlayerController BSPC;
	local BS_Ability BSAbilityToUse;
	local BS_AttribBoostAbility BSABA;
	local float AdjAttackRadius;
	local float effectRadius;
	BSPC = BS_PlayerController( BS_UnitController( Owner ).Controller );
	
	if( BSPC != none && AbilityToUse != none )
	{
		BSAbilityToUse = AbilityToUse;			
	}
	else if( BSPC != none )
	{	
		if( BSPC.CurrentAction == ACTION_ABILITY )
		{
			BSAbilityToUse = Abilities[ BSPC.CurrentAbilityIndex ];
		}
	}

	foreach AllActors( class'BS_Pawn', BSPawn )
	{
		if( BSPawn != self && BSPawn.TeamNumber != TeamNumber )
		{
			if( BSAbilityToUse != none )
			{
				if( BSAbilityToUse.CanUseAbility( BSPawn, BSPawn.Location ) )
				{
					BSABA = BS_AttribBoostAbility( BSAbilityToUse );
					if( BSABA != none )
					{
						effectRadius = ( ( BSABA.StanceEffect.AttackRadiusBonusPercent > 0.0f ) ?  ( AttackRadius * BSABA.StanceEffect.AttackRadiusBonusPercent ) : BSABA.StanceEffect.EffectRadius );
						if( BSABA.bInStance )
						{
							AdjAttackRadius = GetAdjustedAttackRadius() - effectRadius;
						}
						else
						{
							AdjAttackRadius = GetAdjustedAttackRadius() + effectRadius;
						}
						if( VSize( BSPawn.Location - Location ) <= AdjAttackRadius + GetCollisionRadius() + BSPawn.GetCollisionRadius() )
						{
							if( !bIsMeleeUnit && ProjArchetypeToFire != none )
							{
								if( FastTrace( BSPawn.Location ) )
								{
									BSPawn.ActivatePawnInRange();
								}
								else
								{
									BSPawn.DeactivatePawnInRange();
								}
							}
						}
						else
						{
							BSPawn.DeactivatePawnInRange();
						}
					}
					else if( BSAbilityToUse.AbilityType == ABILITY_TYPE_RADIAL )
					{
						if( BSAbilityToUse.TargetType == TARGET_TYPE_ENEMIES )//&& VSize( BSPawn.Location - Location ) <= BSAbilityToUse.GetEffectRadius() + GetCollisionRadius() )
							BSPawn.ActivatePawnInRange();					
					}
					else if( BSAbilityToUse.AbilityType != ABILITY_TYPE_PLACED )
					{
						if( BSAbilityToUse.TargetType == TARGET_TYPE_ENEMIES )//&& VSize( BSPawn.Location - Location ) <= BSAbilityToUse.GetAbilityRange() + GetCollisionRadius() )
							BSPawn.ActivatePawnInRange();
					}
					else
					{
						BSPawn.DeactivatePawnInRange();
					}
				}
				else
				{
					BSPawn.DeactivatePawnInRange();
				}
			}
			else
			{
				if( ValidTarget( BSPawn ) )
				{
					BSPawn.ActivatePawnInRange();
				}
				else
				{
					BSPawn.DeactivatePawnInRange();
				}
			}
		}
		else //Same team
		{
			if( BSAbilityToUse != none )
			{
				BSABA = BS_AttribBoostAbility( BSAbilityToUse );
				if( BSABA != none )
				{
					if( BSABA.StanceEffect.TargetType == TARGET_TYPE_ALLIES || BSABA.StanceEffect.TargetType == TARGET_TYPE_ALL )
					{
						effectRadius = ( ( BSABA.StanceEffect.AttackRadiusBonusPercent > 0.0f ) ?  ( AttackRadius * BSABA.StanceEffect.AttackRadiusBonusPercent ) : BSABA.StanceEffect.EffectRadius );
						
						if( BSAbilityToUse.CanUseAbility( BSPawn ) && VSize( BSPawn.Location - Location ) < effectRadius + GetCollisionRadius() )
						{
							BSPawn.ActivatePawnInRange();		
						}
						else
						{
							BSPawn.DeactivatePawnInRange();
						}
					}
				}
				else if( BSAbilityToUse.TargetType == TARGET_TYPE_ALLIES || BSAbilityToUse.TargetType == TARGET_TYPE_ALL )
				{
					if( BSAbilityToUse.AbilityType != ABILITY_TYPE_PLACED && BSAbilityToUse.CanUseAbility( BSPawn ) && VSize( BSPawn.Location - Location ) < BSAbilityToUse.GetAbilityRange() + GetCollisionRadius() )
					{
						BSPawn.ActivatePawnInRange();		
					}
					else
					{
						BSPawn.DeactivatePawnInRange();
					}
				}
			}
			else
			{
				BSPawn.DeactivatePawnInRange();
			}
		}
	}
}

function DeactivateAllPawnsInRange()
{
	local BS_Pawn BSPawn;
	foreach AllActors( class'BS_Pawn', BSPawn )
	{
		BSPawn.DeactivatePawnInRange();
	}
}

function ActivateInRangeOfPlacedAbility( Vector loc, float radius, BS_Ability.TargetTypes targetType )
{
	local BS_Pawn BSPawn;
	foreach AllActors( class'BS_Pawn', BSPawn )
	{
		if( ( targetType == TARGET_TYPE_ENEMIES || targetType == TARGET_TYPE_ALL ) && BSPawn.TeamNumber != TeamNumber )
		{
			if( VSize2D( BSPawn.Location - loc ) < radius + BSPawn.GetCollisionRadius() )
			{
				BSPawn.ActivatePawnInRange();
			}
			else
			{
				BSPawn.DeactivatePawnInRange();
			}
		}		
		
		if( ( targetType == TARGET_TYPE_ALLIES || targetType == TARGET_TYPE_ALL ) && BSPawn.TeamNumber == TeamNumber )
		{
			if( VSize2D( BSPawn.Location - loc ) < radius + BSPawn.GetCollisionRadius() )
			{
				BSPawn.ActivatePawnInRange();
			}
			else
			{
				BSPawn.DeactivatePawnInRange();
			}
		}
	}
}

defaultproperties
{
	MaxHealth = 3
	MaxActionPoints = 2
	MoveRadius = 300
	AttackRadius = 100
	AttackPower = 1
	WindUpDelay = 0.0
	AttackSoundDelay = 0.0
	DefenseRating = 0.0
	TeamNumber = 0;
	actionPoints = 2;
	bIsMeleeUnit = true
	bCombatLock = false
	combatLockedWith = none
	CurrentTarget = none
	bIsSelected=false
	bIsNPC = false
	bIsUsingAbility = false
	bAbilityStillFiring = false
	bIsAttacking = false
	bIsBeingHit = false
	CombatLockPSOffset = (Z=70)
	PercentageLowHealth = 0.90 //0.42
	NumberOfDesirableTargets = 2;
	DesirableMinDistanceFromAllies = 700;
    //CurrentDestination = self.Location;

	Begin Object Name=CollisionCylinder
      CollisionHeight=+40.000000
      CollisionRadius=+30.000000;
    End Object

	Begin Object Class=SkeletalMeshComponent Name=MySkeletalMeshComponent
		SkeletalMesh=SkeletalMesh'bs_Chara_Bjorn.bs_chara_bjorn_walk'
		Animsets(0)=AnimSet'bs_Chara_Bjorn.BS_Chara_Bjorn_AnimSet'
		AnimTreeTemplate=AnimTree'BS_Chara_Anim.AnimTree.BS_Chara_AnimTree'
		Animations=none
		CollideActors=true
		BlockRigidBody=true
		bHasPhysicsAssetInstance=true
		bUpdateKinematicBonesFromAnimation=true
		MinDistFactorForKinematicUpdate=0.f
 	End Object
 	Mesh=MySkeletalMeshComponent
 	Components.Add(MySkeletalMeshComponent)


	Begin Object Class=SkeletalMeshComponent Name=OPawnSkeletalMeshComponent
		//SkeletaMesh=
		//Materials(0)=MaterialInstanceConstant'bs_Chara_Outline.Material.bs_Chara_Outline_Mat_INST'
		//Materials(1)=MaterialInstanceConstant'bs_Chara_Outline.Material.bs_Chara_Outline_Mat_INST'
		DepthPriorityGroup=SDPG_Foreground // Render the occluded skeletal mesh in the foreground layer
		ShadowParent=MySkeletalMeshComponent
		ParentAnimComponent=MySkeletalMeshComponent
		bCacheAnimSequenceNodes=false
		AlwaysLoadOnClient=true 
		AlwaysLoadOnServer=true
		bOwnerNoSee=true
		CastShadow=false // Casting shadows is not required
		BlockRigidBody=true
		bUpdateSkelWhenNotRendered=false
		bIgnoreControllersWhenNotRendered=true
		bUpdateKinematicBonesFromAnimation=true
		bCastDynamicShadow=false // Casting shadows is not required
		RBChannel=RBCC_Untitled3
		RBCollideWithChannels=(Untitled3=true)
		//LightEnvironment=MyLightEnvironment
		bOverrideAttachmentOwnerVisibility=true
		bAcceptsDynamicDecals=false
		bHasPhysicsAssetInstance=true
		TickGroup=TG_PreAsyncWork
		MinDistFactorForKinematicUpdate=0.2f
		bChartDistanceFactor=true
		RBDominanceGroup=20
		//MotionBlurScale=0.f
		bUseOnePassLightingOnTranslucency=true
		//bPerBoneMotionBlur=true
	End Object
	OccludedMesh=OPawnSkeletalMeshComponent
	Components.Add(OPawnSkeletalMeshComponent)
	SelectedMaterial = MaterialInstanceConstant'bs_Chara_Outline.Material.bs_Chara_Outline_Selected_Mat_INST'
	DeselectedMaterial = MaterialInstanceConstant'bs_Chara_Outline.Material.bs_Chara_Outline_Blue_Mat_INST'

	EnemyDeselectedMaterial = MaterialInstanceConstant'bs_Chara_Outline.Material.bs_Chara_Outline_Deselected_Mat_INST'


	ControllerClass = class'BS_UnitController'

	SupportedEvents.Add( class'SeqEvent_Death' )
	SupportedEvents.Add( class'BS_SeqEvent_PawnSelected' )

	AttackAnimName=bs_chara_attack
	DamagedAnimName=bs_chara_hit
	DeathAnimName=bs_chara_death
	HeightForHealthBarPos = 47.0f
	bRecievedDamageThatHasntBeenShown = false
	bIsInFutureMode = false

	//bCollideWorld = true
	//bCollideActors = true
	//bBlockActors = true
	//bCanJump = false
	//bJumpCapable = false

	CurrentAction = none;

	bShouldSkipTurn = false

	HoverOverPSOffset=( X=0, Y=0, Z=-47 )
	ProjSpawnDelay = 0.1
	HoverOverRingPSOffset = (Z=-20)
	bIsRingPSActiveRegardlessOfHoverOver = true
	UnitVelocityMag = 100.0f 

	bCanMove = true
	bCanAttack = true
	bUseHealthBar = true
	bUseDamagePopups = true
	bAffectedByImpulse = true
	bCanBeCombatLocked = true
	bIsDesctrubleObject =  false
	MeshToSpawnOnDeath=StaticMesh'BS_Midgard_01.bs_midgard_bonepile_mesh'
	DeadMeshLifespan=10.0f
	DeadMeshOffset=( X=0.0, Y=0.0, Z=-47.0 )

	bDrawingHoveredAbilityRange = false
	Objective = none;
	DesiredMaxDistanceFromObjective = 600;
	bJustEnteredCombatLock = false;

	MasterBallArchetype = BS_AbilityMasterBall'BS_SuperSecertPackage.SecertStuff.MasterBall'
	bIsInPokeBall = false
	IsStationary = false

	CurrentHoverOverRingHeightOffset = 0.0f
	HoverOverRingAnimStartHeight = 30.0f;

	CurrentHoverOverBeamHeightOffset = 0.0f
	HoverOverBeamAnimStartHeight = 500.0f
	CurrentRingAnimScale = 1.0f;
	MaxRingAnimScale = 1.3f
	bIsBeingHoveredOver = false

	HoverOverAnimTimer = 1.0f
	HoverOverAnimMaxTime = 1.0f
	RingAnimScaleDir = -1.0f;
}						  

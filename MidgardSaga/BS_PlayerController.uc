//=============================================================================
// BS_PlayerController
//
// By: Andrew Nguyen
//=============================================================================

class BS_PlayerController extends SimplePC;

enum PlayerAction
{
	ACTION_MOVEATTACK,
	ACTION_ABILITY
};

var ( Player ) BS_PlayerProperties Properties;

var byte TeamNumber;
var BS_Pawn SelectedHero;
var BS_Ability SelectedAction;
var BS_TurnManager TurnManager;

var array< BS_Pawn > units;

var PlayerAction CurrentAction;
var int CurrentAbilityIndex;

var array< name > OrderOfUnitNames;

//End of game stats
var int NumEnemiesKilled;
var int TotalAPSpent;
var int NumHerosDied;

//Other random stats to collect
var float TotalDamageTaken;
var float TotalHealthHealed;
var float TotalDamageDealt;

var bool bUseArrayToDetermineUnitsThatCanBeSelected;
var array< BS_Pawn > PawnsThatCanBeSelected;

var bool bControlIsDown;

var bool bSelectNearestUnitWithAPWhenUnitIsOutOfAP;

var array<float> FastForwardSpeed;
var int CurrentFastForwardIndex;

var float bsDoubleClickTimer;
var const float MAX_DOUBLE_CLICK_TIME;

function BeginTurn()
{
	local int i;

	for( i = 0; i < units.Length; ++i )
	{
		units[i].actionPoints = units[i].GetAdjustedMaxAP();
		units[i].Mesh.SetMaterial( 0, units[i].DefaultMaterial );
		units[i].Mesh.SetMaterial( 1, units[i].DefaultMaterial );
	}

	for( i = 0; i < units.Length; ++i )
	{
		units[i].ProcessStatusEffects( START_OF_TURN );
		if( units[i].bJustEnteredCombatLock )
		{
			units[i].bJustEnteredCombatLock = false;
		}
	}

	if( units.Length == 0 )
	{
		`log( "You lost!" );
		//turnManager.Quit();
	}
	if( TurnManager.CurrentTurn != 0 )
		SelectFirstHeroThatIsAlive();
	SnapCameraToHero();
}

function EndTurn()
{
	local int i;

	SelectHero( none );
	CurrentAction = ACTION_MOVEATTACK;

	for( i = 0; i < units.Length; ++i )
	{
		units[i].ProcessStatusEffects( END_OF_TURN );
	}
}

simulated function PostBeginPlay()
{
	local BS_Pawn unit;
	local BS_TurnManager tm;

	super.PostBeginPlay();

	foreach AllActors( class'BS_Pawn', unit )
	{
		if( !unit.bIsNPC )
		{
			units[units.Length] = unit;
			BS_UnitController( unit.Owner ).Controller = self;
			unit.PlayerController = self;
		}
	}

	foreach AllActors( class'BS_TurnManager', tm )
	{
		TurnManager = tm;
		tm.PlayerController = self;
	}
	
}

event Tick( float DeltaTime )
{
	super.Tick( DeltaTime );
	if( CurrentAction == ACTION_ABILITY && SelectedHero != none )
	{
		SelectedHero.CirclePainter.DrawCircle( RED, SelectedHero.Abilities[CurrentAbilityIndex].Range, SelectedHero.CurrentCirclePos );	
	}
	bsDoubleClickTimer -= DeltaTime;
}

function AddUnit( BS_Pawn unitToAdd )
{
	local int i;
	local bool hasUnit;

	hasUnit = false;

	for( i = 0; i < units.Length; ++i )
	{
		if( units[i] == unitToAdd )
		{
			hasUnit = true;
			break;
		}
	}

	if( unitToAdd != none && !hasUnit )
	{
		units[i] = unitToAdd;
	}
}

function RemoveUnit( BS_Pawn unitToRemove )
{
	units.RemoveItem( unitToRemove );
}

exec function SnapCameraToHero()
{
	local BS_Camera camera;

	camera = BS_Camera(PlayerCamera);
	if (camera != None && SelectedHero != none )
	{
		camera.SetDesiredCameraLocation(SelectedHero.Location);
	}
}

function SnapCameraToUnit( BS_Pawn Unit )
{
	local BS_Camera camera;

	camera = BS_Camera(PlayerCamera);
	if (camera != None && Unit != none )
	{
		camera.SetDesiredCameraLocation(Unit.Location);
	}
}

function SnapCameraToPosition( Vector position )
{
	local BS_Camera camera;

	camera = BS_Camera( PlayerCamera );
	if( camera != none )
	{
		camera.SetDesiredCameraLocation( position );
	}
}

function SnapCameraToActor( Actor actor )
{
	local BS_Camera camera;
	camera = BS_Camera( PlayerCamera );
	if( camera != none && actor != none )
	{
		camera.SetDesiredCameraLocation( actor.Location );
	}
}

//PH: I edited this so that if the unit to select is none it deselects the current unit
function SelectHero( BS_Pawn Hero )
{
	local BS_HUD bsHUD;
	bsHUD = BS_HUD( myHUD );
	
	if( SelectedHero != none )
	{
		SelectedHero.DeselectPawn();		
		CurrentAction = ACTION_MOVEATTACK;
	}

	SelectedHero = Hero;
	if( SelectedHero != none )
	{
		SelectedHero = Hero;
		bsHUD.SetCurrentUnitSelection( Hero );
		SelectedHero.SelectPawn();
		//SnapCameraToHero();
	}
	else
	{
		bsHUD.SetCurrentUnitSelection( none );
	}
}

function bool HasUnitWithAction()
{
	local int i;
	local bool hasAction;

	hasAction = false;

	for( i = 0; i < units.Length; ++i )
	{
		if( units[i].actionPoints > 0 || units[i].IsPerformingAction() )
		{
			hasAction = true;
			break;
		}
	}

	return hasAction;
}

function SelectFirstHeroThatIsAlive()
{
	local int nameIndex;
	local int unitIndex;
	local bool unitFound;
	unitFound = false;

	for( nameIndex = 0; nameIndex < OrderOfUnitNames.Length && !unitFound; ++nameIndex )
	{
		for( unitIndex = 0; unitIndex < units.Length; ++unitIndex )
		{
			if( OrderOfUnitNames[ nameIndex ] == units[ unitIndex ].UnitName )
			{
				SelectHero( units[ unitIndex ] );
				unitFound = true;
				break;
			}
		}
	}	
}

function SelectNearestHeroToOtherHeroWithAP( BS_Pawn otherHero )
{
	local int heroIdx;
	local int nearestIdx;
	local float closestDistSq;
	local float tempDistSq;
	nearestIdx = -1;
	closestDistSq = 2000000000000.0f;
	for( heroIdx = 0; heroIdx < units.Length; ++heroIdx )
	{
		if( units[ heroIdx ] != otherHero )
		{
			tempDistSq = VSizeSq( units[ heroIdx ].Location - otherHero.Location );
			if( tempDistSq < closestDistSq && units[ heroIdx ].actionPoints > 0 )
			{
				nearestIdx = heroIdx;
				closestDistSq = tempDistSq;
			}
		}
	}
	if( nearestIdx != -1 )
	{
		SelectHero( units[ nearestIdx ] );
		SnapCameraToHero();
	}
}

//hack function for testing
exec function AdvanceDialogue()
{
	local GFxCLIKWidget.EventData params;
	BS_HUD( myHUD ).TheHUDMovie.AdvanceCurrentDialogue( params );
}

function IncreaseTimeSpeed()
{
	++CurrentFastForwardIndex;
	if( CurrentFastForwardIndex >= FastForwardSpeed.Length )
		CurrentFastForwardIndex = 0;
	WorldInfo.TimeDilation = FastForwardSpeed[ CurrentFastForwardIndex ];
}

function float GetTimeSpeed()
{
	return FastForwardSpeed[ CurrentFastForwardIndex ];
}

function ResetTimeSpeed()
{
	CurrentFastForwardIndex = 0;
	WorldInfo.TimeDilation = FastForwardSpeed[ CurrentFastForwardIndex ];
	BS_HUD( myHUD ).TheHUDMovie.ResetSpeedButton();
}

// Default properties block
defaultproperties
{
	CameraClass=class'Bjeardsoft.BS_Camera'
	InputClass=class'BS_PlayerInput'

	CurrentAbilityIndex = 0;
	CurrentAction = ACTION_MOVEATTACK
	//Properties=BS_PlayerProperties'Bjeardsoft.Properties.PlayerProperties'

	OrderOfUnitNames(0)=Bjorn
	OrderOfUnitNames(1)=Dag
	OrderOfUnitNames(2)=Freya
	OrderOfUnitNames(3)=Iliana

	bUseArrayToDetermineUnitsThatCanBeSelected = false
	bControlIsDown = false;
	bSelectNearestUnitWithAPWhenUnitIsOutOfAP = false

	FastForwardSpeed(0)=1.0f;
	FastForwardSpeed(1)=2.0f;
	FastForwardSpeed(2)=3.0f;
	FastForwardSpeed(3)=4.0f;

	CurrentFastForwardIndex=0
	TeamNumber = 0

	bsDoubleClickTimer = 2.0f
	MAX_DOUBLE_CLICK_TIME = 2.0f

	
	TotalDamageTaken = 0.0f
	TotalHealthHealed = 0.0f
	TotalDamageDealt = 0.0f
}

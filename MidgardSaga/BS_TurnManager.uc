///////////////////////////////////////////////////////////////////////////
// BS_TurnManager.uc
// Controls turn actions
// By: Andrew Nguyen
// 7/3/2013
///////////////////////////////////////////////////////////////////////////

class BS_TurnManager extends Actor;

enum TurnPhase
{
	TURN_START,
	TURN_END
};

enum PathFindingErrors
{
	START_NODE_NOT_FOUND,
	OUT_OF_NODES_TO_TRAVERSE,
	NO_ERROR_OCCURED
};

struct TurnEventWrapper
{
	var BS_TurnEvent turnEvent;
	var TurnPhase phaseToFire;
	var byte turnInterval;
};

var byte CurrentTurn;
var int CurrentPlayersTurn;
var int NumberOfPlayers;
var int WinnerIndex;
var array<Controller> Controllers;

var array< BS_PathNode > NavNodes;
var int PathID;

var BS_Pawn archetype;

var array< TurnEventWrapper > TurnEvents;

var BS_AIController aiController;
var BS_PlayerController playerController;

var bool bShowHealthBars;

var float TimeDelayBeforeAITurnStart;
var bool bGameStarted;
var bool bGamePaused;

var PathFindingErrors LastPathingError;

var bool bCanEndTurnEarly;

//////////////////////////////////////////////////////////////////
// SetTimer-like functionality
// - Function callbacks must of void footprint: funcName()
// - turnToFire: turn number to fire event
// - phaseToFire: specify start of end of turn
// - inbLoop: Should event loop or fire once
//////////////////////////////////////////////////////////////////
function RegisterEvent( delegate< BS_TurnEvent.EventCallback > callbackFunc, byte turnsUntilFire, TurnPhase phaseToFire, optional bool inbLoop=false )
{
	local TurnEventWrapper newEvent;
	newEvent.turnInterval = turnsUntilFire;
	newEvent.turnEvent.callbackFunction = callbackFunc;
	newEvent.turnEvent.turnToFire = turnsUntilFire + CurrentTurn;
	newEvent.turnEvent.isLooping = inbLoop;
	newEvent.phaseToFire = phaseToFire;

	TurnEvents.AddItem( newEvent );
}

simulated function PreBeginPlay()
{
	super.PreBeginPlay();
}

simulated function PostBeginPlay()
{
	local NavigationPoint NavPoint;
	local BS_PathNode NavNode;
	//local int i, j;

	super.PostBeginPlay();

	Controllers[Controllers.Length] = Spawn( class'BS_AIController', self );

	NumberOfPlayers = Controllers.Length;

	foreach AllActors( class'NavigationPoint', NavPoint )
	{
		NavNode = Spawn( class'BS_PathNode' );
		NavNode.NavPoint = NavPoint;
		NavNodes.AddItem( NavNode );
		//`log( "Added Point " $ NavPoint.Name );
	}

	//for( i = 0; i < NavNodes.Length; ++i )
	//{
	//	`log( "Neighbors for NavPoint : " $ NavNodes[i].NavPoint.Name );
	//	for( j = 0; j < NavNodes[i].NavPoint.PathList.Length; ++j )
	//	{
	//		`log( NavNodes[i].NavPoint.PathList[j].GetEnd().Name );
	//	}
	//}
}

function bool CanTakeAction( BS_Pawn unit, byte apCost )
{
	if( unit.actionPoints >= apCost )
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 
// Checks if pawn action is valid
// @ Returns true if action is taken and false if not
// Subtracts action points
// 
function bool UseAction( BS_Pawn unit, byte StandardAPCost, optional int abilityIndex = -1 )
{
	local bool useAction;
	local int actionPointsToUse;
	local BS_PlayerController BSPC;
	useAction = false;	
	actionPointsToUse = 0;
	
	if( unit.teamNumber == CurrentPlayersTurn )  // invalid action if not pawn's turn
	{
		if( BS_GameInfo( WorldInfo.Game ).supermanMode ) 
			unit.actionPoints = unit.MaxActionPoints;

		if( abilityIndex >= 0 )
		{
			if( unit.actionPoints >= unit.Abilities[abilityIndex].GetAPCost() )
			{
				actionPointsToUse = unit.Abilities[abilityIndex].GetAPCost();
				useAction = true;
			}

			else if( unit.IsCombatLocked() )
			{
				BroadcastLocalizedMessage( class'BS_LocalMessage', LM_NOT_ENOUGH_AP );	
			}
			else
			{
				BroadcastLocalizedMessage( class'BS_LocalMessage', LM_NOT_ENOUGH_AP );
			}
		}

		else if( ( !unit.IgnoresCombatLock() && unit.bCombatLock ) || unit.IsExhausted() )    
		{
			if( unit.actionPoints > 1 && StandardAPCost > 0 )
			{
				actionPointsToUse = 2;
				useAction = true;
			}
			else if( StandardAPCost > 0 )
			{
				BroadcastLocalizedMessage( class'BS_LocalMessage', LM_NOT_ENOUGH_AP );							
			}
		}

		else
		{
			if( unit.actionPoints != 0 && unit.actionPoints >= StandardAPCost )
			{
				actionPointsToUse = StandardAPCost;
				useAction = true;
			}
			else
			{
				BroadcastLocalizedMessage( class'BS_LocalMessage', LM_NOT_ENOUGH_AP );						
			}
		}	
	}

	else
	{			
		BroadcastLocalizedMessage( class'BS_LocalMessage', LM_NOT_PLAYERS_TURN );
	}

	if( !BS_GameInfo( WorldInfo.Game ).supermanMode )
		unit.actionPoints -= actionPointsToUse;

	BSPC = BS_PlayerController( BS_UnitController( unit.Owner ).Controller );
	if( BSPC != none )
		BSPC.TotalAPSpent += actionPointsToUse;
	if( unit.actionPoints <= 0 )
	{			
		unit.OutOfAP();
	}
	
	return useAction;
}

//PH: Added this so the AI wouldn't start immediately
function StartAITurn()
{
	`log( "Start AI Turn" );
	aiController.BeginTurn();
	gotostate( 'TakingTurn' );
}

function Controller GetController( byte TeamNumber )
{
	local int i;
	local BS_AIController tempAIController;

	for( i = 0; i < Controllers.Length; ++i )
	{
		tempAIController = BS_AIController( Controllers[i] );
		if( tempAIController != none && tempAIController.TeamNumber == TeamNumber )
		{
			return Controllers[i];
		}
		else if( BS_PlayerController( Controllers[i] ).TeamNumber == TeamNumber )
		{
			return Controllers[i];
		}
	}

	return none;
}

function EndTurnEarly( byte playerTeamNum )
{
	//local BS_AIController BSAI;
	//BSAI = BS_AIController( GetController( playerTeamNum ) );
	//if( BSAI != none )
	//	BSAI.GotoState( 'Idle' );
	if( playerTeamNum == CurrentPlayersTurn )
		gotostate( 'EndTurn' );
}

function Quit()
{
	ConsoleCommand( "exit", true );
	gotostate( 'GameOver' );
}

function GetPath( Pawn Pather, Vector End, out array< NavigationPoint > PathResult )
{
	local int i;
	local BS_PathNode StartingNavPoint, NodeToProcess, NeighborNode;
	local array< BS_PathNode > OpenList;
	local bool PathFound;

	PathFound = false;
	StartingNavPoint = GetReachableNavPointClosestToPawn( Pather );

	if( StartingNavPoint != none )
	{
		StartingNavPoint.SetOwner( none );
		StartingNavPoint.GCost = 0;
		StartingNavPoint.HCost = VSize2D( End - StartingNavPoint.NavPoint.Location );
		OpenList.AddItem( StartingNavPoint );

		while( OpenList.Length > 0 && !PathFound )
		{
			NodeToProcess = OpenList[OpenList.Length - 1];
			NodeToProcess.ClosedByPathID = PathID;
			OpenList.Remove( OpenList.Length - 1, 1 ); 

			if( DestinationReachableFromNavPoint( Pather, End, NodeToProcess.NavPoint ) )
			{
				PathFound = true;
				while( NodeToProcess != none )
				{
					PathResult.AddItem( NodeToProcess.NavPoint );
					NodeToProcess = BS_PathNode( NodeToProcess.Owner );
				}
				break;
			}

			for( i = 0; i < NodeToProcess.NavPoint.PathList.Length; ++i )
			{
				GetNavNodeFromNavPoint( NodeToProcess.NavPoint.PathList[i].GetEnd(), NeighborNode );

				if( NeighborNode.ClosedByPathID == PathID && ( NodeToProcess.GCost + 1 ) > NeighborNode.GCost )
				{
					continue;
				}

				if( NeighborNode.OpenedByPathID != PathID || NeighborNode.GCost > ( NodeToProcess.GCost + 1 ) )
				{
					NeighborNode.SetOwner( NodeToProcess );
					NeighborNode.GCost = NodeToProcess.GCost + 1;
					NeighborNode.HCost = VSize2D( End - NeighborNode.NavPoint.Location );

					if( NeighborNode.OpenedByPathID != PathID )
					{
						InsertNavNode( NeighborNode, OpenList );
					}
					else
					{
						EditNavNode( NeighborNode, OpenList );
					}

				}
			}
		}
	}

	++PathID;
}

function BS_PathNode GetReachableNavPointClosestToPawn( Pawn Unit )
{
	local BS_PathNode NearestNode;
	local int i;
	local float DistanceToPoint, DistanceToClosestPoint;

	for( i = 0; i < NavNodes.Length; ++i )
	{
		if( BS_Pawn( Unit ).IsPointReachable( NavNodes[i].NavPoint.Location, false ) )
		{
			DistanceToPoint =  VSize2D( Unit.Location - NavNodes[i].NavPoint.Location );
			if( NearestNode == none || ( NearestNode != none && DistanceToClosestPoint > DistanceToPoint ) )
			{
				DistanceToClosestPoint = DistanceToPoint;
				NearestNode = NavNodes[i];
			}
		}
	}

	return NearestNode;
}

function BS_PathNode GetReachableNavPointClosestToDestination( Pawn Unit, Vector Destination )
{
	local BS_PathNode NearestNode;
	local int i;
	local Actor HitActor;
	local Vector HitLocation, HitNormal, NavPointLocation;
	local float DistanceToPoint, DistanceToClosestPoint;

	for( i = 0; i < NavNodes.Length; ++i )
	{
		NavPointLocation = NavNodes[i].NavPoint.Location;
		NavPointLocation.Z = Unit.Location.Z;
		HitActor = Trace( HitLocation, HitNormal, NavPointLocation, Unit.Location, false, Unit.GetCollisionExtent() );
		if( HitActor == none )
		{
			DistanceToPoint =  VSize2D( Destination - NavNodes[i].NavPoint.Location );
			if( NearestNode == none || ( NearestNode != none && DistanceToClosestPoint > DistanceToPoint ) )
			{
				DistanceToClosestPoint = DistanceToPoint;
				NearestNode = NavNodes[i];
			}
		}
	}

	return NearestNode;
}

//TODO If this gets too slow, replace with a version where I store the neighbors in BS_PathNode
function GetNavNodeFromNavPoint( NavigationPoint NavPoint, out BS_PathNode Neighbor )
{
	local int i;

	for( i = 0; i < NavNodes.Length; ++i )
	{
		if( NavNodes[i].NavPoint == NavPoint )
		{
			Neighbor = NavNodes[i];
			break;
		}
	}
}

function bool DestinationReachableFromNavPoint( Pawn Pather, Vector Destination, NavigationPoint NavPoint )
{
	local bool hitStaticActor;
	local Actor HitActor;
	local Vector HitLocation, HitNormal, PathCollisionExtent;

	// Create the path collision extent
	PathCollisionExtent.X = Pather.GetCollisionRadius();
	PathCollisionExtent.Y = Pather.GetCollisionRadius();
	PathCollisionExtent.Z = 1.f;

	hitStaticActor = false;

	//Check if any static actors or blocking volumes are in the way
	foreach TraceActors( class'Actor', HitActor, HitLocation, HitNormal, Destination, NavPoint.Location, PathCollisionExtent )
	{
		if( HitActor != none && ( StaticMeshActor( HitActor ) != none || BlockingVolume( HitActor ) != none || InterpActor( HitActor ) != none ) )
		{
			hitStaticActor = true;
		}
	}

	return !hitStaticActor;
}

function InsertNavNode( BS_PathNode NavNode, out array< BS_PathNode > OpenList )
{
	local int index;

	NavNode.OpenedByPathID = PathID;
	if( OpenList.Length == 0 )
	{
		OpenList.AddItem( NavNode );
		return;
	}

	for( index = OpenList.Length; index > 0; --index )
	{
		if( NavNode.GetFCost() <= OpenList[index - 1].GetFCost() )
			break;
	}

	OpenList.InsertItem( index, NavNode );
}

function EditNavNode( BS_PathNode NavNode, out array< BS_PathNode > OpenList )
{
	OpenList.RemoveItem( NavNode );
	InsertNavNode( NavNode, OpenList );
}

function byte GetCurrentTurnNumber()
{
	return CurrentTurn;
}

function StartGame( optional BS_Pawn pawnToSelect )
{}



auto state WaitingToStart
{
	function StartGame( optional BS_Pawn pawnToSelect )
	{
		local BS_PlayerController BSPC;
		bGameStarted = true;
		BSPC = BS_PlayerController( GetController( 0 ) );
		BS_HUD(  BSPC.myHUD ).TheHUDMovie.EnablePlayerHUD();
		BSPC.SelectHero( pawnToSelect );
		BSPC.SnapCameraToHero();
		bCanEndTurnEarly = true;
		BS_PlayerInput( playerController.PlayerInput ).bTheRightWay = BS_GameInfo( WorldInfo.Game ).SaveSettings.bTheRightWay;
		GoToState( 'GameStart' );	
	}

	function Tick( float DeltaTime )
	{}
}

state GameStart
{
	simulated function Tick( float DeltaTime )
	{
		if( GetALocalPlayerController() != none )
		{
			GotoState( 'StartTurn' );
		}
	}
}

state StartTurn
{
	simulated function Tick( float DeltaTime )
	{
		StartTurnFunc();	
	}

	function BeginState( name PreviousStateName )
	{
		StartTurnFunc();		
	}

	function StartTurnFunc()
	{
		local TurnEventWrapper eventToProcess;

		if( !bGamePaused && !IsTimerActive( 'StartAITurn' ) )
		{
			aiController = none;
			playerController = none;
		
			`log( "Start Turn Begin State" );

			foreach TurnEvents( eventToProcess )
			{
				if( eventToProcess.phaseToFire == TURN_START && eventToProcess.turnEvent.turnToFire == CurrentTurn )
				{
					eventToProcess.turnEvent.callbackFunction();
					if( !eventToProcess.turnEvent.isLooping )
					{
						TurnEvents.RemoveItem( eventToProcess );
					}
					else
					{
						eventToProcess.turnEvent.turnToFire = CurrentTurn + eventToProcess.turnInterval;
					}
				}
			}

			aiController = BS_AIController( Controllers[CurrentPlayersTurn] );
	
			if( aiController != none )
			{
				`log( "Ai Turn Timer Started" );
				SetTimer( TimeDelayBeforeAITurnStart, false, 'StartAITurn' );	
				TriggerGlobalEventClass( class'BS_SeqEvent_BeginningOfEnemyTurn', self, 0 ); 
			}
			else
			{
				`log( "Begin Player Turn" );
				playerController = BS_PlayerController( Controllers[CurrentPlayersTurn] );
				TriggerGlobalEventClass( class'BS_SeqEvent_BeginningOfPlayerTurn', self, 0 );
				playerController.BeginTurn();
				GotoState( 'TakingTurn' );
			}
		}
	}
}

state TakingTurn
{
	simulated function Tick( float DeltaTime )
	{
		local bool TurnFinished;

		if( !bGamePaused )
		{
			TurnFinished = true;

			if( aiController != none )
			{
				TurnFinished = !aiController.HasUnitWithAction();
			}
			else
			{
				TurnFinished = !playerController.HasUnitWithAction();
			}
			if( TurnFinished )
			{
				GotoState( 'EndTurn' );
			}
		}
	}
}

state EndTurn
{
	simulated function Tick( float DeltaTime )
	{
		EndTurnFunc();		
	}

	function BeginState( name PreviousStateName )
	{
		EndTurnFunc();
	}

	function EndTurnFunc()
	{
		local TurnEventWrapper eventToProcess;		

		if( !bGamePaused )
		{
			foreach TurnEvents( eventToProcess )
			{
				if( eventToProcess.phaseToFire == TURN_END && eventToProcess.turnEvent.turnToFire == CurrentTurn )
				{
					eventToProcess.turnEvent.callbackFunction();
					if( !eventToProcess.turnEvent.isLooping )
					{
						TurnEvents.RemoveItem( eventToProcess );
					}
					else
					{
						eventToProcess.turnEvent.turnToFire = CurrentTurn + eventToProcess.turnInterval;
					}
				}
			}

			if( aiController != none )
			{
				aiController.EndTurn();
			}
			else
			{
				playerController.EndTurn();
			}	

			++CurrentTurn;
			CurrentPlayersTurn = CurrentTurn % NumberOfPlayers;
			GotoState( 'StartTurn' );
		}
	}
}

function SetWinner( bool winnerIsFirstPlayer )
{
	if( winnerIsFirstPlayer )
	{
		WinnerIndex = BS_PlayerController( GetController( 0 ) ).TeamNumber;
	}
	else
	{
		WinnerIndex = BS_AiController( GetController( 1 ) ).TeamNumber;
	}
	GotoState( 'GameOver' );
}

state GameOver
{
	simulated function Tick( float DeltaTime )
	{

	}

Begin:
	playerController = BS_PlayerController( GetALocalPlayerController() );

	BS_HUD( GetALocalPlayerController().myHUD ).TheHUDMovie.DisplayLevelOutcome( WinnerIndex == playerController.TeamNumber );
}

function EnableHealthBars( bool emabled )
{
	local BS_Pawn BSP;
	local BS_PlayerController BSPC;
	local BS_AiController BSAC;
	
	bShowHealthBars = emabled;
	
	BSPC = BS_PlayerController( GetController( 0 ) );
	BSAC = BS_AIController( GetController( 1 ) );
	
	if( BSPC != none )
	{
		foreach BSPC.units( BSP )
		{
			BSP.ShowHideHealthBar( bShowHealthBars );
		}
	}

	if( BSAC != none )
	{
		foreach BSAC.units( BSP )
		{
			BSP.ShowHideHealthBar( bShowHealthBars );
		}
	}
}

function ShowHealthBars()
{
	EnableHealthBars( true );
}

function HideHealthBars()
{
	EnableHealthBars( false );
}

function PauseGame()
{
	bGamePaused = true;
	SetTimer( 0.0f, false, 'StartAITurn' );
}

function UnpauseGame()
{
	bGamePaused = false;
}


DefaultProperties
{
	CurrentPlayersTurn=0
	CurrentTurn=0
	NumberOfPlayers = 2
	PathID = 0
	LastPathingError = NO_ERROR_OCCURED

	TimeDelayBeforeAITurnStart = 2.0f
	bGameStarted = false
	bGamePaused = false

	//archetype = BS_Pawn'Test.Test.TestEnemy'
	SupportedEvents.Add( class'BS_SeqEvent_BeginningOfPlayerTurn' )
	SupportedEvents.Add( class'BS_SeqEvent_BeginningOfEnemyTurn' )
	SupportedEvents.Add( class'BS_SeqEvent_DialogueEnded' )
	SupportedEvents.Add( class'BS_SeqEvent_SkipMatineeAndDialogue' )
	SupportedEvents.Add( class'BS_SeqEvent_SkillSelectionDone' )
	SupportedEvents.Add( class'BS_SeqEvent_PawnOutOfAP' )
	bShowHealthBars = true
	
	bCanEndTurnEarly = false
}

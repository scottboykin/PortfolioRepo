class BS_PlayerInput extends PlayerInput within BS_PlayerController;

var bool bTheRightWay;
var bool bHUDShowing;
var ParticleSystem MoveIndicatorPS;


exec function StartFire( optional byte FireModeNum )
{
	local BS_HUD bsHUD;
	bsHUD = BS_HUD( myHUD );	

	//PH: If the HUD is capturing input don't issue an order... ie the player was clicking a button
	if( !IsHUDCapturingInput() )
	{
		if( bTheRightWay && CurrentAction == PlayerAction.ACTION_ABILITY )
		{
			TryToIssueUseAbiltyOrder( bsHUD );
			return;
		}

		else if( !bTheRightWay && CurrentAction == PlayerAction.ACTION_ABILITY )
		{
			CurrentAction = PlayerAction.ACTION_MOVEATTACK;
			bsHUD.DeselctedAbilityButton( CurrentAbilityIndex );
			if( SelectedHero != none )
				SelectedHero.ActivateDeactivatePawnsInRange();
		}

		else if( CurrentAction != PlayerAction.ACTION_ABILITY && CanSelectUnit( bsHUD.currentlyHoveringOver ) )
		{
			if( bsHUD.currentlyHoveringOver != SelectedHero )
			{
				SelectHero( bsHUD.currentlyHoveringOver );
				bsDoubleClickTimer = MAX_DOUBLE_CLICK_TIME;
			}

			else if( bsDoubleClickTimer > 0.0f )
			{
				SnapCameraToHero();
				bsDoubleClickTimer = MAX_DOUBLE_CLICK_TIME;
			}
			else
			{
				bsDoubleClickTimer = MAX_DOUBLE_CLICK_TIME;
			}
		}
		
	}
}

exec function StartAltFire( optional byte FireModeNum )
{
	local BS_HUD bsHUD;
	bsHUD = BS_HUD( myHUD );

	if( !IsHUDCapturingInput() )
	{
		if( !bTheRightWay && CurrentAction == PlayerAction.ACTION_ABILITY )
		{
			TryToIssueUseAbiltyOrder( bsHUD );
			return;
		}

		else if( bTheRightWay && CurrentAction == PlayerAction.ACTION_ABILITY )
		{
			CurrentAction = PlayerAction.ACTION_MOVEATTACK;
			bsHUD.DeselctedAbilityButton( CurrentAbilityIndex );
			if( SelectedHero != none )
				SelectedHero.ActivateDeactivatePawnsInRange();
		}

		else if( SelectedHero != none && SelectedHero.teamNumber == TeamNumber )
		{
			if( SelectedHero.IsPerformingAction() )
			{
				`log( 'Still Performing Action' );
				BroadcastLocalizedMessage( class'BS_LocalMessage', LM_UNIT_STILL_PERFORMING_ACTION );
				return;
			}

			if( bsHUD.currentlyHoveringOver == none && !SelectedHero.IsCombatLocked() && IsValidMoveLocation( bsHUD ) && SelectedHero.CanMove() && SelectedHero.ValidDestination( bsHUD.currentMouseLocationOnTerrain ) && SelectedHero.LocationIsInMovementRange( bsHUD.currentMouseVector ) && turnManager.UseAction( SelectedHero, 1 ) )
			{ 
				if( MoveIndicatorPS != none )
					WorldInfo.MyEmitterPool.SpawnEmitter( MoveIndicatorPS, bsHUD.currentMouseLocationOnTerrain );
				SelectedHero.MoveCommand( bsHUD.currentMouseLocationOnTerrain );
				SelectedHero.bIsInFutureMode = false;
			}
			else if( bsHUD.currentlyHoveringOver != none && bsHUD.currentlyHoveringOver.teamNumber != TeamNumber && SelectedHero.ValidTarget( bsHUD.currentlyHoveringOver ) && SelectedHero.bCanAttack && turnManager.UseAction( SelectedHero, 1 ) )
			{
				SelectedHero.AttackTarget( bsHUD.currentlyHoveringOver );
				SelectedHero.bIsInFutureMode = false;
			}
		}
	}
	else
	{
		bsHUD.StartRightClick();
	}

}

function TryToIssueUseAbiltyOrder( BS_HUD bsHUD )
{
	if( SelectedHero != none && !SelectedHero.IsPerformingAction() && SelectedHero.Abilities[CurrentAbilityIndex].CanUseAbility( bsHUD.currentlyHoveringOver, bsHUD.currentMouseLocationOnTerrain ) && turnManager.UseAction( SelectedHero, SelectedHero.Abilities[CurrentAbilityIndex].APCost, CurrentAbilityIndex ) )
	{
		SelectedHero.Abilities[CurrentAbilityIndex].UseAbility( bsHUD.currentlyHoveringOver, bsHUD.currentMouseLocationOnTerrain );
		CurrentAction = PlayerAction.ACTION_MOVEATTACK;
		SelectedHero.ActivateDeactivatePawnsInRange();
	}
}

function TryToIssueUseAbilityOrderUsingPawn( BS_Pawn bsPawn )
{
	if( SelectedHero != none && !SelectedHero.IsPerformingAction() && SelectedHero.Abilities[CurrentAbilityIndex].CanUseAbility( bsPawn ) && turnManager.UseAction( SelectedHero, SelectedHero.Abilities[CurrentAbilityIndex].APCost, CurrentAbilityIndex ) )
	{
		SelectedHero.Abilities[CurrentAbilityIndex].UseAbility( bsPawn );
		CurrentAction = PlayerAction.ACTION_MOVEATTACK;
		SelectedHero.ActivateDeactivatePawnsInRange();
	}
}

function bool IsValidMoveLocation( BS_HUD bsHUD )
{
	if( bsHUD.mouseOnTerrain )
	{
		return SelectedHero.CanMoveTo( bsHUD.currentMouseLocationOnTerrain );
	}

	return false;
}

exec function StartGameFromWaiting()
{
	BS_GameInfo( WorldInfo.Game ).TurnManager.StartGame();
}

exec function LEEEEEROOOOOOYJEEEKKIIIIINNNSSSS()
{
	BS_GameInfo( WorldInfo.Game ).leeroyJenkinsMode = !BS_GameInfo( WorldInfo.Game ).leeroyJenkinsMode;
}

exec function InBrightestDayInBlackestNightNoEvilShallEscapeMySightLetThoseWhoWorshipEvilsMightBewareMyPowerGREENLANTERNSLIGHT()
{
	BS_GameInfo( WorldInfo.Game ).greenLanternsMight = !BS_GameInfo( WorldInfo.Game ).greenLanternsMight;
}

exec function SupermanOP()
{
	BS_GameInfo( WorldInfo.Game ).supermanMode = !BS_GameInfo( WorldInfo.Game ).supermanMode;
}

exec function Speedforce()
{
	WorldInfo.TimeDilation = 4.0f;
}

exec function BATMAN()
{
	local BS_HUD bsHUD;
	bsHUD = BS_HUD( myHUD );
	bsHUD.TheHUDMovie.TheTurnTransition.PlayerTurnTF.SetString( "text", "CAUSE I'M BATMAN" );
	bsHUD.TheHUDMovie.TheTurnTransition.PlayerTurnMC.GotoAndPlayI(1);

	BS_GameInfo( WorldInfo.Game ).leeroyJenkinsMode = false;
	BS_GameInfo( WorldInfo.Game ).greenLanternsMight = false;
	BS_GameInfo( WorldInfo.Game ).supermanMode = false;
	BS_GameInfo( WorldInfo.Game ).bInstantKill = false;
	WorldInfo.TimeDilation = 1.0f;
}

exec function OVER9000()
{
	BS_GameInfo( WorldInfo.Game ).bInstantKill = !BS_GameInfo( WorldInfo.Game ).bInstantKill;
}

exec function GottaCatchemAll()
{
	local BS_Pawn Unit;

	PlaySound( SoundCue'BS_SuperSecertPackage.SecertStuff.PokeTheme' );

	foreach units( Unit )
	{
		Unit.IWantToBeTheVeryBest();
	}

}

exec function TheRightWay()
{
	bTheRightWay = true;
	UpdateRightWay();
}

exec function TheWrongWay()
{
	bTheRightWay = false;
	UpdateRightWay();
}

function UpdateRightWay()
{
	local BS_GameInfo BSGI;
	BSGI = BS_GameInfo( WorldInfo.Game );
	BSGI.SaveSettings.bTheRightWay = bTheRightWay;
	BSGI.SaveGameUtils.SaveGameSettings( BSGI.SaveSettings, BSGI.SaveGameUtils.SaveSettingsFileName );
}

exec function FixItFelix()
{
	`log( "I CAN FIX IT!" );
	BS_GameInfo( WorldInfo.Game ).Fixed = true;
}

exec function WreckItRalph()
{
	`log( "I\'M GONNA WRECK IT!" );
	BS_GameInfo( WorldInfo.Game ).Fixed = false;
}

exec function DebugPathing()
{
	BS_GameInfo( WorldInfo.Game ).debugPathing = !BS_GameInfo( WorldInfo.Game ).debugPathing;
}

exec function SwitchWeapon( byte num )
{
	local BS_HUD bsHUD;
	bsHUD = BS_HUD( myHUD );
	
    if( bsHUD.SkillSelectionMovie != none && bsHUD.SkillSelectionMovie.bMovieIsOpen )
	{
		if( bControlIsDown )
		{
			bsHUD.SkillSelectionMovie.SavePreset( num );
		}
		else
		{
			bsHUD.SkillSelectionMovie.LoadPreset( num );
		}
	}
    else
	{
		if( !TurnManager.bGamePaused && SelectedHero.TeamNumber == TurnManager.CurrentPlayersTurn )
		{
			if( SelectedHero != none && SelectedHero.teamNumber == TeamNumber && num <= SelectedHero.Abilities.Length ) 
			{
				if( SelectedHero.Abilities[ num - 1 ] != none && SelectedHero.Abilities[ num - 1 ].bEnabled )
				{
					CurrentAbilityIndex = num - 1;
					if( SelectedHero.Abilities[CurrentAbilityIndex].AbilityType == ABILITY_TYPE_PASSIVE || SelectedHero.Abilities[CurrentAbilityIndex].AbilityType == ABILITY_TYPE_ATTACKOVERRIDE )
					{
						//Do nothing if it is a passive ability
					}
					else if( SelectedHero.Abilities[CurrentAbilityIndex].InstantCast )
					{
						if( SelectedHero.Abilities[CurrentAbilityIndex].CanUseAbility( bsHUD.currentlyHoveringOver, bsHUD.currentMouseLocationOnTerrain ) && turnManager.UseAction( SelectedHero, SelectedHero.Abilities[CurrentAbilityIndex].APCost ) )
						{
							SelectedHero.Abilities[CurrentAbilityIndex].UseAbility( bsHUD.currentlyHoveringOver, bsHUD.currentMouseLocationOnTerrain );
						}
					}
					else
					{
						CurrentAction = ACTION_ABILITY;
					}
					TurnManager.PlaySound( bsHUD.TheHUDMovie.AbilityClickSound );
					SelectedHero.ActivateDeactivatePawnsInRange();
				}
			}
		}
	}
}

exec function EscKeyPressed()
{
	local BS_HUD BSHUD;
	local GFxCLIKWidget.EventData params;
	BSHUD = BS_HUD( MyHUD );

	if( BSHUD.SkillSelectionMovie != none && BSHUD.SkillSelectionMovie.bMovieIsOpen )
	{
		BSHUD.SkillSelectionMovie.NoConfirmationClicked( params );
	}
	else
	{
		if( SelectedHero != none )
		{
			DeselectUnit();
		}
		else
		{
			ShowMenu();
		}
	}
}

exec function ShowMenu()
{
	local BS_HUD BSHUD;
	BSHUD = BS_HUD( MyHUD );

	if( BSHUD.SkillSelectionMovie != none && BSHUD.SkillSelectionMovie.bMovieIsOpen )
	{
		return;
	}

	BSHUD.PauseMenuMovie.TogglePauseMenu();

	if( BSHUD.PauseMenuMovie.bPauseMenuShowing )
		TurnManager.PauseGame();
	else
		TurnManager.UnpauseGame();

	`log( "ShowMenu" );

	//ConsoleCommand( "Quit" );
}

//PH: Added this to be able to deselect a unit when the ESC key is pressed
exec function DeselectUnit()
{
	local BS_HUD bsHUD;
	bsHUD = BS_HUD( myHUD );

	if( CurrentAction == ACTION_ABILITY )
	{		
		CurrentAction = ACTION_MOVEATTACK;
		bsHUD.DeselctedAbilityButton( CurrentAbilityIndex );
		if( SelectedHero != none )
			SelectedHero.ActivateDeactivatePawnsInRange();
	}
	else
	{
		SelectHero( none );
	}

	if( bsHUD.SkillSelectionMovie != none && bsHUD.SkillSelectionMovie.bMovieIsOpen )
	{
		bsHUD.SkillSelectionMovie.DeselectAllUnitPanels();
	}
}

exec function EndTurnEarly()
{
	if( !TurnManager.bGamePaused && TurnManager.bCanEndTurnEarly && TurnManager.CurrentPlayersTurn == TeamNumber )
		TurnManager.EndTurnEarly( TeamNumber );
}

//PH: Used to check if a HUD button is capturing input
function bool IsHUDCapturingInput()
{
	local BS_HUD BSHUD;
	BSHUD = BS_HUD( MyHUD );
	if( BSHUD == none || BSHUD.TheHUDMovie == none )
		return false;

	return BSHUD.IsHUDCapturingInput() || TurnManager.bGamePaused || !TurnManager.bGameStarted;
}

exec function CameraZoomIn()
{
	if( !TurnManager.bGamePaused )
		BS_Camera( PlayerCamera ).CameraZoomIn();
}

exec function CameraZoomOut()
{
	if( !TurnManager.bGamePaused )
		BS_Camera( PlayerCamera ).CameraZoomOut();
}

exec function OpenMainMenu()
{
	ConsoleCommand( "open UDKFrontEndMap" );
}

exec function ToggleFutureMode()
{
	if( !TurnManager.bGamePaused )
		SelectedHero.bIsInFutureMode = !SelectedHero.bIsInFutureMode;
}

exec function ToggleHealthBars()
{
	if( !TurnManager.bGamePaused )
		TurnManager.EnableHealthBars( !TurnManager.bShowHealthBars );
}

exec function SkipMatinee()
{
	local BS_HUD BSHUD;
	local GFxCLIKWidget.EventData params;
	ConsoleCommand( "cancelmatinee" );
	//TurnManager.TriggerGlobalEventClass( class'BS_SeqEvent_SkipMatineeAndDialogue', TurnManager );
	BSHUD = BS_HUD( MyHUD );
	//if( BSHUD == none || BSHUD.TheHUDMovie == none )
	//	return;
	//BSHUD.TheHUDMovie.DisableDialoguePanel();
	if( BSHUD != none )
	{
		if( BSHUD.SkillSelectionMovie != none && BSHUD.SkillSelectionMovie.bMovieIsOpen )
			BSHUD.SkillSelectionMovie.SkillSelectionDone( params );
	}
}

exec function HotkeySelectHero( byte num )
{
	local GFxCLIKWidget.EventData params;
	local BS_HUD theHUD;
	theHUD = BS_HUD( myHUD );

	if( !TurnManager.bGamePaused && ( theHUD.SkillSelectionMovie == none || !theHUD.SkillSelectionMovie.bMovieIsOpen ) )
	{
		switch( num )
		{
		case 0:
			theHUD.TheHUDMovie.UnitsBjornButtonClicked( params );
			break;
		case 1:
			theHUD.TheHUDMovie.UnitsDagButtonClicked( params );
			break;
		case 2:
			theHUD.TheHUDMovie.UnitsFreyaButtonClicked( params );
			break;
		case 3:
			theHUD.TheHUDMovie.UnitsIlianaButtonClicked( params );
			break;
		}
	}
	else
	{
		switch( num )
		{
		case 0:
			theHUD.SkillSelectionMovie.BjornSkillClicked( params );
			break;
		case 1:
			theHUD.SkillSelectionMovie.DagSkillClicked( params );
			break;
		case 2:
			theHUD.SkillSelectionMovie.FreyaSkillClicked( params );
			break;
		case 3:
			theHUD.SkillSelectionMovie.IlianaSkillClicked( params );
			break;
		}
	}
}

exec function AllTheThings()
{
	local BS_GameInfo BSGI;
	BSGI = BS_GameInfo( class'WorldInfo'.static.GetWorldInfo().Game );
	BSGI.SaveGameState.SetLevelStatus( 0, true );
	BSGI.SaveGameState.SetLevelStatus( 1, true );
	BSGI.SaveGameState.SetLevelStatus( 2, true );
	BSGI.SaveGameState.SetLevelStatus( 3, true );
	BSGI.SaveGameState.SetLevelStatus( 4, true );
	BSGI.SaveGameUtils.SaveGameState( BSGI.SaveGameState, BSGI.SaveGameStateFileName );
}

function bool CanSelectUnit( BS_Pawn pawnToCheck )
{
	local bool canSelect;
	local int i;
	canSelect = true;
	if( bUseArrayToDetermineUnitsThatCanBeSelected )
	{
		canSelect = false;
		for( i = 0; i < PawnsThatCanBeSelected.Length; ++i )
		{
			if( PawnsThatCanBeSelected[i] == pawnToCheck )
				canSelect = true;
		}
	}

	return canSelect;
}

exec function ControlDown()
{
	bControlIsDown = true;
}

exec function ControlUp()
{
	bControlIsDown = false;
}

//Hack function for taking screen shots
exec function ToggleHUD()
{
	if( bHUDShowing )
		BS_HUD( myHUD ).TheHUDMovie.HideHUD();
	else
		BS_HUD( myHUD ).TheHUDMovie.EnablePlayerHUD();
	bHUDShowing = !bHUDShowing;
}


DefaultProperties
{
	bTheRightWay = true
	bHUDShowing = true
	MoveIndicatorPS = ParticleSystem'BS_EnvironmentParticles.MoveLocation.bs_envParticles_moveLocation_PS'
}

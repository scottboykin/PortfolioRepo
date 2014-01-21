--#require Collision


--JS: File load and execution priority
priority = -950

dofile( self, APP_PATH .. "scripts/include/xinput_defines.lua" )

-- DLL that wraps XInput
g.require( "XInputLuaWrapper" )

--UI Variables
moveSpeed     = 3.0
sprintSpeed   = 5.0
jumpSpeed     = 10.0
climbSpeed    = 10.0
friction      = 0.9
lockpicks	  = 5
onLadder = false
interactTimer = nil
lastObjectContacted = nil

--Player animation state controllers
initialized = false
isAlive     = false
isIdle      = false
isMoving    = false
isJumping   = false
isFalling   = false
isIdleClimbing = false
isClimbing	= false
isSabotaging = false
isLockpicking = false
isPushing = false
isStunned   = false
isInCombat  = false
isKnockingOutGuard = false
attemptingKnockout = false
isHidden = false
isInLight = false
exitToMenu = false
respawn = false
newState    = ""
frozenDuringCollision = false
objectiveUpdated = false
pickedUpClue = false

--Player properties
currentObjective = ""
currentObj = 1
stunTime       = 0
deathDelay     = 0
respawnDelay = 0
reposeDelay = 0
currentDeathDelay = 0
exitToMenuDelay = 0
radius = 0
drawCollision  = false
maxSpeed  = vec2( 0, 0 )
newSpeed	= vec2( 0, 0 )
dir = 1
rotation  = 0
currentStunTime = 0
sabotageTime = 0
lockpickTime = 0
knockoutTime = 0
jumpDir = 1
idleClimbingTime = 0
currentLevel = ""

grabDistance = 4

climbSound = nil
climbSoundPlaying = false

sprintSound = nil
sprintSoundPlaying = false

lightCheckCount = 0

movingAnim = nil
pushingAnim = nil

strangleAnim = nil
strangleMissAnim = nil

tardisAnim = nil
tardisTransparency = 1

deathAnimations = {}
deathAnim = nil

lockpickSound = nil
sabotageSound = nil

speechBubble = nil

aOldState = false
aNewState = false
xOldState = false
xNewState = false

ui = {
	moveSpeed      = { order = 1,  type = "number",  label = "Movement Speed" },
	sprintSpeed    = { order = 2,  type = "number",  label = "Sprint Speed" },
	jumpSpeed      = { order = 3,  type = "number",  label = "Jump Speed" },
	climbSpeed     = { order = 4,  type = "number",  label = "Climb Speed" },
	friction       = { order = 5,  type = "number",  label = "Floor Friction" },
	lockpicks	   = { order = 6,  type = "number",  label = "Number of Lockpicks" },
	drawCollision  = { order = 7, type = "bool", label = "Show collision?" },
}


local debugComponent = false


function InitializePlayer()
	if ( g.player ~= this ) then
		g.player = this -- store player as global
	end
	
	if ( rigidBody ~= nil ) then
		g.physics:remove( rigidBody )
	end
	radius = g.math.max( iconW/pixelsPerUnit, iconH/pixelsPerUnit ) - 1
	rigidBody = g.physics:createCircle( x, y, RigidBody_DYNAMIC, radius, 1.0 )
	rigidBody:setCollisionCategory( CollisionCategory_Player )
	rigidBody:setCollisionMask( CollisionMask_Player )	

	--JS: Set actor properties from UI values
	maxSpeed.x  = g.math.clamp( g.math.abs( moveSpeed ), 0.1, 128 )
	maxSpeed.y  = -g.math.clamp( g.math.abs( jumpSpeed ), 0, 128 )
	sprintSpeed = g.math.clamp( g.math.abs( sprintSpeed ), g.math.abs( moveSpeed ), 128 )
	climbSpeed  = -climbSpeed
	friction    = g.math.clamp( friction, 0, 1 )


	SetPlayerPropertiesFromSession()

	playerDeath = g.eventStruct;
	playerDeath = g.EventSystem_LocateEventMapping("evtPlayerPreDeath");
	if ( playerDeath ~= nil ) then
		playerDeath.sender = this.id;
		playerDeath.destinationIDs = { this.id }
	end
	
	initialized = true
	isAlive     = true
	iconVisible = false
	respawn = false

	--Set current level in session variables
	SetSessionVariable( "currentLevel", g.thisLevel )
	
	tardisAnim = ImageAnim( "Objects/TARDIS.lua" )
	
	movingAnim = ImageAnim( "Mooney/Walk.lua" )
	pushingAnim = ImageAnim( "Mooney/Pushing.lua" )
	strangleAnim = ImageAnim( "Mooney/Strangle.lua" )
	strangleMissAnim = ImageAnim( "Mooney/StrangleMiss.lua" )
	
	deathAnimations[1] = ImageAnim( "Mooney/Death1.lua" )
	deathAnimations[2] = ImageAnim( "Mooney/Death2.lua" )
	deathAnimations[3] = ImageAnim( "Mooney/Death3.lua" )
	
	climbSound = Sound( "sm_sfx_ladder_climb.wav" )
	sprintSound = Sound( "sm_sfx_footsteps.wav" )
	lockpickSound = Sound( "sm_sfx_lockpick.wav" )
	sabotageSound = Sound( "sm_sfx_sabotage.wav" )
	
	currentLevel = g.session.currentLevel
	g.string.lower( currentLevel )
	
	if g.string.find( currentLevel, "sm_level" ) ~= nil or g.string.find( currentLevel, "sm_Level" ) ~= nil then
		loadScreen = Image( "MenuHUD/" .. g.session.currentLevel .. "_load.png" )
	end
	
	if ( debugComponent ) then
		debugPrint( this.id .. " : PLAYER : init: Initialized" )
	end
	
	Image_setZoomScaleFactor( 1.3 )

end


function init()
	InitializePlayer()
end


function update( dt ) 
	if ( initialized ) then
		exitToMenuDelay = exitToMenuDelay + dt
		if ( isAlive ) then
			if inputEnabled then
				if g.keys["RETURN"] or IsButtonDown( XINPUT_GAMEPAD_START ) then
					if exitToMenuDelay > 2 then
						callTheDoctor()
					end
				end
				
				UpdateXboxInput()
				
				if g.keys["LEFT"] and not g.keys["A"] then
					eventFn_PlayerMove_Left()
				end
				
				if g.keys["RIGHT"] and not g.keys["D"] then
					eventFn_PlayerMove_Right()
				end
				
				if g.keys["UP"] and not g.keys["W"] then
					eventFn_PlayerMove_ClimbUp()
				end
				
				if g.keys["DOWN"] and not g.keys["S"] then
					eventFn_PlayerMove_ClimbDown()
				end
				
				if IsButtonJustDown( XINPUT_GAMEPAD_A ) then
					eventFn_PlayerJump()
				end
				
				if (IsButtonJustDown( XINPUT_GAMEPAD_RIGHT_SHOULDER ) or isKeyPress("Q")) and rigidBody:isOnGround() then
					KnockoutGuard()
				end
				
				if IsButtonJustDown( XINPUT_GAMEPAD_X ) or IsButtonJustDown( XINPUT_GAMEPAD_B ) then
					eventFn_PlayerInteract()
				end
						
				local joyLeftX, joyLeftY = g.XInputLuaWrapper.get_joystick_position( "left" )
				local len = joyLeftX * joyLeftX + joyLeftY * joyLeftY
				
				if (len > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE and joyLeftX < -30000) or IsButtonDown(XINPUT_GAMEPAD_DPAD_LEFT) then
					eventFn_PlayerMove_Left()
				end
				
				if (len > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE and joyLeftX > 30000) or IsButtonDown(XINPUT_GAMEPAD_DPAD_RIGHT) then
					eventFn_PlayerMove_Right()
				end
				
				if (len > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE and joyLeftY < -30000) or IsButtonDown(XINPUT_GAMEPAD_DPAD_DOWN) then
					eventFn_PlayerMove_ClimbDown()
				end
				
				if (len > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE and joyLeftY > 30000) or IsButtonDown(XINPUT_GAMEPAD_DPAD_UP) then
					eventFn_PlayerMove_ClimbUp()
				end	
			end	
			
			if isSabotaging then
				sabotageTime = sabotageTime + dt
				if sabotageTime >= 3 then
					lastObjectContacted.sabotageFuse()
					isSabotaging = false
					interactTimer.destroy()
					interactTimer = nil
				end
			end
			
			if isLockpicking then
				lockpickTime = lockpickTime + dt
				if lockpickTime >= 3 then
					lastObjectContacted.Unlock()
					isLockpicking = false
					interactTimer.destroy()
					interactTimer = nil
				end
			end
			
			if attemptingKnockout and not isKnockingOutGuard then
				reposeDelay = reposeDelay + dt
				if reposeDelay >= .5 then
					EnablePlayerInput()
					attemptingKnockout = false
				end
			end
			
			if isKnockingOutGuard then
				knockoutTime = knockoutTime + dt
				debugPrint("Knocking out: " .. knockoutTime)
				if knockoutTime >= 3 then
					isKnockingOutGuard = false
					interactTimer.destroy()
					interactTimer = nil
				end
			end
			
		else
			frozenDuringCollision = false
		end

		if exitToMenu then
			if iconAnimTime > (tardisAnim:frames() - 1) / tardisAnim:fps() then
				iconAnimTime = ( tardisAnim:frames() -1 ) / tardisAnim:fps()
				tardisTransparency = tardisTransparency - .01
				if tardisTransparency <= 0 then
					if g.session.currentLevel ~= "sm_main_menu" then
						loadLevel( "sm_main_menu" )
					else
						loadLevel( g.session.previousLevel )
					end
				end
			end
		end

		if ( isAlive == false ) then
			currentDeathDelay = currentDeathDelay + dt
			if currentDeathDelay >= 2 then
				deathAnimComplete = true
				local thisEvent = new( g.eventStruct )
				thisEvent = g.EventSystem_LocateEventMapping( "evtPlayerPostDeath" )
				if ( thisEvent ~= nil ) then
					thisEvent.sender = this.id
					thisEvent.destinationIDs = { this.id }
					EventClientComponent_PostEvent( thisEvent )
				end
			end
		end
		
		if objectiveUpdated then
			if speechBubble ~= nil then
				speechBubble.rigidBody = g.physics:createCircle( 0, 0, RigidBody_DYNAMIC, 0, 0 )
				speechBubble.destroy()
				speechBubble = nil
			end
			speechBubble = spawn(rigidBody:posX(),rigidBody:posY() - (iconW/pixelsPerUnit), "SpeechBubble", "Foreground")
			objectiveUpdated = false
		end
		
		if respawn then
			respawnDelay = respawnDelay + dt
			if respawnDelay > 0.1 then
				loadLevel( g.session.currentLevel )
			end
		end
		
		newState = "idle"
		if ( isMoving ) then
			newState = "move"
		end
		if ( isClimbing ) then
			if isClimbingIdle then
				newState = "idleclimb"
			else
				newState = "climb" 
			end
		end
		if ( isHidden ) then
			newState = "hide"
		end
		if isJumping then
			newState = "jump"
		end
		if ( isFalling ) then
			isClimbing = false
			newState = "fall"
		end
		if isSabotaging then
			newState = "sabotage"
		end
		if isLockpicking then
			newState = "lockpick"
		end
		if ( frozenDuringCollision ) then 
			newState = "stun"
		end
		if ( isInCombat ) then
			newState = "combat"
		end
		if ( not isAlive ) then
			newState = "dead"
		end

	end
end


function fixedUpdate( dt )
	if ( initialized ) then
		if isAlive and frozenDuringCollision == false and not isHidden and not attemptingKnockout and not isKnockingOutGuard then
			if ( inputEnabled == false ) then
				EnablePlayerInput()
			end
			
			vx = rigidBody:linearVelocityX()
			vy = rigidBody:linearVelocityY()
			if onLadder then
				vy = -.2
				isFalling = false
			end
			
			if ( newSpeed.x ~= 0 ) then
				vx = newSpeed.x
				newSpeed.x = 0
				isClimbing = false
				if isSabotaging then
					sabotageSound:stop()
					isSabotaging = false
				end
				if isLockpicking then
					lockpickSound:stop()
					isLockpicking = false
				end
				if climbSoundPlaying then
					climbSound:stop()
					climbSoundPlaying = false
				end
				if not IsSprinting() and sprintSoundPlaying then
					sprintSoundPlaying = false
					sprintSound:stop()
				end
				if interactTimer ~= nil then
					interactTimer.destroy()
					interactTimer = nil
				end
				
			else
				vx = vx * ( 1 - friction )
				if ( vx > -0.1 ) and ( vx < 0.1 ) then
					vx = 0
				end
				
				joyLeftX, joyLeftY = g.XInputLuaWrapper.get_joystick_position( "left" )
				
				if not g.keys["RIGHT"] and not g.keys["LEFT"] and not g.keys["D"] and not g.keys["A"] and not IsButtonDown( XINPUT_GAMEPAD_DPAD_LEFT ) and not IsButtonDown( XINPUT_GAMEPAD_DPAD_RIGHT ) and ( joyLeftX <= 10000 and joyLeftX >= -10000 )  then
					isMoving = false
					isPushing = false
					if sprintSoundPlaying then
						sprintSoundPlaying = false
						sprintSound:stop()
					end
				end
			end
			
			if isClimbing then
				
				ladderX = rigidBody:posX()
				c = rigidBody:rayCast( rigidBody:posX() - (iconW/pixelsPerUnit)/1.75, rigidBody:posY() - iconH/pixelsPerUnit/4, rigidBody:posX() + (iconW/pixelsPerUnit)/4, rigidBody:posY() - iconH/pixelsPerUnit/4 )
				if c.collides then
					if c.entity:collisionCategory() == 256 then
						ladderX = c.entity:posX()
					end
				end
				c = rigidBody:rayCast( rigidBody:posX() + (iconW/pixelsPerUnit)/1.75, rigidBody:posY() - iconH/pixelsPerUnit/4, rigidBody:posX(), rigidBody:posY() - iconH/pixelsPerUnit/4 )
				if c.collides then
					if c.entity:collisionCategory() == 256 then
						ladderX = c.entity:posX()
					end
				end
				isMoving = false
				rigidBody:setPos( ladderX, rigidBody:posY() )
				
				vy = newSpeed.y
				if( vy ~= 0 ) then
					isClimbingIdle = false
					idleClimbingTime = 0
				else
					vy = vy - .2
					idleClimbingTime = idleClimbingTime + dt
					if idleClimbingTime > .16 then
						climbSound:stop()
						climbSoundPlaying = false
						isClimbingIdle = true
					end
				end
				newSpeed.y = 0
			else	
				if climbSoundPlaying then
					climbSound:stop()
					climbSoundPlaying = false
				end
				if ( newSpeed.y ~= 0 ) then
					vy = newSpeed.y
					newSpeed.y = 0
				else
					if ( vy >= -0.1 ) and ( vy <= 0.1 ) then
						vy = 0
					end
				end
				
				if vy >= 0 then
					isJumping = false
					
					if vy ~= 0 then
						local ladderCheck = inwardRayCast( CollisionCategory_Ladder )
						if ladderCheck ~= nil then
							isFalling = false
							isClimbing = true
							rigidBody:setPos( ladderCheck:posX(), rigidBody:posY() )
							vy = 0
						end
					end
					
					if vy > 2 and not rigidBody:isOnGround() then
						if onLadder then
							isClimbing = true
							isMoving = false
							isFalling = false
						else
							isFalling = true
						end
						if sprintSoundPlaying then
							sprintSoundPlaying = false
							sprintSound:stop()
						end
					else
						isFalling = false
					end
				end
			end
			
			if not isClimbing then
				rigidBody:setLinearVelocity( vx, vy )	
			else
				rigidBody:setLinearVelocity( 0, vy )	
			end
			
			onLadder = false
			
			if isInLight then
				lightCheckCount = lightCheckCount + 1
				if lightCheckCount > 10 then
					isInLight = false
					lightCheckCount = 0
				end
			end
		else
			rigidBody:setLinearVelocity( 0, rigidBody:linearVelocityY() )	
		end		
	end
end

function render( dt ) 
	if isAlive then
		if not isHidden and not exitToMenu then
			if not isPushing and not attemptingKnockout and not isKnockingOutGuard then
				if isMoving and not isJumping and not isFalling then
					if movingAnim ~= nil then
						if IsSprinting() then
							movingAnim:setFPS( 4 )
						else
							movingAnim:setFPS( 3 )
						end
						movingAnim:scaleX( dir )
						movingAnim:draw( iconAnimTime, x, y - .5, 0, Image_COORDS_GAME )
					end
				else
					if ( iconAnimTime ~= nil ) and ( x ~= nil ) and ( y ~= nil ) and ( angle ~= nil ) then
						iconAnim:draw( iconAnimTime, x, y - .5, 0, Image_COORDS_GAME )
					end
				end
			end
			if isPushing then
				pushingAnim:scaleX( dir )
				pushingAnim:draw( iconAnimTime, x + .75 * dir, y - .5, 0, Image_COORDS_GAME )
			end
			
			if attemptingKnockout then
				if ( iconAnimTime ~= nil ) and ( x ~= nil ) and ( y ~= nil ) and ( angle ~= nil ) then
					strangleMissAnim:scaleX( dir )
					strangleMissAnim:draw( iconAnimTime, x, y - .5, 0, Image_COORDS_GAME )
				end
			end
			
			if isKnockingOutGuard then
				if ( iconAnimTime ~= nil ) and ( x ~= nil ) and ( y ~= nil ) and ( angle ~= nil ) then
					strangleAnim:scaleX( dir )
					strangleAnim:draw( iconAnimTime, x, y - .5, 0, Image_COORDS_GAME )
					strangleAnim:draw( iconAnimTime, x, y - .5, 0, Image_COORDS_GAME )
				end
			end
			
		end
		
		if isHidden and exitToMenu then
			local currentImage = tardisAnim:getFrameByTime( iconAnimTime )
			currentImage:setTransparency( tardisTransparency )
			currentImage:draw( x, y - .5, 0, Image_COORDS_GAME )
		end
	end
	
	if not isAlive then
		if iconAnimTime > (deathAnim:frames() - 1) / deathAnim:fps() then
			iconAnimTime = (deathAnim:frames() - 1) / deathAnim:fps()
		end
		deathAnim:scaleX( dir )
		deathAnim:draw( iconAnimTime, x, y - .5, 0, Image_COORDS_GAME )
	end
	
	if ( initialized ) and ( isAlive ) then
		if ( debugComponent or drawCollision) then
			drawLine( rigidBody:posX(), rigidBody:posY(), rigidBody:posX() - (iconW/pixelsPerUnit)/2 + .5, rigidBody:posY(), 0, 255, 0 )
			drawLine( rigidBody:posX(), rigidBody:posY(), rigidBody:posX() + (iconW/pixelsPerUnit)/2 - .5, rigidBody:posY(), 0, 255, 0 )
			drawLine( rigidBody:posX(), rigidBody:posY(), rigidBody:posX(), rigidBody:posY() - (iconW/pixelsPerUnit)/2 + .5, 0, 255, 0 )	
			drawLine( rigidBody:posX(), rigidBody:posY(), rigidBody:posX(), rigidBody:posY() + (iconH/pixelsPerUnit)/2 - .5, 0, 255, 0 )
			drawLine( rigidBody:posX(), rigidBody:posY(), rigidBody:posX() + (iconW/pixelsPerUnit)/2 * dir, rigidBody:posY(), 255, 255, 0 )
			drawLine( rigidBody:posX(), rigidBody:posY(), rigidBody:posX() + (iconW/pixelsPerUnit)/grabDistance * dir, rigidBody:posY(), 255, 0, 0 )
		end
	end	
	
end

function beginContact( collidedWith ) 
	if ( initialized ) and ( isAlive ) then
		if ( frozenDuringCollision ) then
			if ( debugComponent ) then
				debugPrint( this.id .. " : PLAYER : beginContact : Already colliding. Skipping beginContact" )
			end
			return
		end
			
		if ( collidedWith.iconVisible == false ) then
			if ( debugComponent ) then
				debugPrint( this.id .. " : PLAYER : beginContact : Actor " .. collidedWith.name .. "." .. collidedWith.id .. " is hidden. Skipping beginContact" )
			end
			return
		end
		
		if ( collidedWith:isType( "Door" ) ) then
			lastObjectContacted = collidedWith
		end
		
		if ( collidedWith:isType( "ElevatorSwitch" ) ) then
			debugPrint("CollidedWith " .. collidedWith.id )
			lastObjectContacted = collidedWith
		end
		
		if( collidedWith:isType( "FuseBox" ) ) then
			lastObjectContacted = collidedWith
		end
		
		if( collidedWith:isType( "Crate" ) ) then
			collidedWith.fall()
		end
		
	end
end

function inwardRayCast( lookingFor )

	local rayCastStartingX = rigidBody:posX() - (iconW/pixelsPerUnit) / 1.95 * dir
	local collisionLimit = 3
	local collisionCount = 0
	repeat
		rayCastResult = rigidBody:rayCast( rayCastStartingX, rigidBody:posY(), rigidBody:posX(), rigidBody:posY() )
		if rayCastResult.collides then
			--debugPrint( rayCastResult.entity:collisionCategory() )
			if rayCastResult.entity:collisionCategory() == lookingFor then
				return rayCastResult.entity
			else
				rayCastStartingX = rayCastResult.x + .01
				collisionCount = collisionCount + 1
			end
		end
	until rayCastResult.collides == false or collisionCount >= collisionLimit
	
	return nil
end

function CheckPushing()

	local rayCastStartingX = rigidBody:posX()
	local collisionLimit = 3
	local collisionCount = 0
	repeat
		rayCastResult = rigidBody:rayCast( rayCastStartingX, rigidBody:posY(), rigidBody:posX() + iconW/pixelsPerUnit/2 * dir, rigidBody:posY() )
		if rayCastResult.collides then
			if rayCastResult.entity:collisionCategory() == CollisionCategory_Physics then
				if not isPushing then
					debugPrint( "Not pushing to pushing" )
					iconAnimTime = 0
					isPushing = true
				end
				return
			else
				rayCastStartingX = rayCastResult.x + .01 * dir
				collisionCount = collisionCount + 1
			end
		end
	until rayCastResult.collides == false or collisionCount >= collisionLimit
	isPushing = false
end

function checkDistance( x, y )
	--debugPrint( "Checking distance from " .. lastObjectContacted.id )
	return (g.math.abs(rigidBody:posX() - lastObjectContacted.rigidBody:posX()) < lastObjectContacted.iconW/pixelsPerUnit/x) and
				(g.math.abs(rigidBody:posY() - lastObjectContacted.rigidBody:posY()) < lastObjectContacted.iconH/pixelsPerUnit/y)
end

function callTheDoctor()
	rigidBody:setType( RigidBody_STATIC )
	iconAnimTime = 0
	DisablePlayerInput()
	if sprintSoundPlaying then
		sprintSoundPlaying = false
		sprintSound:stop()
	end
	exitToMenu = true
	isHidden = true
end

function displaySpeechBubble()
	
end

function CreateTimer()
	interactTimer = spawn(rigidBody:posX(),rigidBody:posY() - (iconW/pixelsPerUnit),"Stopwatch","Foreground")
end

function IsSprinting()
	return ((g.keys["SHIFT"] or IsRightTriggerDown()) and (rigidBody:linearVelocityX() > 0.25 or rigidBody:linearVelocityX() < -0.25) )
end

function FadeComplete()
	if not g.player.isAlive then
		PlayerRespawn()
	else
		lastObjectContacted.tardisDoor()
	end
end

function PlayerCaught()
	DisablePlayerInput()
	isAlive = false
	rigidBody:setLinearVelocity( 0, 0 )
	--rigidBody:setType( RigidBody_STATIC )
	--[[cast = rigidBody:rayCast( rigidBody:posX(), rigidBody:posY(), rigidBody:posX(), rigidBody:posY() + 4 )
	if cast.collides then
		rigidBody:setPos( rigidBody:posX(), cast.y - radius/2 )
	end]]--
	newState = "dead"
	iconAnimTime = 0
	deathAnim = deathAnimations[ g.math.random( #deathAnimations ) ]
	if interactTimer ~= nil then
		interactTimer.destroy()
		interactTimer = nil
	end
	sprintSound:stop()
	sprintSoundPlaying = false
end

function KnockoutGuard()
	knockoutTime = 0
	reposeDelay = 0
	iconAnimTime = 0
	if sprintSoundPlaying then
		sprintSound:stop()
		sprintSoundPlaying = false
	end
	DisablePlayerInput()
	rigidBody:setLinearVelocity( 0, 0 )
	attemptingKnockout = true
	thisEvent = new( g.eventStruct )
	thisEvent = g.EventSystem_LocateEventMapping( "evtKnockoutGuard" )
	if thisEvent ~= false then
		thisEvent.sender = this.name ..".".. this.id
		thisEvent.destinationIDs = { "Guards" }
		EventClientComponent_PostEvent( thisEvent )
	end
end

function LadderDismount( dir )
	if rigidBody:isOnGround() then
		return true
	end
	
	c = rigidBody:rayCast( rigidBody:posX() + iconW/pixelsPerUnit/1.5 * dir, rigidBody:posY(), rigidBody:posX() + iconW/pixelsPerUnit/2 * dir, rigidBody:posY() + iconH/pixelsPerUnit/1.25 )
	if c.collides then
		return ( c.entity:collisionCategory() == CollisionCategory_World or c.entity:collisionCategory() == CollisionCategory_Physics )
	end
	return false
end

function UpdateXboxInput()
	g.XInputLuaWrapper.query_xinput_state()
	aOldState = aNewState
	aNewState = g.XInputLuaWrapper.is_button_down( XINPUT_GAMEPAD_A )
	yOldState = yNewState
	yNewState = g.XInputLuaWrapper.is_button_down( XINPUT_GAMEPAD_Y )
	xOldState = xNewState
	xNewState = g.XInputLuaWrapper.is_button_down( XINPUT_GAMEPAD_X )
	bOldState = bNewState
	bNewState = g.XInputLuaWrapper.is_button_down( XINPUT_GAMEPAD_B )
	rBumperOldState = rBumperNewState
	rBumperNewState = g.XInputLuaWrapper.is_button_down( XINPUT_GAMEPAD_RIGHT_SHOULDER )
end

function IsButtonDown( button )
	return g.XInputLuaWrapper.is_button_down( button )
end

function IsRightTriggerDown()
	rightTrigger = g.XInputLuaWrapper.get_trigger_position( "right" )
	if rightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD then
		return true
	end
end

function IsButtonJustDown( button )
	if button == XINPUT_GAMEPAD_Y then
		return ( not yOldState and yNewState )
	end
	
	if button == XINPUT_GAMEPAD_A then
		return ( not aOldState and aNewState )
	end
	
	if button == XINPUT_GAMEPAD_X then
		return ( not xOldState and xNewState )
	end
	
	if button == XINPUT_GAMEPAD_B then
		return ( not bOldState and bNewState )
	end
	
	if button == XINPUT_GAMEPAD_RIGHT_SHOULDER then
		return ( not rBumperOldState and rBumperNewState )
	end
	
	return false
end


function SetPlayerPropertiesFromSession( self )
	--[[debugPrint( this.id .. " : PLAYER : Setting player properties from Session" )

	local checkpoint = GetSessionVariable( "checkpoint" )
	if ( checkpoint.posX ~= nil ) then
		rigidBody:setPos( checkpoint.posX, checkpoint.posY )
	else
		local tmp = {}
		tmp.posX = rigidBody:posX()
		tmp.posY = rigidBody:posY()
		tmp.name = this.id
		SetSessionVariable( "checkpoint", tmp )
	end]]--
end


function PlayerRespawn()
	debugPrint( this.id .. " : PLAYER : Respawning at last checkpoint" )
	
	Image_setZoomScaleFactor( 1 )
	respawn = true

	deathAnimComplete = false
end
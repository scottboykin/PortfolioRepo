--#require Collision


--JS: File load and execution priority
priority = -950
local debugComponent = false

--UI Variables
moveSpeed     = 4.0
pursuitSpeed  = 5.0
initialDir    = "LEFT"
friction      = 0.9

--AI variables
initialized = false
isAlive     = false
maxSpeed  = vec2( 0, 0 )
dir       = 1
newSpeed	= vec2( 0, 0 )
rotation  = 0
frozenDuringCollision = false
checkTime = 0
floorCheckCount = 0
grabbed = false
viewDistance = 3
hasIncreasedLineOfSight = false
isInCombat = false
isTrapped = false
isKnockedout = false
drawCollision = false
aiCheckPathing = true
changeDirTime = 1
lastKnownLocation = nil

ui = {
	initialDir     = { order = 1,  type = "list", values = { "LEFT", "RIGHT" }, label = "Initial movement direction" },
	moveSpeed      = { order = 2,  type = "number",  label = "Movement Speed" },
	pursuitSpeed   = { order = 3,  type = "number",  label = "Pursuit Speed" },
	friction       = { order = 4,  type = "number",  label = "Floor Friction" },
	aiCheckPathing = { order = 5,  type = "bool", label = "AI Pathing Check" },
	drawCollision  = { order = 6, type = "bool", label = "Show collision" },
}

function init()
	--JS: Set actor properties from UI values
	moveSpeed = g.math.clamp( g.math.abs( moveSpeed ), 0, 128 )
	pursuitSpeed = g.math.clamp( g.math.abs( pursuitSpeed ), moveSpeed, 128 )
	friction    = g.math.clamp( friction, 0, 1 )
	
	if g.aiKnockoutSound == nil then
		g.aiKnockoutSound = Sound("sm_sfx_guard_grunt.wav")
	end

	if ( initialDir == "LEFT" ) then
		dir = -1
	else
		dir = 1
	end

	initialized = true
	isAlive     = true
	updateAlways = true
	
	this.id = "Guards"
	
	thisEvent = new( g.eventStruct )
	thisEvent = g.EventSystem_LocateEventMapping( "evtKnockoutGuard" )
	if thisEvent ~= false then
		thisEvent.sender = this.name ..".".. this.id
		thisEvent.destinationIDs = { "Guards" }
		EventClientComponent_RegisterListener( thisEvent )
	end

	if ( debugComponent ) then
		debugPrint( this.id .. " : AI : init: Initialized" )
	end

end


function update( dt ) 
	if ( debugComponent ) or ( aiDrawCollision ) then
		debugDrawCollision( this, {255,255,0} )
	end

	if ( initialized ) then
		if isAlive and not isKnockedOut then
			if not isTrapped and not grabbed then
				newSpeed.x = moveSpeed * dir
				if CheckPursuing() or lastKnownLocation ~= nil then
					newSpeed.x = pursuitSpeed * dir
				end
			end
			
			if grabbed then
				if not g.player.isAlive then
					grabbed = false
					iconVisible = true
				end
				if not g.player.isKnockingOutGuard then
					grabbed = false
					KnockOut()
				end
			end
			
		end
		
		if ( isAlive == false ) then
			deathRemoveTime = deathRemoveTime - dt
			if ( deathRemoveTime < 0 ) then
				deathAnimComplete = true
				eventFn_AIPostDeath()
			end
		end
	end
	
	newState = "idle"
	if ( isMoving ) then
		newState = "move"
	end
	if ( isFalling ) then
		newState = "fall"
	end
	if ( isKnockedout ) then 
		newState = "stun"
	end
	
	if frozenDuringCollision then
		newState = "dead"
	end
		
	if newState == "idle" then
		iconAnimTime = 0
	end
		
end


function fixedUpdate( dt )
	if ( initialized ) then
		if not isKnockedout and not frozenDuringCollision and not grabbed then
			
			vx = rigidBody:linearVelocityX()
			vy = rigidBody:linearVelocityY()

			if ( newSpeed.x ~= 0 ) then
				vx = newSpeed.x
				newSpeed.x = 0
				isMoving = true
			else
				vx = vx * ( 1 - friction )
				if ( vx > -0.1 ) and ( vx < 0.1 ) then
					vx = 0
					isMoving = false
				end
			end

			if ( newSpeed.y ~= 0 ) then
				vy = newSpeed.y
				newSpeed.y = 0
			else
				if ( vy >= -0.1 ) and ( vy <= 0.1 ) then
					vy = 0
				end
			end

			if vy > 5 then 
				isFalling = true
				vx = 0
			else
				isFalling = false
			end

			rigidBody:setLinearVelocity( vx, vy )

			if lastKnownLocation ~= nil then
				if dir == 1 and rigidBody:posX() > lastKnownLocation then
					lastKnownLocation = nil
				end
				if dir == -1 and rigidBody:posX() < lastKnownLocation then
					lastKnownLocation = nil
				end
			end
			
			CheckBehind()
			CheckPlayerCollision()
			AICheckReverseHorizontalDirection()
			if aiCheckPathing then
				CheckPath()
			end
			changeDirTime = changeDirTime + dt
		end
	end
end


function render( dt ) 
	if ( initialized ) then
		if debugComponent or drawCollision then
			drawLine( rigidBody:posX(), rigidBody:posY(), rigidBody:posX() + (iconW/pixelsPerUnit) / 2 * dir, rigidBody:posY(), 0, 255, 0 )
			drawLine( rigidBody:posX(), rigidBody:posY(), rigidBody:posX() - (iconW/pixelsPerUnit) / 8 * dir, rigidBody:posY(), 255, 0, 0 )
			drawLine( rigidBody:posX(), rigidBody:posY(), rigidBody:posX() + (iconW/pixelsPerUnit)/1.5 * dir, rigidBody:posY() + (iconH/pixelsPerUnit)/3, 0, 255, 0 )
			drawLine( rigidBody:posX(), rigidBody:posY() - iconH/pixelsPerUnit/5, rigidBody:posX() + (iconW/pixelsPerUnit) * viewDistance * dir, rigidBody:posY() - iconH/pixelsPerUnit/5, 255, 255, 0 )
			drawLine( rigidBody:posX(), rigidBody:posY(), rigidBody:posX() + iconW/pixelsPerUnit/4 * dir, rigidBody:posY() + iconH/pixelsPerUnit/1.95, 0, 0, 255 )
		end
	end
end

function postUpdate( dt )
	if ( initialized ) and debugComponent then
		if g.player.IsSprinting() then
			viewDistance = 1.5
			drawLine( rigidBody:posX(), rigidBody:posY() - iconH/pixelsPerUnit/5, rigidBody:posX() - (iconW/pixelsPerUnit) * viewDistance * dir, rigidBody:posY() - iconH/pixelsPerUnit/5, 0, 255, 0 )
		end
	end
end

function CheckPlayerCollision()
	
	if g.player.isAlive and not g.player.isHidden then
		local c = rigidBody:rayCast( rigidBody:posX(), rigidBody:posY(), rigidBody:posX() + (iconW/pixelsPerUnit) / 2 * dir, rigidBody:posY())
		if c.collides then
			if c.entity:collisionCategory() == CollisionCategory_Player then
				rigidBody:setType( RigidBody_STATIC )
				frozenDuringCollision = true
				g.player.PlayerCaught()
				if c.x < rigidBody:posX() then
					dir = 1
				else
					dir = -1
				end
				return
			end
		end
		c = rigidBody:rayCast( rigidBody:posX(), rigidBody:posY(), rigidBody:posX() - (iconW/pixelsPerUnit) / 8 * dir, rigidBody:posY())
		if c.collides then
			if c.entity:collisionCategory() == CollisionCategory_Player then
				frozenDuringCollision = true
				rigidBody:setType( RigidBody_STATIC )
				g.player.PlayerCaught()
				if c.x < rigidBody:posX() then
					dir = 1
				else
					dir = -1
				end
				return
			end
		end
		
		c = rigidBody:rayCast( rigidBody:posX(), rigidBody:posY() - (iconH/pixelsPerUnit) / 1.5, rigidBody:posX(), rigidBody:posY())
		if c.collides then
			if c.entity:collisionCategory() == CollisionCategory_Player then
				frozenDuringCollision = true
				rigidBody:setType( RigidBody_STATIC )
				g.player.PlayerCaught()
				if g.player.rigidBody:posX() < rigidBody:posX() then
					dir = 1
				else
					dir = -1
				end
				return
			end
		end
	end
end

function CheckPursuing()
	
	if g.player.isAlive and not g.player.isHidden then
		local startingX = rigidBody:posX()
		local collisionCount = 0
		local collisionLimit = 3
		viewDistance = 1.5
		if g.player.isInLight or g.player.IsSprinting() then
			viewDistance = 2.75
		end
		repeat
		c = rigidBody:rayCast( startingX, rigidBody:posY() - iconH/pixelsPerUnit/5, rigidBody:posX() + (iconW/pixelsPerUnit) * viewDistance * dir, rigidBody:posY() - iconH/pixelsPerUnit/5 )
		if ( c.collides ) then
			if c.entity:collisionCategory() == CollisionCategory_World or c.entity:collisionCategory() == CollisionCategory_Physics then
				return false
			end
			if c.entity:collisionCategory() == CollisionCategory_Player then
				lastKnownLocation = g.player.rigidBody:posX()
				return true
			end
			startingX = c.x + .01 * dir
			collisionCount = collisionCount + 1
		end
		until c.collides == false or collisionCount >= collisionLimit
	end
	return false
end

function CheckBehind()

	if g.player.isAlive and not g.player.isHidden and g.player.IsSprinting() and PathFound( -dir ) then
		
		local startingX = rigidBody:posX()
		local collisionCount = 0
		local collisionLimit = 3
		viewDistance = 1.5

		repeat
		c = rigidBody:rayCast( startingX, rigidBody:posY() - iconH/pixelsPerUnit/5, rigidBody:posX() - (iconW/pixelsPerUnit) * viewDistance * dir, rigidBody:posY() - iconH/pixelsPerUnit/5 )
		if ( c.collides ) then
			if c.entity:collisionCategory() == CollisionCategory_World or c.entity:collisionCategory() == CollisionCategory_Physics then
				return
			end
			if c.entity:collisionCategory() == CollisionCategory_Player then
				dir = -dir
				rigidBody:setLinearVelocity( dir, 0 )
				changeDirTime = 1
				return
			end
			startingX = c.x - .01 * dir
			collisionCount = collisionCount + 1
		end
		until c.collides == false or collisionCount >= collisionLimit
	end
	return

end

function CheckPath()
	
	local startingX = rigidBody:posX()
	local startingY = rigidBody:posY()
	local collisionCount = 0
	local collisionLimit = 3
	repeat
	c = rigidBody:rayCast( startingX, startingY, rigidBody:posX() + iconW/pixelsPerUnit/4 * dir, rigidBody:posY() + iconH/pixelsPerUnit/1.95 )
	if not c.collides or c.entity:collisionCategory() == CollisionCategory_Elevator then
		floorCheckCount = floorCheckCount + 1
		if floorCheckCount > 5 then
			floorCheckCount = 0
			--debugPrint("Path ran out, turning around")
			dir = -dir
			rigidBody:setLinearVelocity( rigidBody:linearVelocityX() * dir, 0 )
			if changeDirTime < .5 then
				isTrapped = true
			else
				changeDirTime = 0
			end
		end
		return
	else
		if c.entity:collisionCategory() == CollisionCategory_Ladder or c.entity:collisionCategory() == CollisionCategory_NONE then
			startingX = c.x + 0.1 * dir
			startingY = c.y + 0.1
			collisionCount = collisionCount + 1
		else
			floorCheckCount = 0
			return
		end
	end
	until c.collides == false or collisionCount >= collisionLimit
	
end

function PathFound( dirCheck )
	local startingX = rigidBody:posX()
	local startingY = rigidBody:posY()
	local collisionCount = 0
	local collisionLimit = 3
	repeat
	c = rigidBody:rayCast( startingX, startingY, rigidBody:posX() + iconW/pixelsPerUnit/4 * dirCheck, rigidBody:posY() + iconH/pixelsPerUnit/1.95 )
	if not c.collides or c.entity:collisionCategory() == CollisionCategory_Elevator then
		return false
	else
		if c.entity:collisionCategory() == CollisionCategory_Ladder or c.entity:collisionCategory() == CollisionCategory_NONE then
			startingX = c.x + 0.1 * dirCheck
			startingY = c.y + 0.1
			collisionCount = collisionCount + 1
		else
			return true
		end
	end
	until c.collides == false or collisionCount >= collisionLimit
	return true
end

function AICheckReverseHorizontalDirection()
	
	local startingX = rigidBody:posX()
	local startingY = rigidBody:posY()
	local collisionCount = 0
	local collisionLimit = 3
	repeat
	c = rigidBody:rayCast( startingX, startingY, rigidBody:posX() + (iconW/pixelsPerUnit)/1.5 * dir, rigidBody:posY() + (iconH/pixelsPerUnit)/3 )
	if ( c.collides ) then
		if( c.entity:collisionCategory() == CollisionCategory_World or c.entity:collisionCategory() == CollisionCategory_Physics ) then
			--debugPrint("Ran into object, turning around")
			dir = -dir
			rigidBody:setLinearVelocity( dir, 0 )
			if changeDirTime < .5 then
				isTrapped = true
			else
				changeDirTime = 0
			end
			return
		else
			startingX = c.x + 0.1 * dir
			startingY = c.y
			collisionCount = collisionCount + 1
		end
		
	end
	until c.collides == false or collisionCount >= collisionLimit
end

function KnockOut()
	if not isKnockedOut then
		iconVisible = true
		isKnockedout = true
		g.aiKnockoutSound:play(false)
		rigidBody:setType( RigidBody_STATIC )
		rigidBody:setCollisionCategory( CollisionCategory_NONE )
		rigidBody:setCollisionMask( CollisionCategory_NONE )
		updateAlways = false
	end
end

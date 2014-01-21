top = 0
bottom = 0
moveSpeed = 1
elevatorID = 0
dir = 1
distance = 0
xPos = 0
currentVel = 0
initialDir = "UP"
isMoving = false
gradualAccel = false

movingSound = nil
endSound = nil

ui =
{
	moveSpeed      = { order = 1,  type = "number",  label = "Movement Speed" },
	distance      = { order = 2,  type = "number",  label = "Distance to Move" },
	elevatorID	  = { order = 3, type = "number", label = "Elevator ID" },
	initialDir        = { order = 4,  type = "list", values = { "UP", "DOWN" }, label = "Initial Direction" },
	gradualAccel	= { order = 5,  type = "bool", label = "Gradual Acceleration" },
}

function init()

	if ( rigidBody ~= nil ) then
		g.physics:remove( rigidBody )
	end
	rigidBody = g.physics:createBox( x, y, RigidBody_STATIC, iconW/pixelsPerUnit, iconH/pixelsPerUnit, 1.0 )
	rigidBody:setCollisionCategory( CollisionCategory_World )
	rigidBody:setCollisionMask( CollisionMask_World )	
	
	if( initialDir == "DOWN" ) then
		dir = 1
		top = rigidBody:posY()
		bottom = top + distance * iconH/pixelsPerUnit
	else
		dir = -1
		bottom = rigidBody:posY()
		top = bottom - distance * iconH/pixelsPerUnit
	end
	
	elevatorID = g.math.clamp( elevatorID, 0, 16 )
	local count = 0
	local currentID = 1
	while count < elevatorID do
		currentID = currentID * 2
		count = count + 1
	end
	updateAlways = true
	this.id = "Elevator" .. currentID
	
	thisEvent = new( g.eventStruct )
	thisEvent = g.EventSystem_LocateEventMapping( "evtCallElevator" )
	if thisEvent ~= false then
		thisEvent.sender = this.name ..".".. this.id
		thisEvent.destinationIDs = { this.id }
		EventClientComponent_RegisterListener( thisEvent )
	end
	
	moveSpeed  = g.math.clamp( g.math.abs( moveSpeed ), 0.1, 128 )
	xPos = rigidBody:posX()
	
	movingSound = Sound("sm_sfx_elevator_mid.wav")
	endSound = Sound("sm_sfx_elevator_end.wav")
	
end

function update( dt )
	--[[relativeX = Image_toScreenX( rigidBody:posX() )
	relativeY = Image_toScreenY( rigidBody:posY() )
	if relativeX >= 0 and relativeX < width() and relativeY >= 0 and relativeY < height() then
		debugPrint( id )
	end
	debugPrint( "RigidBodyX: " .. rigidBody:posX() .. " ViewX: " .. Image_toScreenX( rigidBody:posX() ) )
	debugPrint( "RigidBodyY: " .. rigidBody:posY() .. " ViewY: " .. Image_toScreenY( rigidBody:posY() ) )]]--
end

function fixedUpdate( dt )

	vx = rigidBody:linearVelocityX()
	vy = rigidBody:linearVelocityY()
	
	if isMoving then
		iconAnimTime = iconAnimTime + dt
		if rigidBody:posY() <= top and dir == -1 then
			movingSound:stop()
			relativeX = Image_toScreenX( rigidBody:posX() )
			relativeY = Image_toScreenY( rigidBody:posY() )
			if relativeX >= 0 and relativeX < width() and relativeY >= 0 and relativeY < height() then
				endSound:play( false )
			end
			isMoving = false
			dir = -dir
			vy = 0
			currentVel = 0
			rigidBody:setPos( rigidBody:posX(), top )
			rigidBody:setType( RigidBody_STATIC )
			
			else if rigidBody:posY() >= bottom and dir == 1 then
				movingSound:stop()
				relativeX = Image_toScreenX( rigidBody:posX() )
				relativeY = Image_toScreenY( rigidBody:posY() )
				if relativeX >= 0 and relativeX < width() and relativeY >= 0 and relativeY < height() then
					endSound:play( false )
				end
				isMoving = false
				dir = -dir
				vy = 0	
				currentVel = 0
				rigidBody:setPos( rigidBody:posX(), bottom )
				rigidBody:setType( RigidBody_STATIC )
			else
				
				if( gradualAccel ) then
				
					if dir == -1 then
						maxSpeed = g.math.clamp( top - rigidBody:posY(), -moveSpeed, 0 )
						currentVel = g.math.clamp( currentVel - .2, maxSpeed, -6 )
					else
						maxSpeed = g.math.clamp( bottom - rigidBody:posY(), 0, moveSpeed )
						currentVel = g.math.clamp( currentVel + .025, 2, maxSpeed )
					end
					rigidBody:setLinearVelocity( 0, currentVel )			
				
				else
					vy = dir * moveSpeed
					rigidBody:setLinearVelocity( 0, vy )
					if vy > 0 then
						CheckCrushing()
					end
				end
			end
			rigidBody:setPos( xPos, rigidBody:posY() )
		end
	else
		iconAnimTime = 0
	end

end

function beginContact( collidedWith )
	if isMoving and dir == 1 then
		if collidedWith:isType("Player") and collidedWith.rigidBody:posY() > rigidBody:posY() then
			collidedWith.PlayerCaught()
		end
	end
end

function CheckCrushing()
	cast = rigidBody:rayCast( rigidBody:posX(), rigidBody:posY(), rigidBody:posX(), rigidBody:posY() + iconH/pixelsPerUnit/1.99)
	if cast.collides then
		if cast.entity:collisionCategory() == CollisionCategory_Player then
			g.player.PlayerCaught()
		end
	end
end

function activateElevator()
	if not isMoving then
		relativeX = Image_toScreenX( rigidBody:posX() )
		relativeY = Image_toScreenY( rigidBody:posY() )
		if relativeX >= 0 and relativeX < width() and relativeY >= 0 and relativeY < height() then
			movingSound:play( true )
		end
		isMoving = true
		rigidBody:setType( RigidBody_DYNAMIC )
	end
end
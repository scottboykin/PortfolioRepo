isAlive = true
lightOn = false
currentDelay = 0
flickerTime = 0
circuitID = 0
newState = "idle"
sabotageable = true
dir = 1

ui =
{
	circuitID = { order = 1, type = "number", label = "Circuit ID" },
	sabotageable = { order = 2, type = "bool", label = "Can be Sabotaged" }
}

function init()

	if rigidBody ~= nil then
		g.physics:remove( rigidBody )
	end
	rigidBody = g.physics:createBox( x, y, RigidBody_STATIC, iconW/pixelsPerUnit, iconH/pixelsPerUnit, 1.0 )
	rigidBody:setCollisionCategory( CollisionCategory_Light )
	rigidBody:setCollisionMask( CollisionMask_NONE )	
		
	iconVisible = false
	--[[if sabotageable then
		circuitID = g.math.clamp( circuitID, 0, 16 )
		local count = 0
		local currentID = 1
		while count < circuitID do
			currentID = currentID * 2
			count = count + 1
		end
		this.id = "Fusebox" .. currentID
		updateAlways = true
	
		thisEvent = new( g.eventStruct )
		thisEvent = g.EventSystem_LocateEventMapping( "evtLightSabotage" )
		if thisEvent ~= false then
			thisEvent.sender = this.name ..".".. this.id
			thisEvent.destinationIDs = { circuitID }
			EventClientComponent_RegisterListener( thisEvent )
		end
	end]]--
end

function update( dt )
	if isAlive then
		currentDelay = currentDelay + dt
		if currentDelay >= flickerTime then
			currentDelay = 0
			lightOn = not lightOn
			if lightOn then
				flickerTime = g.math.random() * 3
			else
				flickerTime = g.math.random() + 0.5
			end
		end
		if lightOn and not g.player.isInLight then
			g.player.isInLight = checkDistance()
		end
	end
end

function render( dt )
	if isAlive and lightOn then
		iconAnim:getFrame( 1 ):draw( rigidBody:posX(), rigidBody:posY() )
	else
		iconAnim:getFrame( 0 ):draw( rigidBody:posX(), rigidBody:posY() )
	end
	
end

function checkDistance()
	return ( g.math.abs(rigidBody:posX() - g.player.rigidBody:posX()) < iconW/pixelsPerUnit/2 and
		g.math.abs(rigidBody:posY() - g.player.rigidBody:posY()) < iconH/pixelsPerUnit/2 )
end
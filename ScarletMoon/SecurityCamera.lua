drawCollision = false
circuitID = 0
isAlive = true
playerCaught = false
direction = ""
newState = "idle"
dir = 1
sideX = 0
topY = 0

ui = 
{ 
	circuitID = { order = 1, type = "number", label = "Circuit ID" },
	direction = { order = 2, type = "list", values = { "LEFT", "RIGHT" }, label = "Direction" },
	drawCollision = { order = 3, type = "bool", label = "Show collision" } 
}


local debugComponent = false
local collided = false
local collidedFrames = 0
local collidedWith = ""
local valid = false


--MM: Function called every time the levels loads or reload
function init()
	if ( rigidBody ~= nil ) then
		g.physics:remove( rigidBody )
	end
	--[[rigidBody = g.physics:createBox( x, y, RigidBody_SENSOR, iconW/pixelsPerUnit, iconH/pixelsPerUnit, 1.0 )
	rigidBody:setCollisionCategory( CollisionCategory_Hazard )
	rigidBody:setCollisionMask( CollisionCategory_Player )]]--
	
	circuitID = g.math.clamp( circuitID, 0, 16 )
	local count = 0
	local currentID = 1
	while count < circuitID do
		currentID = currentID * 2
		count = count + 1
	end
	updateAlways = true
	this.id = "Fusebox" .. currentID
	
	if direction == "LEFT" then
		dir = 1
	else 
		dir = -1
	end
	
	thisEvent = new( g.eventStruct )
	thisEvent = g.EventSystem_LocateEventMapping( "evtLightSabotage" )
	if thisEvent ~= false then
		thisEvent.sender = this.name ..".".. this.id
		thisEvent.destinationIDs = { circuitID }
		EventClientComponent_RegisterListener( thisEvent )
	end
	
end


--MM: Function called every frame
function update( dt )
	
end

function fixedUpdate( dt )
	--if g.EventSystem_BroadcastListContains( "evtLightSabotage" ) then
	--	event_LightSabotage()
	--end
	if isAlive and not playerCaught then
		checkForPlayer()
	end
	
	if playerCaught then
		newState = "move"
	end
end

function checkForPlayer()
		topY = rigidBody:posY() - (iconH/pixelsPerUnit) / 2.5
		if dir == 1 then
			sideX = rigidBody:posX() - (iconW/pixelsPerUnit) / 2.5
		else
			sideX = rigidBody:posX() + (iconW/pixelsPerUnit) / 2.5
		end
		
		local startingX = sideX
		local startingY = topY
		local collisionCount = 0
		local collisionLimit = 3

		repeat
			local c = rigidBody:rayCast( startingX, startingY, rigidBody:posX(), rigidBody:posY() + (iconH/pixelsPerUnit) / 2 )
			if c.collides then
				if c.entity:collisionCategory() == CollisionCategory_World or c.entity:collisionCategory() == CollisionCategory_Physics then
						return
					end
				if c.entity:collisionCategory() == CollisionCategory_Player then
					g.player.PlayerCaught()
					playerCaught = true
					return
				end
				startingX = c.x + .01 * dir
				startingY = c.y + .01
				collisionCount = collisionCount + 1
			end
		until c.collides == false or collisionCount >= collisionLimit
		
		local startingX = sideX
		local startingY = topY
		local collisionCount = 0
		local collisionLimit = 3
		
		repeat
			local c = rigidBody:rayCast( startingX, startingY, rigidBody:posX() + (iconW/pixelsPerUnit) / 4 * dir, rigidBody:posY() + (iconH/pixelsPerUnit) / 2 )
			if c.collides then
				if c.entity:collisionCategory() == CollisionCategory_World or c.entity:collisionCategory() == CollisionCategory_Physics then
						return
					end
				if c.entity:collisionCategory() == CollisionCategory_Player then
					g.player.PlayerCaught()
					playerCaught = true
					return
				end
				startingX = c.x + .01 * dir
				startingY = c.y + .01
				collisionCount = collisionCount + 1
			end
		until c.collides == false or collisionCount >= collisionLimit
end

function render( dt )
	if debugComponent and isAlive then
		drawLine( sideX, topY, rigidBody:posX(), rigidBody:posY() + (iconH/pixelsPerUnit) / 2, 255, 0, 0 ) 
		drawLine( sideX, topY, rigidBody:posX() + (iconW/pixelsPerUnit) / 4 * dir, rigidBody:posY() + (iconH/pixelsPerUnit) / 2, 255, 0, 0 )
	end
	
end

--[[MM: Catches evtHazard thrown by any trigger
function event_Hazard()
	--DamageActor( actorToDamage )
end--]]
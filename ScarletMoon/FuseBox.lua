drawCollision = false
circuitID = 0
isAlive = true
thisEvent = nil
dir = 1

ui = 
{ 
	circuitID = { order = 1, type = "number", label = "Circuit ID" }, 
	drawCollision = { order = 2, type = "bool", label = "Show collision" } 
}


local debugComponent = true
local collided = false
local collidedFrames = 0
local collidedWith = ""
local valid = false


--MM: Function called every time the levels loads or reload
function init()
	
	if rigidBody ~= nil then
		g.physics:remove( rigidBody )
	end
	rigidBody = g.physics:createBox( x, y, RigidBody_SENSOR, iconW/pixelsPerUnit, iconH/pixelsPerUnit, 1.0 )
	rigidBody:setCollisionCategory( CollisionCategory_Fusebox )
	rigidBody:setCollisionMask( CollisionCategory_Player )
	
	circuitID = g.math.clamp( circuitID, 0, 16 )
	
	local count = 0
	local currentID = 1
	while count < circuitID do
		currentID = currentID * 2
		count = count + 1
	end
	
	circuitID = "Fusebox" .. currentID
	thisEvent = new( g.eventStruct )
		thisEvent = g.EventSystem_LocateEventMapping( "evtLightSabotage" )
		if thisEvent ~= false then
			thisEvent.sender = this.name ..".".. this.id
			thisEvent.destinationIDs = { circuitID }
		end
end


--MM: Function called every frame
function update( dt )
	
end

function fixedUpdate( dt )

end

--JS: Assumes collidedWith is the Player. Needs code for other cases.
function beginContact( collidedWith )

end

function sabotageFuse()
	if( isAlive ) then
		EventClientComponent_PostEvent( thisEvent )
		isAlive = false
		newState = "dead"
	end
end
--[[MM: Catches evtHazard thrown by any trigger
function event_Hazard()
	--DamageActor( actorToDamage )
end--]]
elevatorID = 0
thisEvent = nil
dir = 1
delay = 0
activated = false

startSound = nil

ui =
{
	elevatorID = { order = 1, type = "number", label = "Elevator ID" }
}

function init()
	
	if rigidBody ~= nil then
		g.physics:remove( rigidBody )
	end
	rigidBody = g.physics:createBox( x, y, RigidBody_SENSOR, iconW/pixelsPerUnit, iconH/pixelsPerUnit, 1.0 )
	rigidBody:setCollisionCategory( CollisionCategory_Elevator )
	rigidBody:setCollisionMask( CollisionCategory_World )
	
	elevator = g.math.clamp( elevatorID, 0, 16 )
	
	local count = 0
	local currentID = 1
	while count < elevatorID do
		currentID = currentID * 2
		count = count + 1
	end
	
	elevatorID = "Elevator" .. currentID
	thisEvent = new( g.eventStruct )
	thisEvent = g.EventSystem_LocateEventMapping( "evtCallElevator" )
	if thisEvent ~= false then
		thisEvent.sender = this.name ..".".. this.id
		thisEvent.destinationIDs = { elevatorID }
	end
	
	startSound = Sound("sm_sfx_elevator_start.wav")
	
end

function update( dt )
	if activated then
		delay = delay + dt
		if delay > 1 then
			EventClientComponent_PostEvent( thisEvent )
			activated = false
		end
	end
end

function beginContact( o )
end

function Activate()
	if not activated then
		activated = true
		startSound:play( false )
		delay = 0
	end
end
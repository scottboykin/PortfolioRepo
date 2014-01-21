drawCollision = false
isTransitionDoor = false
isLocked = false
isInUse  = false
levelToLoad = ""
newState = "idle"
dir = 1


ui = 
{ 
isLocked = { order = 1, type = "bool", label = "Locked" },
isTransitionDoor = { order = 2, type = "bool", label = "Transition Door" },
levelToLoad = { order = 3, type = "string", label = "Level to Load" },
drawCollision = { order = 4, type = "bool", label = "Show collision" },
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
	rigidBody:setCollisionCategory( CollisionCategory_Door )
	rigidBody:setCollisionMask( CollisionCategory_Player )

	if isLocked then
		newState = "stun"
	end
	
end


--MM: Function called every frame
function update( dt )
	
	newState = "idle"
	if isLocked then
		newState = "stun"
	end
	if isInUse then
		newState = "hide"
	end
end

function render( dt )
	
end

--JS: Assumes collidedWith is the Player. Needs code for other cases.
function beginContact( collidedWith )

end

function tardisDoor()

	g.thisLevel = levelToLoad
	loadLevel( g.thisLevel ) --without .g2d

end

function Unlock()
	isLocked = false
	newState = "idle"
	g.player.lockpicks = g.player.lockpicks - 1
end


--[[MM: Catches evtHazard thrown by any trigger
function event_Hazard()
	--DamageActor( actorToDamage )
end--]]
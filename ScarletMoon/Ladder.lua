drawCollision = false


ui = { drawCollision = { order = 1, type = "bool", label = "Show collision" } }


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
	rigidBody = g.physics:createBox( x, y, RigidBody_SENSOR, iconW/pixelsPerUnit, iconH/pixelsPerUnit, 1.0 )
	rigidBody:setCollisionCategory( CollisionCategory_Ladder )
	rigidBody:setCollisionMask( CollisionCategory_Player )
end


--MM: Function called every frame
function update( dt )
	if ( debugComponent ) or ( drawCollision ) then
		--debugDrawCollision( this, {255,255,0} )
	end
end

function fixedUpdate( dt )

	if (g.math.abs(rigidBody:posX() - g.player.rigidBody:posX()) < g.player.iconW/pixelsPerUnit/2) and
				(g.math.abs(rigidBody:posY() - g.player.rigidBody:posY()) < g.player.iconH/pixelsPerUnit/2) then
		g.player.onLadder = true
		if g.player.isFalling or g.player.isJumping then
			g.player.isClimbing = true
			g.player.isFalling = false
			g.player.isJumping = false
			g.player.rigidBody:setPos( rigidBody:posX(), g.player.rigidBody:posY() )
		end
	end
	
end


--JS: Assumes collidedWith is the Player. Needs code for other cases.
function beginContact( collidedWith )

end


--[[MM: Catches evtHazard thrown by any trigger
function event_Hazard()
	--DamageActor( actorToDamage )
end--]]
--#require Collision


--JS: File load and execution priority
priority = -4800


friction  = 0.9
breakTime = 0.2
physicsTouchDrawCollision = false


ui = {
	breakTime = { order = 1, type = "number", label = "Delay" },
	friction  = { order = 2, type = "number", label = "Floor Friction" },
	physicsTouchDrawCollision = { order = 3, type = "bool", label = "Show Collision?" },
}


local debugComponent = false
local valid = false


function init()
	if ( rigidBody == nil ) then
		debugPrint( self.id .. " : PHYSICSONTOUCH : init : No collision defined. Make sure the Collision component is being used properly" )
		return
	end

	valid = true

	friction  = g.math.clamp( friction, 0, 1 )
	breakTime = g.math.clamp( breakTime, 0, 65535 )
end


function update( dt )
	if ( valid ) then
		if ( debugComponent ) or ( physicsTouchDrawCollision ) then
			debugDrawCollision( this, {255,255,0} )
		end
	end
end


function fixedUpdate( dt )
	if ( valid ) then
		vx = rigidBody:linearVelocityX()
		vy = rigidBody:linearVelocityY()
		vx = vx * ( 1 - friction )
		if ( vx > -0.1 ) and ( vx < 0.1 ) then
			vx = 0
		end
		vy = clamp( vy + 1, vy, 20 )
		rigidBody:setLinearVelocity( vx, vy )
	end
end


function beginContact( o )
	if ( valid ) then
		if ( o:isType("Player") or o:isType("PhysicsOnTouch") ) then
			if o.rigidBody:posY() < rigidBody:posY() then
				setTimer( breakTime, fall )
				debugPrint( "Contact with movable object" )
				o.rigidBody:setLinearVelocity( o.rigidBody:linearVelocityX(), 0 )
			end
		end
	end
	
	if o:isType("AI") then
			if rigidBody:linearVelocityY() > 2 and g.math.abs(rigidBody:posX() - o.rigidBody:posX()) < o.iconW/pixelsPerUnit then
				o.KnockOut()
			end
		end
end


function fall()
	if ( valid ) then
		rigidBody:setType( RigidBody_DYNAMIC )
		rigidBody:setFixedRotation( false )
		rigidBody:setAngularVelocity( (g.math.random()-0.5)*3 )
	end
end
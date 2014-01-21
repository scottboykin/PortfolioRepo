drawCollision = false
isAlive = true

ui = 
{ 
	drawCollision = { order = 2, type = "bool", label = "Show collision" } 
}

function init()

	rigidBody:setCollisionCategory( CollisionCategory_NONE )
	rigidBody:setCollisionMask( CollisionMask_NONE )

end

function update( dt )

end

function fixedUpdate( dt )

end
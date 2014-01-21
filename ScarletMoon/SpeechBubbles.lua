speechBubble = nil
transparency = 1
fadeTime = 0
fade = false
isAlive = true

ui =
{}

function init()
	
	if rigidBody ~= nil then
		g.physics:remove( rigidBody )
	end
	
	iconVisible = false
	speechBubble = Image( "SpeechBubbles/" .. g.player.currentObjective .. ".png" )
	iconAnimTime = 0
	transparency = 1
	fadeTime = 0
	rigidBody:setPos( g.player.rigidBody:posX(), g.player.rigidBody:posY() - iconH/pixelsPerUnit - g.player.iconH/pixelsPerUnit/2 )
end

function fixedUpdate( dt )
	if isAlive and g.player.isAlive then
		rigidBody:setPos( g.player.rigidBody:posX(), g.player.rigidBody:posY() - speechBubble:height()/pixelsPerUnit/2 - g.player.iconH/pixelsPerUnit/2 )
		if fade then
			transparency = transparency - .05
			if transparency <= 0 then
				isAlive = false
			end
		end
	end
end

function postUpdate( dt )
	if isAlive and g.player.isAlive then
		speechBubble:setTransparency(transparency)	
		speechBubble:draw( rigidBody:posX(), rigidBody:posY(), 0, Image_COORDS_GAME )
	end
end

function fadeBubble()
	fade = true
end

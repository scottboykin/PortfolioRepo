dofile( self, APP_PATH .. "scripts/include/xinput_defines.lua" )

-- DLL that wraps XInput
g.require( "XInputLuaWrapper" )

loadDelay = 10
currentTime = 0
levelToLoad = ""
x = 0
y = 0
scrollSpeedX = 0
scrollSpeedY = 0
textPosX = 0
textPosY = 0
output = "Press Enter to Proceed"
targetX = 0
targetY = 5
topY = 0
bottomY = 0
buffer = 1.0
startPos = ""
thisFont = nil
loading = false
controlsImage = nil
loadScreenImage = nil
loadDelay = 0

ui = 
{
	startPos = { order = 1, type = "list", values = { "TOP", "CENTER", "BOTTOM" }, label = "Starting Position" },
	scrollSpeedY = { order = 2, type = "number", label = "Y Scroll Speed" },
	levelToLoad = { order = 3, type = "string",  label = "Level To Load" },
	loadScreen = { order = 4, type = "string",  label = "Loading Screen" }
}

function init()
	
	if( startPos == "TOP" ) then
		targetY = (rigidBody:posY() - iconH/pixelsPerUnit/2) + Image_viewHeight()/2
	end
	
	if startPos == "CENTER" then
		targetY = 0
	end
	
	if startPos == "BOTTOM" then
		targetY = (rigidBody:posY() + iconH/pixelsPerUnit/2) - Image_viewHeight()/2
	end
	
	topY = (rigidBody:posY() - iconH/pixelsPerUnit/2) + Image_viewHeight()/2
	bottomY = (rigidBody:posY() + iconH/pixelsPerUnit/2) - Image_viewHeight()/2
	
	thisFont = Font( "arial", 20 )
	Image_setZoomScaleFactor( 1 )
	
	controlsImage = Image( "MenuHUD/Cutscene_Controls.png" )
	if loadScreen and loadScreen ~= "" then
		loadScreenImage = Image( "MenuHUD/" .. loadScreen .. ".png" )
	end

	controlImagePosX = 0
	controlImagePosY = 0
	
end

function update( dt )

	targetX = targetX + scrollSpeedX*dt
	Image_setViewOffset( targetX, targetY )
	

end

function fixedUpdate( dt )	
	if not loading then
		g.XInputLuaWrapper.query_xinput_state()
		
		scrollSpeed = scrollSpeedY
		if g.keys["SHIFT"] or IsRightTriggerDown() then
			scrollSpeed = scrollSpeed * 2
		end

		if g.keys["DOWN"] or g.keys["S"] then
			targetY = targetY + scrollSpeed*dt
			if targetY > bottomY then
				targetY = bottomY
			end
		end
		
		if g.keys["UP"] or g.keys["W"] then
			targetY = targetY - scrollSpeed*dt
			if targetY < topY then
				targetY = topY
			end
		end
		
		local joyLeftX, joyLeftY = g.XInputLuaWrapper.get_joystick_position( "left" )
		local len = joyLeftX * joyLeftX + joyLeftY * joyLeftY
		
		if (len > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE and joyLeftY < -30000) or IsButtonDown(XINPUT_GAMEPAD_DPAD_DOWN) then
			targetY = targetY + scrollSpeed*dt
			if targetY > bottomY then
				targetY = bottomY
			end
		end
		
		if (len > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE and joyLeftY > 30000) or IsButtonDown(XINPUT_GAMEPAD_DPAD_UP) then
			targetY = targetY - scrollSpeed*dt
			if targetY < topY then
				targetY = topY
			end
		end	
		
		if g.keys["RETURN"] then --or g.keys["E"] then
			loadNextLevel()
		end
		
		if g.XInputLuaWrapper.is_button_down( XINPUT_GAMEPAD_START ) then --[[or g.XInputLuaWrapper.is_button_down( XINPUT_GAMEPAD_A )
			or g.XInputLuaWrapper.is_button_down( XINPUT_GAMEPAD_X )  or g.XInputLuaWrapper.is_button_down( XINPUT_GAMEPAD_B ) then]]--
			loadNextLevel()
		end
	else
		loadDelay = loadDelay + dt
		if loadDelay > .25 then
			loadLevel( levelToLoad )
		end
	end
end

function loadNextLevel()
	loading = true
	if loadScreenImage ~= nil then
		iconVisible = false
	end
end

function render( dt )
	if not loading then
		if controlsImage ~= nil then
			controlsImage:draw( controlsImage:width()/2, controlsImage:height()/2, 0, Image_COORDS_SCREEN )	
		end
	else
		if loadScreenImage ~= nil then
			loadScreenImage:draw( width()/2, height()/2, 0, Image_COORDS_SCREEN )	
		end
	end
end

function IsButtonDown( button )
	return g.XInputLuaWrapper.is_button_down( button )
end

function IsRightTriggerDown()
	rightTrigger = g.XInputLuaWrapper.get_trigger_position( "right" )
	if rightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD then
		return true
	end
end
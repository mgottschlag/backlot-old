
if Client ~= nil then
	gibs:setVisible(false)
	if this:isLocal() then
		-- Center player on the screen
		local camera = Graphics.get():getCamera()
		camera:setPosition(position:getVector2F())
	end
end

function on_loaded()
	print("Entity loaded.");
	if Client ~= nil then
		print("Entity: Client.");
		if this:isLocal() then
			print("Entity: local.");
			Game.get():setInputTarget(this);
		end
	end
	if Server ~= nil then
	end
end
function on_changed(property)
	if property:getName() == "position" then
		if Client ~= nil and this:isLocal() then
			-- Center player on the screen
			local camera = Graphics.get():getCamera()
			camera:setPosition(property:getVector2F())
		end
	end
	if property:getName() == "rotation" then
		if Client ~= nil then
			body:setRotation(0);
			body:setPosition(-0.5, -0.5);
			body:rotate(property:getFloat() + 180, 0.5, 0.5);
			weapon:setRotation(0);
			weapon:setPosition(-0.5, -0.5);
			weapon:rotate(property:getFloat() + 180, 0.5, 0.5);
		end
	end
	-- Movement
	if property:getName() == "keys" then
		local speed = Vector2F(0, 0)
		-- Get movement speed
		if keys:bit(0) == true then
			speed.y = speed.y + 1;
		end
		if keys:bit(1) == true then
			speed.y = speed.y - 1;
		end
		if keys:bit(2) == true then
			speed.x = speed.x + 1;
		end
		if keys:bit(3) == true then
			speed.x = speed.x - 1;
		end
		if speed:getLength() > 0.1 then
			speed = speed / speed:getLength() * 2
		end
		-- Set movement
		if health:getInt() == 0 then
			return
		end
		this:setSpeed(speed, false);
		if Client ~= nil then
			feet:setRotation(0);
			feet:setPosition(-0.5, -0.5);
			if speed.x ~= 0 or speed.y ~= 0 then
				if not feetanim:isPlaying() then
					feetanim:start();
				end
				feet:rotate(Vector2F(speed.y, -speed.x):getRotation(), 0.5, 0.5);
			else
				if feetanim:isPlaying() then
					feetanim:stop();
				end
			end
		end
		if Server ~= nil then
			-- Shooting
			if keys:bit(4) == true then
				local weaponentity = Game.get():getEntity(currentweapon:getInt())
				if weaponentity.__ok then
					weaponentity:getScript():callFunction("start_shooting", this:getID())
				end
			else
				local weaponentity = Game.get():getEntity(currentweapon:getInt())
				if weaponentity.__ok then
					weaponentity:getScript():callFunction("stop_shooting")
				end
			end
		end
	end
	if property:getName() == "currentweapon" then
		print("Weapon changed. "..currentweapon:getInt())
		weaponentity = Game.get():getEntity(currentweapon:getInt())
		if weaponentity.__ok and Client ~= nil then
			-- TODO: Change HUD
			-- Change weapon texture
			local textureid = weaponentity:getScript():callFunctionInt("get_player_texture")
			texture = Texture.get(textureid)
			weapon:setTexture(texture)
			print("Texture. "..textureid)
		end
	end
	if property:getName() == "health" then
		if health:getInt() == 0 then
			-- Die
			this:setSpeed(Vector2F(0, 0), false);
			if Client ~= nil then
				gibs:setVisible(true)
				body:setVisible(false)
				feet:setVisible(false)
				weapon:setVisible(false)
			end
			if Server ~= nil then
				respawntimer = Timer();
				respawntimer:setInterval(1000)
				respawntimer:setCallback(this:getScript(), "respawn")
				respawntimer:start()
			end
			local weaponentity = Game.get():getEntity(currentweapon:getInt())
			if weaponentity.__ok then
				weaponentity:getScript():callFunction("stop_shooting")
			end
		else
			if Client ~= nil then
				gibs:setVisible(false)
				body:setVisible(true)
				feet:setVisible(true)
				weapon:setVisible(true)
			end
		end
	end
end
function on_set_weapon(weapon)
	print("on_set_weapon")
	currentweapon:setInt(weapon)
end
function on_mouse_input(rotangle)
	rotation:setFloat(rotangle)
end
function on_keyboard_input(key, state)
	print("Keyboard input.")
	if key == "up" then
		keys:bit(1, state)
	elseif key == "down" then
		keys:bit(0, state)
	elseif key == "left" then
		keys:bit(3, state)
	elseif key == "right" then
		keys:bit(2, state)
	elseif key == "shoot" then
		keys:bit(4, state)
	elseif key == "exit" then
		Menu.get("pause"):setActive(true)
	end
end

function on_update()
	-- Get weapon - this should be done by the engine really!
	if currentweapon:getInt() ~= 65535 then
		if weaponentity == nil or not weaponentity.__ok then
			weaponentity = Game.get():getEntity(currentweapon:getInt())
			if weaponentity.__ok and Client ~= nil then
				-- Change weapon texture
				local textureid = weaponentity:getScript():callFunctionInt("get_player_texture")
				texture = Texture.get(textureid)
				weapon:setTexture(texture)
			end
		end
	end
end

function do_damage(bulletid, damage)
	print("Damage done: "..damage)
	-- Change health points
	local hp = health:getInt()
	if hp == 0 then
		return
	end
	hp = hp - damage;
	if hp < 0 then
		hp = 0
	end
	health:setInt(hp)
end

function respawn()
	respawntimer = nil
	print("Respawning.")
	health:setInt(100)
	-- Get spawn point
	local spawnpoints = Game.get():getEntities("spawnpoint")
	if spawnpoints:getSize() == 0 then
		print("Warning: No spawn point set!")
		position:setVector2F(Vector2F(2, 2))
		return
	end
	local index = math.random(spawnpoints:getSize()) - 1
	local spawnpoint = spawnpoints:getEntity(index)
	position:setVector2F(spawnpoint:getPosition())
end

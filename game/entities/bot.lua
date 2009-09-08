
if Client ~= nil then
	gibs:setVisible(false)
end

function on_loaded()
end
function on_changed(property)
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
	if property:getName() == "currentweapon" then
		weaponentity = Game.get():getEntity(currentweapon:getInt())
		if weaponentity.__ok and Client ~= nil then
			-- Change weapon texture
			local textureid = weaponentity:getScript():callFunctionInt("get_player_texture")
			texture = Texture.get(textureid)
			weapon:setTexture(texture)
		end
	end
	if property:getName() == "health" then
		if health:getInt() == 0 then
			-- Die
			stopaction()
			respawn()
		end
	end
end

framespassed = 10
currentaction = 0
targetposition = Vector2F(0, 0)
pathfinder = nil
targetentity = nil

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
	if Client ~= nil then
		return
	end
	framespassed = framespassed + 1
	-- Change state only every 200 ms
	if framespassed >= 10 then
		framespassed = 0
		think()
	end
	-- Move
	if currentaction == 1 then
		if pathfinder:getStatus() == PathFinder.Ready then
			local pos = position:getVector2F()
			local nextpoint = pathfinder:getNextWaypoint(pos)
			print("Waypoint: "..nextpoint.x.."/"..nextpoint.y)
			direction = nextpoint - pos
			if direction:getLength() > 0.1 then
				direction = direction / direction:getLength() * 2
				rotation:setFloat(direction:getRotation() + 90)
			end
			this:setSpeed(direction, false);
		else
			this:setSpeed(Vector2F(0, 0), false);
			if pathfinder:getStatus() ~= PathFinder.Waiting then
				stopaction()
			end
		end
	else
		this:setSpeed(Vector2F(0, 0), false);
	end
end

function stopaction()
	currentaction = 0
	pathfinder = nil
	targetentity = nil
	targetposition = Vector2F(0, 0)
	local weaponentity = Game.get():getEntity(currentweapon:getInt())
	if weaponentity.__ok then
		weaponentity:getScript():callFunction("stop_shooting")
	end
	this:setSpeed(Vector2F(0, 0), false);
end

function attack(entity)
	-- Set current action
	currentaction = 2
	pathfinder = nil
	targetentity = entity
	-- Set rotation
	targetposition = targetentity:getPosition()
	local pos = position:getVector2F()
	local direction = targetposition - pos
	if direction:getLength() > 10 then
		stopaction()
		return
	end
	rotation:setFloat(direction:getRotation() + 90)
	-- Start shooting
	local weaponentity = Game.get():getEntity(currentweapon:getInt())
	if weaponentity.__ok then
		weaponentity:getScript():callFunction("start_shooting", this:getID())
	end
end

function think()
	-- Look whether there are any other players around
	local rect = this:getRectangle()
	rect.x = rect.x - 7
	rect.y = rect.y - 7
	rect.width = rect.width + 14
	rect.height = rect.height + 14
	local entitylist = Game.get():getEntities(rect, "player")
	if entitylist:getSize() > 0 then
		attack(entitylist:getEntity(0))
	end
	entitylist = Game.get():getEntities(rect, "bot")
	if entitylist:getSize() > 0 then
		if entitylist:getEntity(0):getID() ~= this:getID() then
			attack(entitylist:getEntity(0))
		elseif entitylist:getSize() > 1 then
			attack(entitylist:getEntity(1))
		end
	end
	-- TODO: Check whether we have reached our target
	if currentaction == 0 then
		-- Look for now point on the map to walk to
		local waypoints = Game.get():getEntities("waypoint")
		if waypoints:getSize() == 0 then
			print("Warning: No way point set!")
			return
		end
		local index = math.random(waypoints:getSize()) - 1
		local waypoint = waypoints:getEntity(index)
		targetposition = waypoint:getPosition()
		-- Start moving towards that point
		pathfinder = PathFinder(Server.get():getMap())
		pathfinder:initialize(position:getVector2F(), targetposition)
		currentaction = 1
		print("Moving to "..targetposition.x.."/"..targetposition.y)
	elseif currentaction == 2 then
		-- Set rotation
		targetposition = targetentity:getPosition()
		local pos = position:getVector2F()
		local direction = targetposition - pos
		rotation:setFloat(direction:getRotation() + 90)
	end
end

function on_set_weapon(weapon)
	print("on_set_weapon")
	currentweapon:setInt(weapon)
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

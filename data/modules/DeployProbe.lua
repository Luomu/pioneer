-- Deploy Probe mission:
-- Get a 'Probe' item from employer, land on a surface of an (unexplored)
-- planet, dump probe there, return.
-- This is not necessarily very thrilling but requires:
-- way to check landing on a planet
-- a quest specific item
-- a way to use the item
local max_delivery_dist = 20

local missions = {}
local ads = {}

local onChat = function(form, ref, option)
	local ad = ads[ref]

	form:Clear()

	if option == -1 then
		form:Close()
		return
	end

	if option == 0 then --greeting
		form:SetFace({ female = ad.isfemale, seed = ad.faceseed })
		form:SetMessage("Hello. There.\n\nYou will need one ton of free cargo space.")
	elseif option == 1 then --accept
		form:RemoveAdvertOnClose()

		ads[ref] = nil

		local mission = {
			type     = "Probing",
			client   = ad.client,
			location = ad.targetLocation,
			clientLocation = ad.location,
			reward   = ad.reward,
			due      = ad.due,
			flavour  = ad.flavour
		}

		local mref = Game.player:AddMission(mission)
		missions[mref] = mission

		form:SetMessage("Good. Get to it. Must be done by " .. Format.Date(ad.due))
		Game.player:AddEquip("MINING_MACHINERY")
		form:AddOption("Hang up.", -1)

		return
	end

	local stats = Game.player:GetStats()
	if stats.freeCapacity > 0 then
		form:AddOption("I'll do it.", 1)
		form:AddOption("Hang up.", -1)
	else
		form:AddOption("I don't have cargo space right now.", -1)
	end
end

local onDelete = function(ref)
	--is this a good way to remove?
	ads[ref] = nil
end

local makeAdvert = function(station)
	--need to find: landable planet in an unexplored system
	--reusing same planets is not a big problem
	local nearbysystems = Game.system:GetNearbySystems(max_delivery_dist, function (s) return #s:GetStationPaths() > 0 end)
	if #nearbysystems == 0 then return end

	local nearbysystem = nearbysystems[Engine.rand:Integer(1,#nearbysystems)]
	local dist = nearbysystem:DistanceTo(Game.system)

	local nearbystations = nearbysystem:GetStationPaths()
	local location = nearbystations[Engine.rand:Integer(1,#nearbystations)]
	local flavour = "WANTED. Someone to probe a ball of rock. Big rewards."
	local isfemale = Engine.rand:Integer(1) == 1
	local client = NameGen.FullName(isfemale)
	local due = Game.time + 3*24*60*60
	local reward = 5000

	local ad = {
		station  = station,
		flavour  = flavour,
		client   = client,
		location = station.path,
		targetLocation = location,
		due      = due,
		reward   = reward,
		isfemale = isfemale,
		faceseed = Engine.rand:Integer(),
		desc     = flavour,
	}

	local ref = station:AddAdvert(ad.desc, onChat, onDelete)
	ads[ref] = ad
end

local onCreateBB = function(station)
	--create a new mission advertisement
	makeAdvert(station)
end

local onUpdateBB = function(station)
	--delete expired missions
	--create new missions
end

local removeMission = function(ref)
	Game.player:RemoveMission(ref)
	missions[ref] = nil
end

local onShipDocked = function(ship, station)
	if not ship:IsPlayer() then return end
	--check for success, or failure
	--failure conditions:
	--time limit exceeded
	--probe misplaced
	for ref,mission in pairs(missions) do
		if mission.location == station.path then
			if mission.status == 'COMPLETED' then
				UI.ImportantMessage("That was excellent probing.", mission.client)
				Game.player:AddMoney(mission.reward)
				removeMission(ref)
			elseif mission.status == 'FAILED' or Game.time > mission.due then
				UI.ImportantMessage("You failure. No money for you.", mission.client)
				removeMission(ref)
			end
		end
	end
end

-- Jettisoning on surface = deploy
-- jettisoning in space is not instant fail as cargo is recoverable
local onDeploy = function(ship, cargo)
	if not ship:IsPlayer() then return end
	if cargo.type ~= 'MINING_MACHINERY' then return end
	print(ship.label .. " dumped some " .. cargo.type)
	for ref,mission in pairs(missions) do
		-- Correct location?
		mission.status = 'COMPLETED'
		mission.location = mission.clientLocation
		Game.player:UpdateMission(ref, mission)
	end
end

local loaded_data

local onGameStart = function ()
	ads = {}
	missions = {}

	if not loaded_data then return end

	for k,ad in pairs(loaded_data.ads) do
		local ref = ad.station:AddAdvert(ad.desc, onChat, onDelete)
		ads[ref] = ad
	end
	for k,mission in pairs(loaded_data.missions) do
		local mref = Game.player:AddMission(mission)
		missions[mref] = mission
	end

	loaded_data = nil
end

local serialize = function()
	return { ads = ads, missions = missions }
end

local unserialize = function(data)
	loaded_data = data
end

EventQueue.onGameStart:Connect(onGameStart)
EventQueue.onCreateBB:Connect(onCreateBB)
EventQueue.onUpdateBB:Connect(onUpdateBB)
EventQueue.onShipDocked:Connect(onShipDocked)
EventQueue.onJettison:Connect(onDeploy)

Serializer:Register("DeployProbe", serialize, unserialize)

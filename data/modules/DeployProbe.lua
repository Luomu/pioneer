-- Deploy Probe mission:
-- Get a 'Probe' item from employer, land on a surface of an (unexplored)
-- planet, dump probe there, return.
-- This is not necessarily very thrilling but requires:
-- way to check landing on a planet
-- a quest specific item
-- a way to use the item
-- reward bonus is based on distance from client and planet
-- characteristics (which cannot be accessed yet) and danger
-- from environmental hazards (which do not exist yet)
local max_dist = 10
local base_reward = 500 --how much is enough?
local distance_bonus = 100 --per LY
local probe_type = 'MINING_MACHINERY' --the probest there is now

local missions = {}
local ads = {}

function GiveProbe()
	Game.player:AddEquip(probe_type, 1)
end

local onChat = function(form, ref, option)
	local ad = ads[ref]

	form:Clear()

	if option == -1 then
		form:Close()
		return
	end

	if option == 0 then --greeting
		form:SetFace({ female = ad.isfemale, seed = ad.faceseed, name = ad.client, title = "Scientist" })
		form:SetMessage("Hello. We want you to drop our fancy probe on a planet of your choosing. You need one ton of free cargo space.")
	elseif option == 1 then --accept
		form:RemoveAdvertOnClose()

		ads[ref] = nil

		local mission = {
			type     = "Probing",
			client   = ad.client,
			location = ad.location,
			reward   = ad.reward,
			due      = ad.due,
			flavour  = ad.flavour,
			status   = 'ACTIVE',
			bonus    = 0,
		}

		local mref = Game.player:AddMission(mission)
		missions[mref] = mission

		form:SetMessage("Good. Return here with the probe data for your reward. You have until " .. Format.Date(ad.due))
		GiveProbe()
		form:AddOption("Hang up.", -1)

		return
	elseif option == 2 then -- more info
		form:SetMessage("Beats me.")
	end

	local stats = Game.player:GetStats()
	if stats.freeCapacity > 0 then
		form:AddOption("I'll do it.", 1)
		form:AddOption("What are good planets?", 2)
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
	local location = path
	local flavour = "WANTED. Someone to probe interesting balls of rock. Big rewards."
	local isfemale = Engine.rand:Integer(1) == 1
	local client = NameGen.FullName(isfemale)
	local due = Game.time + 15*24*60*60 --plenty of time
	local reward = 0 --rewards calculated from results

	local ad = {
		station  = station,
		flavour  = flavour,
		client   = client,
		location = station.path,
		due      = due,
		reward   = reward,
		isfemale = isfemale,
		faceseed = Engine.rand:Integer(), --would rather get some scientistic faces and backgrounds
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
	--should have a chat with employer to determine reward
	--check for success, or failure
	--failure conditions:
	--time limit exceeded
	--probe misplaced: can't do this right now since it's mining machinery
	for ref,mission in pairs(missions) do
		if mission.location == station.path then
			if mission.status == 'COMPLETED' then
				UI.ImportantMessage("That was excellent probing.", mission.client)
				Game.player:AddMoney(base_reward + mission.bonus)
				removeMission(ref)
			elseif mission.status == 'FAILED' or Game.time > mission.due then
				UI.ImportantMessage("You failure. No money for you.", mission.client)
				removeMission(ref)
			end
		end
	end
end

-- Jettisoning on surface = deploy
local onDeploy = function(cargo)
	if cargo.type ~= probe_type then return end
	--reward for unexplored systems only
	--if body.path:GetStarSystem().population > 0 then return end
	for ref,mission in pairs(missions) do
		if mission.status == 'ACTIVE' then
			UI.Message("Geological probe deployed.")
			mission.status = 'COMPLETED'
			mission.bonus = mission.bonus + distance_bonus * body.path:DistanceTo(mission.location)
			Game.player:UpdateMission(ref, mission)
			return
		end
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
EventQueue.onCargoUnload:Connect(onDeploy)

Serializer:Register("DeployProbe", serialize, unserialize)

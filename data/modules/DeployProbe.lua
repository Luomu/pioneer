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
		form:SetMessage("Hello. There.")
	elseif option == 1 then --accept
		form:RemoveAdvertOnClose()

		ads[ref] = nil

		local mission = {
			type     = "Probing",
			client   = ad.client,
			location = ad.location,
			reward   = ad.reward,
			due      = ad.due,
			flavour  = ad.flavour
		}

		local mref = Game.player:AddMission(mission)
		missions[mref] = mission

		form:SetMessage("Good. Get to it. Must be done by " .. Format.Date(ad.due))
		form:AddOption("Hang up.", -1)

		return
	end

	form:AddOption("I'll do it.", 1)
	form:AddOption("Hang up.", -1)
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
	local due = Game.time + 50
	local reward = 5000

	local ad = {
		station  = station,
		flavour  = flavour,
		client   = client,
		location = location,
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

local onShipDocked = function(ship, station)
	if not ship:isPlayer() then return end
	print("Docked")
	--check for success, or failure
	--failure conditions:
	--time limit exceeded
	--probe misplaced
end

-- Jettisoning on surface = deploy
local onDeploy = function(ship, cargo)
	if not ship:IsPlayer() then return end
	if cargo.type ~= 'HYDROGEN' then return end
	print(ship.label .. " dumped some " .. cargo.type)
end

local serialize = function()
	--save missions status
end

local unserialize = function(data)
	--load mission status
end

EventQueue.onCreateBB:Connect(onCreateBB)
EventQueue.onUpdateBB:Connect(onUpdateBB)
EventQueue.onShipDocked:Connect(onShipDocked)
EventQueue.onJettison:Connect(onDeploy)

Serializer:Register("DeployProbe", serialize, unserialize)

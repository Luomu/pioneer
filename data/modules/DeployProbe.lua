-- Deploy Probe mission:
-- Get a 'Probe' item from employer, land on a surface of an (unexplored)
-- planet, dump probe there, return.
-- This is not necessarily very thrilling but requires:
-- way to check landing on a planet
-- a quest specific item
-- a way to use the item

local mission = nil;

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

EventQueue.onShipDocked:Connect(onShipDocked)
EventQueue.onJettison:Connect(onDeploy)

Serializer:Register("DeployProbe", serialize, unserialize)

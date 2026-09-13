local Lighting = game:GetService("Lighting")
local RunService = game:GetService("RunService")
local Workspace = game:GetService("Workspace")

local FLOWERS = {
	Color3.new(0, 1, 1),
	Color3.new(0, 1, 0),
	Color3.new(1, 0.9, 0),
	Color3.new(1, 0, 1),
	Color3.new(1, 0.4, 0),
	Color3.new(0, 0, 1),
}

local TOTAL_FLOWERS = #FLOWERS
local RADIUS = 10
local AMPLITUDE = 0.5

local flowerParts = {}

local baseplate = Instance.new("Part")
baseplate.Name = "Baseplate"
baseplate.Anchored = true
baseplate.Color = Color3.fromHSV(0, 0, 0.5)
baseplate.Position = Vector3.new(0, -20, 0)
baseplate.Size = Vector3.new(50, 20, 50)
baseplate.Transparency = 0
baseplate.Parent = Workspace

for index, flowerColor in FLOWERS do
	local flowerSoul = Instance.new("Part")
	flowerSoul.Anchored = true
	flowerSoul.Color = flowerColor
	flowerSoul.CastShadow = true
	flowerSoul.Size = Vector3.new(2, 2, 2)
	flowerSoul.Parent = Workspace
	flowerParts[index] = flowerSoul
end

RunService.PreRender:Connect(function()
	for index, flowerSoul in flowerParts do
		local step = index / TOTAL_FLOWERS
		local angle = step * math.tau
		local x = math.cos(angle + os.clock()) * RADIUS
		local z = math.sin(angle + os.clock()) * RADIUS
		local y = math.sin(os.clock() + step) * AMPLITUDE
		flowerSoul.Position = Vector3.new(x, y, z)
	end
end)

local sky = Instance.new("Sky")
sky.Parent = Lighting

Lighting.ClockTime = 14
Lighting.Ambient = Color3.new(0, 0, 0)

local Lighting = game:GetService("Lighting")
local Workspace = game:GetService("Workspace")

local baseplate = Instance.new("Part")
baseplate.Anchored = true
baseplate.Color = Color3.fromHSV(0, 0, 0.5)
baseplate.Position = Vector3.new(0, -8, 0)
baseplate.Size = Vector3.new(50, 16, 50)
baseplate.Parent = Workspace

local sky = Instance.new("Sky")
sky.Parent = Lighting

const c, t, p = require("@scripts/classes").unpack()

return c({
	name = "Lighting",
	description = "Controls global lighting. It includes a range of adjustable properties that you can use to change how lighting appears and interacts with other objects.",
	since = "0.1.0",

	properties = {
		Ambient = {
			description = "The lighting hue applied to areas that are occluded from the sky, such as indoor areas.",
			type = t.Color3,
			unmodified = "gargantuan::Color3(0.0f, 0.0f, 0.0f)",
		},
		ClockTime = {
			description = "A numerical representation (in hours) of the current time of day.",
			type = t.Float,
			unmodified = "14",
		},
		GeographicLatitude = {
			description = "The geographic latitude, in degrees, of the scene, influencing the result of Lighting time on the position of the sun and moon.",
			type = t.Float,
			unmodified = "0",
		},
	},

	methods = {
		GetSunDirection = {
			description = "Returns a vector representing the direction of the sun.",
			returns = t.Vector3,
		},
		GetMoonDirection = {
			description = "Returns a vector representing the direction of the moon.",
			returns = t.Vector3,
		},
	},
})

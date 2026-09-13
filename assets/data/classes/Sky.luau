const c, t, p = require("@scripts/classes").unpack()

return c({
	name = "Sky",
	description = "Changes the default appearance of the sky.",
	since = "0.1.0",

	properties = {
		SkyboxBk = {
			description = "The URL link to a picture for the back surface of the sky.",
			type = t.String,
			unmodified = '"project://textures/sky/null_plainsky512_rt.jpg"',
			serializable = true,
		},
		SkyboxDn = {
			description = "The URL link to a picture for the bottom surface of the sky.",
			type = t.String,
			unmodified = '"project://textures/sky/null_plainsky512_dn.jpg"',
			serializable = true,
		},
		SkyboxFt = {
			description = "The URL link to a picture for the front surface of the sky.",
			type = t.String,
			unmodified = '"project://textures/sky/null_plainsky512_lf.jpg"',
			serializable = true,
		},
		SkyboxLf = {
			description = "The URL link to a picture for the left surface of the sky.",
			type = t.String,
			unmodified = '"project://textures/sky/null_plainsky512_ft.jpg"',
			serializable = true,
		},
		SkyboxRt = {
			description = "The URL link to a picture for the right surface of the sky.",
			type = t.String,
			unmodified = '"project://textures/sky/null_plainsky512_bk.jpg"',
			serializable = true,
		},
		SkyboxUp = {
			description = "The URL link to a picture for the top surface of the sky.",
			type = t.String,
			unmodified = '"project://textures/sky/null_plainsky512_up.jpg"',
			serializable = true,
		},
	},
})

-- Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
-- Licensed under the terms of CC-BY-SA 3.0. See licenses/CC-BY-SA-3.0.txt

define_model('missile', {
	info = {
		bounding_radius = 4,
		materials={ 'body' },
		tags = {'missile'},
	},
	static = function(lod)
		set_material('body', 1,1,1,1)
		use_material('body')
		cylinder(8, v(0,0,1), v(0,0,-3), v(0,1,0), .25)
		thruster(v(0,0,1), v(0,0,1), 10, true)
	end
})


--used by talon
define_model('nosewheel', {
	info = {
		lod_pixels={5,15,30,0},
		bounding_radius = 7,
		materials={'leg','tyre'}
	},
	static = function(lod)
		set_material('leg', .5, .5, .5, 1, .5, .5, .5, 2.0, 0, 0, 0)
		set_material('tyre', .3, .3, .3, 1, 0,0,0, 1, 0, 0, 0)
		use_material('leg')
		local v6 = v(0, 0, 0)
		local v7 = v(0, 3, 0)
		local v8 = v(0, 5, 0)
		local divs = lod*4
		cylinder(divs, v6, v8, v(0,0,1), .4)
		cylinder(divs, v7, v8, v(0,0,1), .5)
		use_material('tyre')
		xref_cylinder(divs, v(.5,5,0), v(1,5,0), v(0,0,1), 1.0)
	end
})

--used by cobra1, cobra3
define_model('nosewheelunit', {
	info = {
		bounding_radius = 9,
		materials={'inside', 'matvar2'}
	},
	static = function(lod)
		set_material('inside', .2,.2,.2,1, 0,0,0, 1, 0,0,0)
	end,
	dynamic = function(lod)
		-- flaps
		local v6 = v(1.5, 0, 6)
		local v7 = v(1.5, 0, -1)
		local v8 = v(0, 0, 6)
		local v9 = v(0, 0, -1)
		set_material('matvar2', get_arg_material(2))

		use_material('inside')
		zbias(1, v(0,0,0), v(0,1,0))
		-- flap internal
		xref_quad(v8, v6, v7, v9)
		-- SHould use parameter material(2) here but param materials not done yet
		use_material('matvar2')
		local flap_ang = 0.5*math.pi*math.clamp(3*get_animation_position('WHEEL_STATE'),0,1)
		local wheel_ang = 0.5*math.pi*math.clamp(1.5*(get_animation_position('WHEEL_STATE')-0.34), 0, 1)
		local vrot = 1.5*v(-math.cos(flap_ang), math.sin(flap_ang), 0)
		xref_quad(v7, v6, v6+vrot, v7+vrot)
		xref_quad(v7, v7+vrot, v6+vrot, v6)

		call_model('nosewheel', v(0,0,0), v(1,0,0),
		v(0,math.sin(wheel_ang),math.cos(wheel_ang)), 1.0)
		zbias(0)
	end
})

--used by talon
define_model('mainwheel', {
	info = {
		lod_pixels = {5,15,30,0},
		bounding_radius = 8,
		materials = {'leg', 'tyre'}
	},
	static = function(lod)
		local v6 = v(0,0,0)
		local v7 = v(0,3,0)
		local v8 = v(0,5,0)
		-- crossbar
		local v13 = v(0, 5, 1.4)
		local v14 = v(0, 5, -1.4)
		local divs = 4*lod
		set_material('leg', .5,.5,.5,1, 1,1,1, 2, 0,0,0)
		use_material('leg')
		cylinder(divs, v6, v8, v(0,0,1), .4)
		cylinder(divs, v7, v8, v(0,0,1), .5)
		cylinder(4, v13, v14, v(1,0,0), .5)
		set_material('tyre', .3,.3,.3,1, 0,0,0, 1, 0,0,0)
		use_material('tyre')
		xref_cylinder(divs, v(.5, 5, 1.1), v(1, 5, 1.1), v(0,0,1), 1)
		xref_cylinder(divs, v(.5, 5, -1.1), v(1, 5, -1.1), v(0,0,1), 1)
	end
})


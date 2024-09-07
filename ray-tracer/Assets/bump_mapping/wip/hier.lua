-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.reflective_material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25, 0.01)
grass = gr.reflective_material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0)
blue = gr.reflective_material({0.0471, 0.5294, 0.8}, {0.5, 0.4, 0.8}, 25, 0.3)

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(6, -2, -15)

s = gr.sphere('s')
s:scale(2, 2, 2)
s:translate(-4, 5, 0)
s:set_material(gold)

scene:add_child(s)

-- the floor
plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(blue)
plane:scale(30, 30, 30)
--plane:add_texture('water_texture.png')
-- add bumps at a u, v coordinate (ones that correspond with the texture)
-- then we have an extra term that will determine how many periods we want this to run for
-- should be a uint
-- finally should be the period of the sine wave
plane:mesh_add_bumps({0.5, 0.5}, 2, 0.1)

cube = gr.mesh( 'cube', 'cube.obj' )
scene:add_child(cube)
cube:set_material(gold)
cube:translate(-2, 3, 0)
cube:add_texture('water_texture.png')
cube:mesh_add_bumps({0.5, 0.5}, 50, 0.1)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'hier.png', 512, 512, 
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})

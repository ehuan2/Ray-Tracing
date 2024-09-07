gold = gr.reflective_material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25, 0.01)
grass = gr.reflective_material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0)
blue = gr.reflective_material({0.0471, 0.5294, 0.8}, {0.8, 0.8, 0.8}, 5, 1.0)

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(2, -2, -15)

s = gr.sphere('s')
s:scale(2, 2, 2)
s:translate(0, 2, 0)
s:set_material(gold)
scene:add_child(s)

-- the floor
plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(blue)
plane:scale(30, 30, 30)
plane:mesh_add_bumps({0.5, 0.5}, 2, 0.5)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'clear_waves.png', 512, 512, 
	  {0, 0, 15}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})

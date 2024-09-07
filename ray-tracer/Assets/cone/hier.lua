-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.reflective_material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25, 0.01)
grass = gr.reflective_material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0)
blue = gr.reflective_material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25, 0.3)

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(6, -2, -15)

s = gr.sphere('s')
s:scale(2, 2, 2)
s:translate(-4, 5, 0)
s:set_material(gold)

--scene:add_child(s)

-- the floor
plane = gr.mesh( 'plane', 'plane.obj' )
--scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)

-- now we add the cone
cone = gr.cone('cone')
scene:add_child(cone)
cone:set_material(gold)
cone:translate(-2, 4, 0)
cone:rotate('Y', -30)

cone_two = gr.cone('cone_two')
scene:add_child(cone_two)
cone_two:set_material(gold)
cone_two:rotate('X', -90)
cone_two:translate(-2, 6, 0)

sphere = gr.sphere('sphere')
sphere:set_material(gold)
scene:add_child(sphere)
sphere:translate(-5, 4, 0)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'hier.png', 512, 512, 
	  {0, 0, 10}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})

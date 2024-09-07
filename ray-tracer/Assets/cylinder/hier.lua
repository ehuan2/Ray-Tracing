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
scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)

-- now we add the cylinder
cylinder = gr.cylinder('cylinder')
scene:add_child(cylinder)
cylinder:set_material(gold)
cylinder:scale(2, 2, 2)
cylinder:translate(-2, 4, 0)
cylinder:add_texture('mountain.png')
cylinder:rotate('Y', -30)

cyl = gr.cylinder('cyl')
scene:add_child(cyl)
cyl:set_material(gold)
cyl:rotate('X', 90)
cyl:translate(-5, 4, 0)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'hier.png', 512, 512, 
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})

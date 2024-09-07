gold = gr.reflective_material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25, 0.01)
grass = gr.reflective_material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0)
blue = gr.reflective_material({0.0471, 0.5294, 0.8}, {0.8, 0.8, 0.8}, 5, 1.0)

scene = gr.node('scene')

s = gr.sphere('s')
s:scale(2, 2, 2)
s:translate(0, 2, 0)
s:set_material(gold)
scene:add_child(s)

cyl = gr.cylinder('cyl')
cyl:scale(2, 2, 2)
cyl:translate(5, 2, 0)
cyl:set_material(gold)
scene:add_child(cyl)

cone = gr.cone('cone')
cone:scale(2, 2, 2)
cone:translate(-5, 2, 0)
cone:set_material(gold)
scene:add_child(cone)




-- adding the water
water = gr.mesh( 'water', 'cube.obj' )
water:set_material(blue)
water:scale(50, 0.5, 70)
water:translate(-10, -3, -10)
water:mesh_add_bumps({0.85, 0.25}, 5000, 0.01)
scene:add_child(water)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'reflection.png', 512, 512, 
	  {0, 0, 50}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})

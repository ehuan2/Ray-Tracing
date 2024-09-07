gold = gr.reflective_material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25, 0.0)
grass = gr.reflective_material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0.0)
blue = gr.reflective_material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25, 1.0)

scene = gr.node('scene')

-- the floor
plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)
plane:translate(0, -5, 0)

-- add in reflecting spheres
painting = gr.mesh( 'painting', 'plane.obj' )
scene:add_child(painting)
painting:set_material(gold)
painting:scale(50, 50, 50)
painting:rotate('X', 90)
painting:translate(0, 4, 31)
painting:add_texture("sample.png")

-- now we add spheres
gold_1 = gr.reflective_material({0.8588, 0.6745, 0.2039}, {0.5, 0.7, 0.5}, 25, 0.0)
gold_2 = gr.reflective_material({0.8588, 0.6745, 0.2039}, {0.5, 0.7, 0.5}, 25, 0.25)
gold_3 = gr.reflective_material({0.8588, 0.6745, 0.2039}, {0.5, 0.7, 0.5}, 25, 0.5)
gold_4 = gr.reflective_material({0.8588, 0.6745, 0.2039}, {0.5, 0.7, 0.5}, 25, 0.75)
gold_5 = gr.reflective_material({0.8588, 0.6745, 0.2039}, {0.5, 0.7, 0.5}, 25, 1.0)

scale_factor = 1

s1 = gr.nh_sphere('s1', {-10, 0, 0}, 2)
s1:set_material(gold_1)
scene:add_child(s1)

s2 = gr.nh_sphere('s2', {-5, 0, 0}, 2)
s2:set_material(gold_2)
scene:add_child(s2)

s3 = gr.nh_sphere('s3', {0, 0, 0}, 2)
s3:set_material(gold_3)
scene:add_child(s3)

s4 = gr.nh_sphere('s4', {5, 0, 0}, 2)
s4:set_material(gold_4)
scene:add_child(s4)

s5 = gr.nh_sphere('s5', {10, 0, 0}, 2)
s5:set_material(gold_5)
scene:add_child(s5)

-- The lights
l1 = gr.light({200,200,20}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 55, 10}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'reflection.png', 1024, 1024, 
	  {0, 0, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})

-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.reflective_material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0)

scene = gr.node('scene')
scene:rotate('X', 23)

-- add in reflecting spheres
painting = gr.mesh( 'painting', 'plane.obj' )
scene:add_child(painting)
painting:set_material(gold)
painting:scale(10, 10, 10)
painting:rotate('X', 90)
painting:translate(-15, 0, 0)
painting:add_texture("sample.png")

painting_2 = gr.mesh( 'painting_2', 'plane.obj' )
scene:add_child(painting_2)
painting_2:set_material(gold)
painting_2:scale(10, 10, 10)
painting_2:rotate('X', 90)
painting_2:translate(15, 0, 0)
painting_2:add_texture("big_final.png")

-- now we add the cylinder
cylinder = gr.cylinder('cylinder')
scene:add_child(cylinder)
cylinder:set_material(gold)
cylinder:scale(5, 5, 5)
cylinder:translate(0, 10, 0)
cylinder:add_texture('mountain.png')
cylinder:rotate('Y', -30)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'art_gallery.png', 512, 512, 
	  {0, 0, 70}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})


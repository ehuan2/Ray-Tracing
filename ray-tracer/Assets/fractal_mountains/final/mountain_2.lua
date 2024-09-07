gold = gr.reflective_material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25, 0)
grass = gr.reflective_material({0.1, 0.7, 0.1}, {0.2, 0.2, 0.2}, 25, 0)
blue = gr.reflective_material({0.0471, 0.5294, 0.8}, {0.5, 0.4, 0.8}, 25, 0.3)

scene = gr.node('scene')
scene:rotate('X', -23)
scene:translate(6, -2, -15)

-- provide {w, h, l}, roughness and number of times to subdivide
mountain = gr.fractal_mountain( 'fuji', {30.0, 10.0, 10.0}, 0.1, 2 )
scene:add_child(mountain)
mountain:set_material(grass)
mountain:scale(5, 7, 3)
mountain:translate(-80, -3, -225)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})
l3 = gr.light({-70, 10, 20}, {0.8, 0.8, 0.8}, {1, 0, 0})

gr.render(scene, 'mountain_2.png', 512, 512,
	  {0, 0, 10}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1})

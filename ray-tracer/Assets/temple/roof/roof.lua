-- A simple scene with five spheres
gold = gr.reflective_material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25, 0.01)

scene = gr.node('root')
scene:rotate('X', 23)

lower_roof = gr.mesh('lower_roof', 'roof.obj')
lower_roof:set_material(gold)

higher_roof = gr.mesh('higher_roof', 'roof.obj')
higher_roof:set_material(gold)
higher_roof:add_texture('roof_texture.png')
higher_roof:translate(0, 0.2, 0)

roof = gr.node('roof')
roof:add_child(lower_roof)
roof:add_child(higher_roof)

scene:add_child(roof)

white_light = gr.light({100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene, 'roof.png', 512, 512,
	  {0, 0, 10}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})

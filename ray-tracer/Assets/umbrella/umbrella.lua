-- A simple scene with five spheres
gold = gr.reflective_material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25, 0.01)

scene = gr.node('root')
scene:rotate('X', 23)

-- top of umbrella is one sphere minus another
s1 = gr.sphere('s1')
s1:translate(0, 0.2, 0)
s1:set_material(gold)
s2 = gr.sphere('s2')
s2:set_material(gold)

top = gr.diff('top', s1, s2)

cube = gr.cube('cube')
cube:scale(2, 1, 2)
cube:translate(-1, 0, -1)
s3 = gr.sphere('s3')
s3:scale(1, 1, 0.2)
s3:set_material(gold)
semi = gr.diff('semi', s3, cube)

small = gr.sphere('small')
small:scale(0.5, 0.5, 4)
small:set_material(gold)
handle = gr.diff('handle', semi, small)

arm = gr.cylinder('arm')
arm:scale(0.3, 6, 0.1)
arm:scale(0.5, 0.5, 0.5)
arm:translate(-0.7, 3, 0)
arm:set_material(gold)
support = gr.union('support', handle, arm)

umbrella = gr.node('umbrella')
top:scale(4, 2, 4)
top:translate(-0.5, 6, 0)
umbrella:add_child(top)
umbrella:add_child(support)
--umbrella:rotate('X', -90)

scene:add_child(umbrella)

white_light = gr.light({100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene, 'umbrella.png', 512, 512,
	  {0, 0, 20}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})

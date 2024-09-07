-- A simple scene with five spheres

mat1 = gr.reflective_material({0.7, 0.7, 0.7}, {0.5, 0.7, 0.5}, 25, 0.0)
mat2 = gr.reflective_material({0.8, 0.3, 0.8}, {0.5, 0.7, 0.5}, 25, 0.0)
mat3 = gr.reflective_material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.0)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {0, 0, 0}, 1)
s1:set_material(mat1)

s2 = gr.sphere('s2')
s2:translate(1, 0, 0)
s2:set_material(mat2)

diff = gr.diff('donut', s1, s2)

s3 = gr.sphere('s3')
s3:translate(0, 1, 0)
s3:set_material(mat3)

diff_two = gr.diff('diff_two', s3, diff)
diff_two:rotate('X', -90)
scene_root:add_child(diff_two)

extra = gr.node('extra')
extra:translate(0, -1, 0)
extra:add_child(diff)
--scene_root:add_child(extra)

white_light = gr.light({100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'diff_one.png', 512, 512,
	  {0, 0, 20}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})

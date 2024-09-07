-- A simple scene with five spheres

mat1 = gr.reflective_material({0.7, 0.7, 0.7}, {0.5, 0.7, 0.5}, 25, 0.0)
mat2 = gr.reflective_material({0.8, 0.3, 0.8}, {0.5, 0.7, 0.5}, 25, 0.0)
mat3 = gr.reflective_material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.0)

scene = gr.node('root')

s1 = gr.nh_sphere('s1', {0, 0, 0}, 1)
s1:set_material(mat1)

s2 = gr.nh_sphere('s2', {0.5, 0, 0}, 1)
s2:set_material(mat2)

s3 = gr.nh_box('s3', {0, 0, 0}, 1)
s3:rotate('Z', 45)
s3:rotate('Y', 45)
s3:translate(-1.0, 0, 0)
s3:set_material(mat3)

a_minus_b = gr.diff('a_minus_b', s1, s2)
a_minus_b:translate(-6, 0, 0)
b_minus_a = gr.diff('b_minus_a', s2, s1)
b_minus_a:translate(-2, 0, 0)

a_int_b = gr.intersect('a_int_b', s1, s2)
a_int_b:translate(2, 0, 0)
a_union_b = gr.union('a_union_b', s1, s2)
a_union_b:translate(6, 0, 0)

scene:add_child(a_minus_b)
scene:add_child(b_minus_a)
scene:add_child(a_int_b)
scene:add_child(a_union_b)

white_light = gr.light({100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene, 'simple-csg.png', 512, 512,
	  {0, 0, 25}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})

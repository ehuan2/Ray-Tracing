-- A simple scene with five spheres

mat1 = gr.reflective_material({0.7, 0.7, 0.7}, {0.5, 0.7, 0.5}, 25, 0.0)
mat2 = gr.reflective_material({0.8, 0.3, 0.8}, {0.5, 0.7, 0.5}, 25, 0.0)
mat3 = gr.reflective_material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.0)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {0, 0, 0}, 1)
s1:set_material(mat1)

s2 = gr.nh_sphere('s2', {-0.5, 0, 0}, 1)
s2:translate(0, 0, 0)
s2:set_material(mat2)

union = gr.union('union', s1, s2)

intersect = gr.intersect('intersect', s1, s2)
intersect:translate(0, 0.5, 0)

intersect_big = gr.intersect('intersect_big', union, intersect)
intersect_big:rotate('Y', 45)

intersect_more = gr.intersect('intersect_more', intersect, intersect_big)
scene_root:add_child(intersect_more)

white_light = gr.light({100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'intersect.png', 512, 512,
	  {0, 0, 5}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})

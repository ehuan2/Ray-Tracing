-- A simple scene with five spheres

mat1 = gr.reflective_material({0.7, 0.7, 0.7}, {0.5, 0.7, 0.5}, 25, 0.0)
mat2 = gr.reflective_material({0.8, 0.3, 0.8}, {0.5, 0.7, 0.5}, 25, 0.0)
mat3 = gr.reflective_material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.0)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {0, 0, 0}, 1)
--s1:scale(40, 40, 40)
--s1:translate(0, 0, -100)
s1:set_material(mat1)

s2 = gr.nh_box('s2', {0, 0, 0}, 1)
--s2:scale(40, 40, 40)
s2:set_material(mat2)

s3 = gr.nh_box('s3', {0, 0, 0}, 1)
--s3:scale(40, 40, 40)
s3:rotate('Z', 45)
s3:rotate('Y', 45)
s3:translate(-1.0, 0, 0)
s3:set_material(mat3)

union = gr.union('union', s1, s2)
union:rotate('Y', 90)
--scene_root:add_child(s1)
--scene_root:add_child(s2)


big_union = gr.union('big_union', union, s3)
--scene_root:add_child(intersect)

s4 = gr.nh_box('s4', {-0.5, 0.5, 0}, 1)
s4:set_material(mat2)
big_intersect = gr.intersect('big_intersect', big_union, s4)
scene_root:add_child(big_intersect)

white_light = gr.light({100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'union.png', 512, 512,
	  {0, 0, 5}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})

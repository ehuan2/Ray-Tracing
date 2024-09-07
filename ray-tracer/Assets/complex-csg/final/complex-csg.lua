-- A simple scene with five spheres

mat1 = gr.reflective_material({0.7, 0.7, 0.7}, {0.5, 0.7, 0.5}, 25, 0.0)
mat2 = gr.reflective_material({0.8, 0.3, 0.8}, {0.5, 0.7, 0.5}, 25, 0.0)
mat3 = gr.reflective_material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.0)

scene = gr.node('root')

s1 = gr.nh_sphere('s1', {0, 0, 0}, 1)
s1:set_material(mat1)

show_s1 = gr.node('show_s1')
show_s1:add_child(s1)
show_s1:translate(5, 0, 0)
scene:add_child(show_s1)

s2 = gr.cylinder('s2')
s2:scale(0.5, 1.5, 0.5)
s2:rotate('Z', 45)
s2:set_material(mat2)
scene:add_child(s2)

s3 = gr.cone('s3', {0, 0, 0}, 2)
s3:set_material(mat3)

show_s3 = gr.node('show_s3')
show_s3:add_child(s3)
show_s3:translate(-5, 0, 0)
scene:add_child(show_s3)

union_1 = gr.union('union_1', s1, s3)
int_1 = gr.intersect('int_1', s1, s3)
diff_1 = gr.diff('diff_1', s1, s3)

show_int_1 = gr.node('show_int_1')
show_int_1:add_child(int_1)
show_int_1:translate(-5, 6, 0)

show_union_1 = gr.node('show_union_1')
show_union_1:add_child(union_1)
show_union_1:translate(0, 6, 0)

show_diff_1 = gr.node('show_diff_1')
show_diff_1:add_child(diff_1)
show_diff_1:translate(5, 6, 0)

union_2 = gr.union('union_2', s2, union_1)
int_2 = gr.union('int_2', s2, int_1)
diff_2 = gr.union('diff_2', s2, diff_1)

show_int_2 = gr.node('show_int_2')
show_int_2:add_child(int_2)
show_int_2:translate(-5, 3, 0)

show_union_2 = gr.node('show_union_2')
show_union_2:add_child(union_2)
show_union_2:translate(0, 3, 0)

show_diff_2 = gr.node('show_diff_2')
show_diff_2:add_child(diff_2)
show_diff_2:translate(5, 3, 0)

union_3 = gr.diff('union_3', union_1, s2)
int_3 = gr.diff('int_3', int_1, s2)
diff_3 = gr.diff('diff_3', diff_1, s2)

show_int_3 = gr.node('show_int_3')
show_int_3:add_child(int_3)
show_int_3:translate(-5, -3, 0)

show_union_3 = gr.node('show_union_3')
show_union_3:add_child(union_3)
show_union_3:translate(0, -3, 0)

show_diff_3 = gr.node('show_diff_3')
show_diff_3:add_child(diff_3)
show_diff_3:translate(5, -3, 0)

union_4 = gr.intersect('union_4', s2, union_1)
int_4 = gr.intersect('int_4', s2, int_1)
diff_4 = gr.intersect('diff_4', s2, diff_1)

show_int_4 = gr.node('show_int_4')
show_int_4:add_child(int_4)
show_int_4:translate(-5, -6, 0)

show_union_4 = gr.node('show_union_4')
show_union_4:add_child(union_4)
show_union_4:translate(0, -6, 0)

show_diff_4 = gr.node('show_diff_4')
show_diff_4:add_child(diff_4)
show_diff_4:translate(5, -6, 0)

scene:add_child(show_union_1)
scene:add_child(show_int_1)
scene:add_child(show_diff_1)

scene:add_child(show_union_2)
scene:add_child(show_int_2)
scene:add_child(show_diff_2)

scene:add_child(show_union_3)
scene:add_child(show_int_3)
scene:add_child(show_diff_3)

scene:add_child(show_union_4)
scene:add_child(show_int_4)
scene:add_child(show_diff_4)

white_light = gr.light({100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene, 'complex-csg.png', 512, 512,
	  {0, 0, 20}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})

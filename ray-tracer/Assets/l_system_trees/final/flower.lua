-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.reflective_material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25, 0.01)
grass = gr.reflective_material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0)
blue = gr.reflective_material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25, 0.3)

scene = gr.node('scene')

trunk = gr.cylinder('trunk')
trunk:set_material(gold)
leaf = gr.mesh('leaf', 'leaf.obj')
leaf:rotate('X', 90)
leaf:set_material(grass)
leaf:scale(0.3, 0.3, 0.3)

flower = gr.tree('flower', 'flower.grammar', 6, trunk, leaf)
flower:scale(0.5, 0.5, 0.5)
flower:translate(0, -5, 0)

scene:add_child(flower)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'flower.png', 512, 512, 
	  {0, 0, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})

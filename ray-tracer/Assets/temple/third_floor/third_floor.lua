-- first floor

gold = gr.reflective_material({0.8588, 0.6745, 0.2039}, {0.8, 0.8, 0.8}, 5, 0.01)
grass = gr.reflective_material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0)
blue = gr.reflective_material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25, 0.3)
white = gr.reflective_material({0.8, 0.8, 0.8}, {0.8, 0.8, 0.4}, 25, 0)
wood_brown = gr.material({0.4, 0.2, 0.0}, {0.5, 0.4, 0.8}, 25)

scene = gr.node('scene')
--scene:rotate('X', 23)

-- third floor:
-- a node representing the center of the first floor
third_floor = gr.node('third_floor')
scene:add_child(third_floor)

do
        -- bottom part of temple
        local bottom_floor = gr.cube('bottom_floor')
        bottom_floor:scale(10, 0.2, 12)
        bottom_floor:set_material(gold)
        bottom_floor:translate(-5, -2.2, -6)
        third_floor:add_child(bottom_floor)

-- first floor 3 x 1 panelling
        local plane = gr.mesh( 'plane', 'plane.obj' )
        plane:set_material(gold)
        plane:scale(1, 1, 1.5)
        plane:rotate('X', 90)
        plane:translate(0, -0.5, 0)

        -- wood section
        local wood = gr.mesh('wood', 'cube.obj')
        wood:set_material(gold)
        wood:scale(0.05, 0.05, 1)

        -- now we add the wood to 
        local wood_left = gr.node('wood_left')
        wood_left:add_child(wood)
        wood_left:translate(-1, 0.01, 0)
        plane:add_child(wood_left)

        -- now we add the wood to 
        local wood_right = gr.node('wood_right')
        wood_right:add_child(wood)
        wood_right:translate(1, 0.01, 0)
        plane:add_child(wood_right)

        -- now we add the wood to 
        local wood_up = gr.node('wood_up')
        wood_up:add_child(wood)
        wood_up:rotate('Y', 90)
        wood_up:translate(0, 0, 1)
        plane:add_child(wood_up)

        -- now we add the wood to 
        local wood_down = gr.node('wood_down')
        wood_down:add_child(wood)
        wood_down:rotate('Y', 90)
        wood_down:translate(0, 0, -1)
        plane:add_child(wood_down)

-- first floor 3 x 1 wood panelling
        local wood_plane = gr.mesh( 'wood_plane', 'plane.obj' )
        wood_plane:set_material(gold)
        wood_plane:scale(1, 1, 1.5)
        wood_plane:rotate('X', 90)
        wood_plane:translate(0, -0.5, 0)

        wood_plane:add_child(wood_left)
        wood_plane:add_child(wood_right)
        wood_plane:add_child(wood_up)
        wood_plane:add_child(wood_down)

-- now we define a small panel, a 1 x 1
        local small_panel = gr.mesh( 'small_panel', 'plane.obj' )
        small_panel:set_material(white)
        small_panel:scale(1, 1, 0.5)
        small_panel:rotate('X', 90)
        small_panel:translate(0, 1.5, 0)

        -- wood section
        -- now we add the wood to
        small_panel:add_child(wood_left)
        -- now we add the wood to 
        small_panel:add_child(wood_right)
        -- now we add the wood to 
        small_panel:add_child(wood_up)
        -- now we add the wood to 
        small_panel:add_child(wood_down)

        local wood_panel = gr.node('wood_panel')
        wood_panel:add_child(wood_plane)
        wood_panel:add_child(small_panel)

        local reg_panel = gr.node('reg_panel')
        reg_panel:add_child(plane)
        reg_panel:add_child(small_panel)

        local wood_wall = gr.node('wood_wall')

        -- front wall is taking three wood planes
        for i = 1, 4 do
            tmp_wood_panel = gr.node('tmp_wood_panel' .. tostring(j))
            tmp_wood_panel:translate((i - 1) * 2, 0, 0)
            tmp_wood_panel:add_child(wood_panel)
            wood_wall:add_child(tmp_wood_panel)
        end

        local side_wall = gr.node('side_wall')
        side_wall:add_child(wood_panel)
        for i = 2, 3 do
            tmp_reg_panel = gr.node('tmp_reg_panel' .. tostring(j))
            tmp_reg_panel:translate((i - 1) * 2, 0, 0)
            tmp_reg_panel:add_child(reg_panel)
            side_wall:add_child(tmp_reg_panel)
        end

        local right_wall = gr.node('right_wall')
        right_wall:add_child(side_wall)
        right_wall:rotate('Y', 90)
        right_wall:translate(4, 0, 2.5)

        local left_wall = gr.node('left_wall')
        left_wall:add_child(side_wall)
        left_wall:rotate('Y', 90)
        left_wall:translate(-4, 0, 2.5)

        local front_wall = gr.node('front_wall')
        front_wall:add_child(wood_wall)
        front_wall:translate(-3, 0, 3.5)

        local back_wall = gr.node('back_wall')
        back_wall:add_child(wood_wall)
        back_wall:translate(-3, 0, -2.5)

        third_floor:add_child(right_wall)
        third_floor:add_child(left_wall)
        third_floor:add_child(front_wall)
        third_floor:add_child(back_wall)
end

-- roof portion:
roof = gr.node('roof')
do
        local lower_roof = gr.mesh('lower_roof', 'roof.obj')
        lower_roof:set_material(gold)

        local higher_roof = gr.mesh('higher_roof', 'roof.obj')
        higher_roof:set_material(gold)
        higher_roof:add_texture('roof_texture.png')
        higher_roof:translate(0, 0.1, 0)

        roof:add_child(lower_roof)
        roof:add_child(higher_roof)
end

-- add to third floor
third_floor_roof = gr.node('third_floor_roof')
third_floor_roof:add_child(roof)
third_floor_roof:scale(3.5, 3, 3.5)
third_floor_roof:translate(0, 2, 0)
third_floor:add_child(third_floor_roof)

-- add the decoration to the roof
decoration = gr.mesh('decoration', 'cow.obj')
decoration:set_material(gold)
decoration:scale(0.2, 0.2, 0.2)
decoration:rotate('Y', -90)
decoration:translate(0, 1.5, 0)
third_floor_roof:add_child(decoration)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'third_floor.png', 512, 512, 
	  {0, 0, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})


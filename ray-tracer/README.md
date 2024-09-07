----------------------------------------------------------------------------------------------------
Compiling the program:

As usual, we just need to do ``premake4 gmake`` and then ``make``. However, there are a few flags
that I should mention are found under src/tools/directives.hpp which can be defined and undefined
for different behaviour. They are listed below:

1. USE_ANTI_ALIASING - default on

Turns on anti aliasing to run such that we have 9 samples per pixel.

2. RENDER_BOUNDING_VOLUMES - default off

Turns on bounding boxes on such that we can put a box over all meshes and show what the
bounding box would be.

3. MAX_RAY_RECURSE_DEPTH - default 4

This value is the max number of times that we will allow the ray to recurse when doing reflections.

3. SINGLE_PIXEL_DEBUG - default off

Turns on debugging for a single pixel, which can be helpful to isolate specific behaviours, and
checking that the algorithms are doing their job.

4. USE_THREADING - default on

This turns on multithreading where we split up the image into batches and let 16 threads keep
running and continue working until the end.

5. PRINT_PROGRESS - default on

This tells us how much progress we've made (%) that combines all the threads.

6. PRINT_TREE_GRAMMAR - default off

This flag when turned on will print the grammar of all the trees that are within the scene.
Useful debugging L-system trees.

----------------------------------------------------------------------------------------------------
Running the program:
As before, the program is ran just by calling the Ray-Tracer binary found, however there are some
changes to what the Lua commands are as well as a few extra files that are to be included
based on what we want to model.

1. gr.reflective_material(<kd>, <ks>, p, reflective-strength)

This generates a new PhongMaterial object which will have kd 3-tuple, ks 3-tuple and p as they were
previously (being diffuse reflection, specular reflection and Phong coefficients respectively)
with a new reflective strength mark which is a number between 0 and 1 which marks how much of the
reflection do they take. i.e. 1 for completing reflecting and 0 for not reflecting at all.
    
e.g.:
    blue = gr.reflective_material({0.0471, 0.5294, 0.8}, {0.8, 0.8, 0.8}, 5, 1.0)

2. gr.add_texture('<png path relative to execution>')

Using the library lodepng provided already within the code base, this function is able to map a png
provided (relative to the folder of execution) and add it to meshes, cylinders, and spheres.

e.g.:
    plane = gr.mesh( 'plane', 'plane.obj' )
    plane:set_material(grass)
    plane:scale(30, 30, 30)
    plane:add_texture("sample.png")

3. gr.fractal_mountain( <name>, {x-size, y-size, z-size}, roughness, subdivisions )

This is a new Lua command that generates a new class of Mesh called a FractalMountain.
The 3-tuple of {x-size, y-size, z-size} are the starting dimensions of the mountain, however do note
that due to the unbounded nature of the mountain's perturbation, that the y-size will probably
change.

The roughness is in fact a standard deviation, marking the standard deviation of the random process
used to generate the mountain. Subdivision is the total number of recursive subdivisions we
apply to the faces. The more there are, the more detailed it becomes.

Note: We might not want to texture map fractal mountains because faces tend to be stretched out
across the y. This leads to mountains that are quite streaky... Also, note that we do not
need to provide a material since the mountain itself will be default coloured depending on
the height.

e.g.:
    mountain = gr.fractal_mountain( 'fuji', {20.0, 2.0, 10.0}, 0.1, 3 )

4. gr.diff(<name>, A, B), gr.union(<name>, A, B), gr.intersect(<name>, A, B)

Given two Geometry Nodes A, B that are either:
1. Another CSG node (from diff, union or intersect)
2. A geometry node with a primitive (namely cube, sphere, cylinder and cone)
the above returns the resultant A - B, A U B and A intersect B constructive solid geometries.

The resultant is a geometry node, however we cannot:
1. Use material on it
2. Give it a texture

e.g.:
    s1 = gr.nh_sphere('s1', {0, 0, 0}, 1)
    s1:set_material(mat1)
    s2 = gr.sphere('s2')
    s2:scale(1, 1.5, 1)
    s2:translate(1.0, 0, 0)
    s2:set_material(mat2)
    diff = gr.diff('diff', s1, s2)

5. gr.tree(<name>, <grammar-file>, recursion-depth, <trunk geometry node>, <leaf geometry node>)

The grammar file (see .grammar file structure below) marks the Context-Free Grammar (CFG) and
its rules that we will expand from, which will generate a string that will describe an l-system
tree.

Recursion depth marks the number of times we expand on the grammar. The more you mark this,
the bigger the tree (given an expanding grammar) will become.

Trunk geometry node and leaf geometry node provide an interpretation for the grammar. These can
be any type of geometry node from meshes to primitives to CSGs. See .grammar file structure 
for more details.

e.g.:
    trunk = gr.cylinder('trunk')
    trunk:set_material(mat1)
    leaf = gr.mesh('leaf', 'leaf.obj')
    leaf:set_material(mat1)
    tree = gr.tree('tree', 'tree.grammar', 3, trunk, leaf)

6. <mesh-node>:mesh_add_bumps({u, v}, num-periods, period)

To add in normal bumping that occurs in a wave like fashion (i.e. repeats depending on distance).
The coordinates {u, v} mark the center of the waves, where num-periods mark the number of periods
we'll render and period marks how large the periods are. So, if we want bigger waves,
we'll increase period, and make it smaller for smaller ones. If we want to have the waves end
soon, we'll limit num-periods to 1 or 2.

e.g.:
    cube = gr.mesh('cube', 'cube.obj')
    cube:set_material(mat1)
    cube:mesh_add_bumps({0.5, 0.5}, 5, 0.1)

----------------------------------------------------------------------------------------------------
Generating the final scene

Simply go to Assets/final/ and run ../../Ray-Tracer temple.lua. The resulting image should be in temple.png.

----------------------------------------------------------------------------------------------------
.grammar file structure

For L-system trees, we need to be able to define a Context-Free Grammar (CFG) that models
the tree. The following describes how we can generate and describe these trees.
        1. <start-symbol>

        2. trunk <symbol> <x-scale> <y-scale> <z-scale>
        3. leaf <symbol> <x-scale> <y-scale> <z-scale>
        4. branch <left-bracket> <right-bracket> <x-rotate> <y-rotate> <z-rotate>

        5. <symbol> <probability> <list-of-symbols>

Here, we define a grammar such that 1. denotes the starting symbol upon which the grammar 
will expand from. Then 2. and 3. mark how we will interpret each of the symbols, either as 
a leaf or a trunk defined from the gr.tree command. These can be any arbitrary geometry node.
Then, the scaling tells us how to change the size of these nodes. This is useful for creating
trees that differ in size depending on where in the tree it is (e.g. trunks are smaller than
branches).

4. marks the different type of branches we can do. It marks what symbols will be read as
a left bracket and right bracket that marks which symbols belong to a branch. For example,
F ( F F ) marks that we will branch after an initial F, to have two F's conjoined. It also marks
how we rotate when we branch, described under x-rotate, y-rotate, z-rotate. Note that we rotate
around the axes in order, i.e. we do x-rotate amount, then y then z.

5. then marks the expansion rule we apply. This will tell us how we want to expand on the grammar
given a probability. For example,

(a) F 0.3 F F
(b) F 0.5 F ( F F ) F
(c) F 0.2 [ F F ] F

F has a 30% chance of expanding to F F, 50% to F ( F F ) F, and a 20% chance to expand
to [ F F ] F.

Note: Since we are using spaces as our delimeters, we have to write our rules as ( F F )
and not (FF) or (F F). This allows us to have strings as symbols, not just characters. And an
unlimited amount of brackets (can use iamleftbracket iamrightbracket as brackets).

Note that for 2-4 we can have as many of these as long as we don't have conflicting symbols used.
For 5., we can have as many rules as we want, though the probabilities need to add to 1.0 given
a symbol. If not, a hidden rule of disappearing is added.

----------------------------------------------------------------------------------------------------
External Models/Textures Credit
In Assets/final/
* cow.obj was taken from A4
* roof_texture.png was taken from https://architextures.org/textures/581
* lighter_wood.png was taken from
    https://www.onairdesign.com/products/dark-wood-texture-board-ww-d-oa-145
* cube.obj was taken from A4
* plane.obj was taken from A4
* leaf.obj and roof.obj were both generated from me in Blender
* The grammar I used in final.grammar was inspired from (i.e. the one without the leaves)
  pg. 25 of The Algorithmic Beauty of Plants by Prusinkiewicz and Lindenmayer

----------------------------------------------------------------------------------------------------
Good Scene Generation:
Under Assets/ we have a bunch of different pictures that showcase the different objectives
I fulfilled. To differentiate between my work in progress (wip) pictures and the final good ones,
each of them will have a wip/ folder and a final/ folder, where the images for our documentation
is found under final/.

----------------------------------------------------------------------------------------------------
Objective List:
1. Reflections, add to water
2. Texture Mapping, for temple
3. Multithreading
4. Fractal Mountains
5. Procedural Generation of Temple Modelling
6. Simple CSG
7. L-system plants
8. Normal Mapping for wave effects
9. Complicated CSG, model an umbrella
10. Final scene


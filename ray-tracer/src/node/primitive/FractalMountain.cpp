#include "FractalMountain.hpp"
#include <iostream>
#include <random>
#include <string>

using namespace std;

FractalMountain::FractalMountain(const glm::vec3 &dim, double roughness, uint depth):
    Mesh(), width(dim[0]), height(dim[1]), length(dim[2]),
    roughness (roughness), depth(depth)
{
    // now on initialization, we will go through and do the recursive
    // fractalization of the mountain
    init();
}


// this helper function adds the bottom of the mountain
void add_bottom(std::vector<Triangle> &faces) {
    faces.push_back(Triangle(0, 2, 1, 0, 2, 1));
    faces.push_back(Triangle(0, 3, 2, 0, 2, 1));
}

// this initializes the mountain that we want.
// to start, we'll need to initialize a simple pyramid, which we'll subdivide
// over all faces of it
void FractalMountain::init() {
    // generate the initial pyramid to subdivide
    init_mountain();

    // create the function that generates stuff as necessary
    string seed_phrase = "Sakura trees fall";
    std::seed_seq seed(seed_phrase.begin(), seed_phrase.end());
    std::default_random_engine gen(seed);
    std::normal_distribution<double> distribution(0.0, roughness);
    gen_num = [&gen, &distribution]() {
        return distribution(gen);
    };

    // the following subdivides as necessary
    std::vector<Triangle> new_faces;
    
    // then, for each depth we want, we'll subdivide every face
    for (uint i = 0; i < depth; ++i) {
        // for every single face, at this depth, we'll subdivide it once
        new_faces.clear();
        // preallocate to save space
        new_faces.reserve(7 * (m_faces.size() - 2) + 2);

        // avoid subdividing on the bottom two faces
        for (uint j = 0; j < m_faces.size(); ++j) {
            subdivide_face(new_faces, j);
        }
        // replace m_faces with new_faces
        m_faces = new_faces;
    }

    // finally, go through all the faces and find the highest point
    uint max_height = height;
    auto update_max = [&] (size_t v_idx) {
        if (max_height < m_vertices[v_idx].y) {
            max_height = m_vertices[v_idx].y;
        }
    };
    for (auto &face : m_faces) {
        // check all three vertices
        update_max(face.v1);
        update_max(face.v2);
        update_max(face.v3);
    }
    m_vertices[4].y = 1.01 * max_height;
    upper_right_corner = glm::vec3(width, m_vertices[4].y, length);
    // now let's make sure that there is a peak...
}

// helper function that starts by defining the peaks that we want
void FractalMountain::init_mountain() {
    // start out with a rectangular based pyramid
    glm::vec3 origin (0, 0, 0);
    glm::vec3 l_up (0, 0, -length);
    glm::vec3 r_up (width, 0, -length);
    glm::vec3 r_bottom (width, 0, 0);
    glm::vec3 peak (width / 2.0, height, -length / 2.0);

    // first, for each of these points above, let's add some texture vertices
    m_texture_vertices.push_back(glm::vec2( 0, 0 )); // origin 
    m_texture_vertices.push_back(glm::vec2( 0, 1 )); // l_up
    m_texture_vertices.push_back(glm::vec2( 1, 1 )); // r_up
    m_texture_vertices.push_back(glm::vec2( 1, 0 )); // r_bottom
    m_texture_vertices.push_back(glm::vec2( 0.5, 0.5 )); // peak

    m_vertices.push_back(origin); // 0
    m_vertices.push_back(l_up); // 1
    m_vertices.push_back(r_up); // 2
    m_vertices.push_back(r_bottom); // 3
    m_vertices.push_back(peak); // 4

    // top
    m_faces.push_back(Triangle(1, 0, 4, 1, 0, 4));
    m_faces.push_back(Triangle(1, 4, 2, 1, 4, 2));
    m_faces.push_back(Triangle(3, 2, 4, 3, 2, 4));
    m_faces.push_back(Triangle(0, 3, 4, 0, 3, 4));

    // then, let's also set the bounding box as well
    lower_left_corner = glm::vec3(0, 0, 0);
}

// this is a helper function that will generate a midpoint (perturbed in y)
glm::vec3 FractalMountain::generate_midpoint(const glm::vec3 &v1, const glm::vec3 &v2) {
    // perturb the heights of the midpoints by some amount
    glm::vec3 mid = (v1 + v2) / 2.0f;
    double d = glm::distance(v1, v2);

    // take P(d) = d
    double perturb = gen_num();

    // perturb it according to some distribution we provide
    mid.y += d * std::min(std::max(perturb, -1.5), 1.5);

    return mid;
}

// helper function that will subdivide a face, given a face to subdivide
// and we populate new_faces at the same time as doing so
void FractalMountain::subdivide_face(std::vector<Triangle> &new_faces, size_t face_idx) {
    // for each face subdivision, we want to generate seven new faces    
    // here's the trick though, the old vertex numbers will always stay the same
    // so, we although we will change the faces quite a bit, the vertices will stay the same

    // recall that faces are ordered in counter clockwise (respect to normal)
    Triangle face = m_faces[face_idx];

    // for each line segment, we want to get the midpoints.
    // First, let's do this, then generate the seven different faces we want
    glm::vec3 mid_1 = (m_vertices[face.v1] + m_vertices[face.v2]) / 2.0f;
    glm::vec3 mid_2 = generate_midpoint(m_vertices[face.v2], m_vertices[face.v3]);
    // don't perturb the very bottom (causes some lighting issues)
    glm::vec3 mid_3 = generate_midpoint(m_vertices[face.v3], m_vertices[face.v1]);

    // this helper function adds the vertex to the collection, and then
    // returns its index for future updates
    auto add_to_vertices = [&](const glm::vec3 &midpoint) {
        size_t idx = m_vertices.size();
        m_vertices.push_back(midpoint);
        // add the texture mapping as well
        // basically just ignore the y
        m_texture_vertices.push_back(glm::vec2(midpoint.x / width, midpoint.z / length));
        return idx;
    };

    size_t mid_1_idx = add_to_vertices(mid_1);
    size_t mid_2_idx = add_to_vertices(mid_2);
    size_t mid_3_idx = add_to_vertices(mid_3);

    // now let's generate the seven faces we want
    auto add_to_new_faces = [&] (size_t pt1, size_t pt2, size_t pt3, bool reverse = false) {
        if (reverse) {
            new_faces.push_back(Triangle( pt2, pt1, pt3, pt2, pt1, pt3 ));
            return;
        }
        new_faces.push_back(Triangle( pt1, pt2, pt3, pt1, pt2, pt3 ));
    };

    // top 3 faces: 
    add_to_new_faces( face.v3, face.v1, mid_3_idx );
    add_to_new_faces( face.v3, mid_3_idx, mid_2_idx );
    add_to_new_faces( face.v3, mid_2_idx, face.v2 );
    // bottom 3 faces:
    add_to_new_faces( face.v1, mid_1_idx, mid_3_idx );
    add_to_new_faces( mid_3_idx, mid_1_idx, mid_2_idx );
    add_to_new_faces( mid_2_idx, mid_1_idx, face.v2 );
}

// override the colour getting based on triangle
std::optional<glm::vec3> FractalMountain::get_colour_of_triangle(
    const Triangle &tri, const glm::vec3 &int_point
) {
    // our intersection point is actually the basis of triangle
    if (int_point.y < 0.5 * upper_right_corner.y) {
        return glm::vec3(0.1, 0.7, 0.1) - glm::vec3(0.1, 0.1, 0.1);
    }
    if (int_point.y < 0.8 * upper_right_corner.y) {
        return glm::vec3(0.8588, 0.6745, 0.2039);
    }
    return glm::vec3(1.0, 1.0, 1.0) - glm::vec3(0.3, 0.3, 0.3);
}

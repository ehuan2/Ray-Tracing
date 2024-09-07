// Termm--Fall 2023

#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

using namespace std;

// Mesh --------------------------------------------------------------------------------------------
// this splits a string by its '/', meant to parse face code
std::pair<size_t, std::optional<size_t>> split_string(const std::string &str) {
    std::istringstream isstream (str);
    std::string str1, str2;
    size_t s1, s2;

    if (str.find("/") == std::string::npos) {
        isstream >> s1;
        return make_pair(s1, std::nullopt);
    }

    bool get_val = (bool)std::getline(isstream, str1, '/');
    get_val &= (bool)std::getline(isstream, str2, '/');
    if (!get_val) {
        cout << "Misformed obj file" << endl;
        exit(1);
    }

    s1 = std::stoi(str1);
    s2 = std::stoi(str2);

    return make_pair(s1, s2);
}

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
    double vt_u, vt_v;
	size_t s1, s2, s3;

    // keep track of the smallest and largest vx, vy and vz:
    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double min_z = std::numeric_limits<double>::max();

    double max_x = std::numeric_limits<double>::min();
    double max_y = std::numeric_limits<double>::min();
    double max_z = std::numeric_limits<double>::min();
    
	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );

            if (vx < min_x) min_x = vx;
            if (vy < min_y) min_y = vy;
            if (vz < min_z) min_z = vz;

            if (vx > max_x) max_x = vx;
            if (vy > max_y) max_y = vy;
            if (vz > max_z) max_z = vz;
		} else if( code == "f" ) {
            // now we split it up in the way that we want:
            std::string str1, str2, str3;
            ifs >> str1 >> str2 >> str3;
            
            // now we need to split it by the 
            auto split_1 = split_string(str1);
            auto split_2 = split_string(str2);
            auto split_3 = split_string(str3);
            s1 = split_1.first;
            s2 = split_2.first;
            s3 = split_3.first;

            auto vt_idx_1 = split_1.second;
            auto vt_idx_2 = split_2.second;
            auto vt_idx_3 = split_3.second;

            if (vt_idx_1.has_value() && vt_idx_2.has_value() && vt_idx_3.has_value()) {
                // if we have texture mappings
                m_faces.push_back(
                    Triangle(
                        s1 - 1, s2 - 1, s3 - 1,
                        *vt_idx_1 - 1, *vt_idx_2 - 1, *vt_idx_3 - 1
                    )
                );
            } else {
			    m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
            }
		} else if ( code == "vt" ) {
            // have to parse the vertex texture coordinates as well
 	        ifs >> vt_u >> vt_v;
			m_texture_vertices.push_back( glm::vec2( vt_u, vt_v ) );
        }
	}

    // initializes the corners of the bounding box
    lower_left_corner = glm::vec3(min_x, min_y, min_z);
    upper_right_corner = glm::vec3(max_x, max_y, max_z);
}

// intialize an empty mesh (useful for inheritance)
Mesh::Mesh(): m_vertices(),m_texture_vertices(), m_faces(),
    lower_left_corner(), upper_right_corner() {}

// returns whether or not the ray intersects with the given triangle face
// if it does, then return the position and the normal, to colour it properly
OptIntersect Mesh::IntersectTriangle(const Ray &ray, const Triangle &tri) {
    // first, find the cross product
    glm::vec3 vec_one = m_vertices[tri.v3] - m_vertices[tri.v2];
    glm::vec3 vec_two = m_vertices[tri.v1] - m_vertices[tri.v2];

    glm::vec3 normal = glm::normalize(glm::cross(vec_one, vec_two));
    
    double epsilon = 1e-09;

    double A = normal.x;
    double B = normal.y;
    double C = normal.z;

    glm::vec3 avg = 1.0f/3.0f * (m_vertices[tri.v1] + m_vertices[tri.v2] + m_vertices[tri.v3]);

    double D = -(A * avg.x + B * avg.y + C * avg.z);

    auto start = ray.get_start();
    auto diff = glm::normalize(ray.get_diff_vec());

    double t = -(A * start.x + B * start.y + C * start.z + D) / (A * diff.x + B * diff.y + C * diff.z);

    if (t < epsilon || isnan(t) || isinf(t)) return {};

    glm::vec3 point_to_check = start + (float) t * diff;

    // now we check whether the point belongs in the triangle
    // we do this by switching to a basis of {vec_one, vec_two}. How do we transform vectors to this?
    // well we can just take the inverse of M = [vec_one, vec_two, normal]!

    glm::mat4 basis_mat;

    auto to_4d_vec = [](const glm::vec3 &vec) {
        return glm::vec4(vec.x, vec.y, vec.z, 0.0);
    };

    basis_mat[0] = to_4d_vec(vec_one);
    basis_mat[1] = to_4d_vec(vec_two);
    basis_mat[2] = to_4d_vec(normal);
    basis_mat[3] = glm::vec4(0, 0, 0, 1);

    glm::mat4 inverse = glm::inverse(basis_mat);

    // use tri.v2 as our origin
    glm::vec3 vec = point_to_check - m_vertices[tri.v2];
    glm::vec4 new_coordinates = inverse * to_4d_vec(vec);

    // clip to left side
    if (new_coordinates[0] <= epsilon) return {};
    // clip to bottom edge:
    if (new_coordinates[1] <= epsilon) return {};
    // clip to diagonal edge: discard it if y > 1 - x
    if (new_coordinates[1] + epsilon >= 1.0 - new_coordinates[0]) return {};

    // now let's get the colour for a specific triangle
    auto colour_triangle = get_colour_of_triangle(tri, point_to_check);

    // now that we know it's correct, we need to transform point_to_check
    // except we already did! it's just new_coordinates
    if (tri.uses_texture) {
        // then in this case, we need to take new_coordinates as a weighted average
        // of the two u, v to return
        glm::vec2 pt1 = m_texture_vertices[tri.vt1];
        glm::vec2 pt2 = m_texture_vertices[tri.vt2];
        glm::vec2 pt3 = m_texture_vertices[tri.vt3];

        // then we need to follow what we did to create it:
        // x = 3 - 2
        // y = 1 - 2
        glm::vec2 new_u = new_coordinates[0] * (pt3 - pt2);
        glm::vec2 new_v = new_coordinates[1] * (pt1 - pt2);
        glm::vec2 final_coord = new_u + new_v + pt2;

        // return the new u and v!
        return IntersectInfo {
            point_to_check,
            normal,
            LightingInfo {
                t,
                final_coord[0],
                final_coord[1],
                colour_triangle
            }
        };
    }

    // if it doesn't use texture, then return the coordinates as is
    return IntersectInfo {
        point_to_check,
        normal,
        LightingInfo {
            t,
            new_coordinates[0],
            new_coordinates[1],
            colour_triangle
        }
    };
}

OptIntersect Mesh::IntersectRay(const Ray &ray) {
    // over all the faces, get the smallest t value that is positive

    double min_distance = -1.0;
    OptIntersect out_intersect;

    for (auto &face : m_faces) {
        auto next_face = IntersectTriangle(ray, face);

        // if there's no intersection, we skip
        if (!next_face.has_value()) continue;

        // otherwise, let's compare and see if we hit it
        double next_dist = glm::distance(next_face->point_info, ray.get_start());

        if (!out_intersect.has_value() || (next_dist < min_distance)) {
            out_intersect = next_face;
            min_distance = next_dist;
        }
    }

    return out_intersect;
}

// helper function to print texture info
void Mesh::print_texture_info(const Triangle &tri) {
    glm::vec2 pt1 = m_texture_vertices[tri.vt1];
    glm::vec2 pt2 = m_texture_vertices[tri.vt2];
    glm::vec2 pt3 = m_texture_vertices[tri.vt3];

    printf(
        "Points: (%.3f, %.3f), (%.3f, %.3f), (%.3f, %.3f)\n",
        pt1.x, pt1.y,
        pt2.x, pt2.y,
        pt3.x, pt3.y
    );
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

// a function that we'll override in other primitives...
std::optional<glm::vec3> Mesh::get_colour_of_triangle(
    const Triangle &tri, const glm::vec3 &int_point
) {
    return std::nullopt;
}

void Mesh::set_bounding_box(BoundingBox **bbox_ptr) {
    *bbox_ptr = new BoundingBox(lower_left_corner, upper_right_corner);
}

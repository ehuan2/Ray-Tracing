// Termm--Fall 2023

#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;

    size_t vt1;
    size_t vt2;
    size_t vt3;

    bool uses_texture;

	Triangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
        , vt1 ( 0 )
        , vt2 ( 0 )
        , vt3 ( 0 )
        , uses_texture ( false )
	{}

	Triangle( size_t pv1, size_t pv2, size_t pv3, size_t pvt1, size_t pvt2, size_t pvt3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
        , vt1( pvt1 )
        , vt2( pvt2 )
        , vt3( pvt3 )
        , uses_texture ( true )
	{}
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname );

  // an empty mesh, with no vertices, texture vertices or faces yet
  Mesh();

protected:
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec2> m_texture_vertices;
	std::vector<Triangle> m_faces;
    
    // intialize the corners (which hold the minimums) of the bounding box
    glm::vec3 lower_left_corner;
    glm::vec3 upper_right_corner;

    OptIntersect IntersectRay(const Ray &ray) override;
    OptIntersect IntersectTriangle(const Ray &ray, const Triangle &tri);

    void print_texture_info(const Triangle &tri);
    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);

    virtual std::optional<glm::vec3> get_colour_of_triangle(
        const Triangle &tri, const glm::vec3 &int_point
    );

    // override writing the setting the bounding box
    void set_bounding_box(BoundingBox **bbox_ptr) override;
};

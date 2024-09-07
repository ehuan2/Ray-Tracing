// Termm--Fall 2023

#include "GeometryNode.hpp"

#include <glm/ext.hpp>
#include <iostream>

using namespace std;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
    , m_bbox( nullptr )
{
	m_nodeType = NodeType::GeometryNode;
    // now let's populate the bounding box
    if (m_primitive) {
        m_primitive->set_bounding_box(&m_bbox);
    }
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

// texture + bumping stuff
void GeometryNode::setTexture( Texture *texture ) {
    // obviously, we can leak memory here
    // this is modelled after the geometry node's set material function
    // so we can solve this using shared_ptr as well
    // but just as we have in setMaterial, we know that:
    // (1) This scene is constructed once, no texture maps will be overwritten
    // (2) We'll throw away our data afterwards
    m_texture = texture;
}

void GeometryNode::setBumping( Bumping *bumping ) {
    m_bumping = bumping;
}

// we intersect the ray with the current geometry node, return t as the parameter
// of where we would intersect the ray with the geometry
OptIntersect GeometryNode::IntersectRay(const Ray &ray) {
    // attempt at solving this by taking the inverse of the ray
    // and then solving for the intersection of the primitive with it
        // then just return whatever that t is

    // when intersecting the ray, we first want to intersect the bounding box
        // only continue to intersect if it intersects the bounding box
    if (m_bbox) {
        OptIntersect intersect = m_bbox->IntersectsBoundingBox(ray);
        if (!intersect.has_value()) return {};
        if (m_bbox->check_bb_hit(intersect.value())) return intersect;
    }

    // if the primitive doesn't exist, just don't do anything
    if (!m_primitive) return {};

    // now transform the normal of a mesh if it has a bumping
    if (m_bumping) {
        // a function that takes in a uv of origin of wave, and returns an axis of rotation
        auto intersect = m_primitive->IntersectRay(ray);
        if (!intersect) return {};

        auto get_axis_of_rotation = [&](const glm::vec2 &uv) {
            return glm::vec3(1, 0, 0);
        };

        glm::vec2 final_coord = glm::vec2(
            intersect->lighting_info.u,
            intersect->lighting_info.v
        );

        glm::vec3 final_normal =
            m_bumping->getNormalMapping(intersect->normal, get_axis_of_rotation, final_coord);

        return IntersectInfo {
            intersect->point_info,
            final_normal,
            intersect->lighting_info
        };
    }

    return m_primitive->IntersectRay(ray);
}

// acts almost identical to intersect ray except we need to apply things to two places
// instead of just one
SegmentList GeometryNode::IntersectSegment(const Ray &ray) {
    // attempt at solving this by taking the inverse of the ray
    // and then solving for the intersection of the primitive with it
        // then just return whatever that t is

    // when intersecting the ray, we first want to intersect the bounding box
        // only continue to intersect if it intersects the bounding box
    if (m_bbox) {
        OptIntersect intersect = m_bbox->IntersectsBoundingBox(ray);
        if (!intersect.has_value()) return {};
    }

    // if the primitive doesn't exist, just don't do anything
    if (!m_primitive) return {};

    // now transform the normal of a mesh if it has a bumping
    if (m_bumping) {
        // a function that takes in a uv of origin of wave, and returns an axis of rotation
        auto segment_list = m_primitive->IntersectSegment(ray, m_material);
        if (segment_list.size() == 0) return {};

        auto get_axis_of_rotation = [&](const glm::vec2 &uv) {
            return glm::vec3(1, 0, 0);
        };

        SegmentList output;

        // for every segment in the list, we want to transform their normal
        for (auto &segment : segment_list) {
            glm::vec2 final_coord_closer = glm::vec2(
                segment.closer.lighting_info.u,
                segment.closer.lighting_info.v
            );
            glm::vec3 norm_closer =
                m_bumping->getNormalMapping(
                    segment.closer.normal,
                    get_axis_of_rotation,
                    final_coord_closer
                );

            glm::vec2 final_coord_further = glm::vec2(
                segment.further.lighting_info.u,
                segment.further.lighting_info.v
            );
            glm::vec3 norm_further =
                m_bumping->getNormalMapping(
                    segment.further.normal,
                    get_axis_of_rotation,
                    final_coord_further
                );

            output.push_back(Segment {
                IntersectInfo {
                    segment.closer.point_info,
                    norm_closer,
                    segment.closer.lighting_info
                },
                IntersectInfo {
                    segment.further.point_info,
                    norm_further,
                    segment.further.lighting_info
                }
            });
        }

        return output;
    }

    // returns the intersection of the segment
    return m_primitive->IntersectSegment(ray, m_material);
}

// used to differentiate between texture mapping or not
glm::vec3 GeometryNode::get_kd(const IntersectInfo &intersect) {
    // some primitives, like the fractal mountain, wants its own colour
    if (intersect.lighting_info.colour.has_value()) {
        return *intersect.lighting_info.colour;
    }

    // some primitives will provide their own material as well
    if (intersect.lighting_info.mat) {
        return intersect.lighting_info.mat->get_kd();
    }

    if (!m_texture) {
        // doesn't use m_texture mapping, just return the kd we want
        if (!m_material) {
            cout << "no material provided for: " << m_name << endl;
            exit(1);
        }
        return m_material->get_kd();
    }

    // instead, we will use the image from the m_texture mapping
    return m_texture->getTextureColour(
        intersect.lighting_info.u,
        intersect.lighting_info.v
    );
}

// used to differentiate for csg vs. non-csg
glm::vec3 GeometryNode::get_ks(const IntersectInfo &intersect) {
    // some primitives will provide their own material as well
    if (intersect.lighting_info.mat) {
        return intersect.lighting_info.mat->get_ks();
    }

    return m_material->get_ks();
}

// used to differentiate for csg vs. non-csg
double GeometryNode::get_shininess(const IntersectInfo &intersect) {
    // some primitives will provide their own material as well
    if (intersect.lighting_info.mat) {
        return intersect.lighting_info.mat->get_shininess();
    }

    return m_material->get_shininess();
}

// used to differentiate for csg vs. non-csg
double GeometryNode::get_reflective(const IntersectInfo &intersect) {
    // some primitives will provide their own material as well
    if (intersect.lighting_info.mat) {
        return intersect.lighting_info.mat->get_reflective();
    }

    return m_material->get_reflective();
}

void GeometryNode::printMaterial() {
    if (!m_material) {
        cout << "No material provided" << endl;
        return;
    }
    m_material->print_material();
}


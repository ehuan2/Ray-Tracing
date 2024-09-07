// Termm--Fall 2023

#pragma once

#include "SceneNode.hpp"
#include "primitive/Primitive.hpp"
#include "primitive/BoundingBox.hpp"
#include "texture/Texture.hpp"
#include "bumping/Bumping.hpp"
#include "material/Material.hpp"
#include "../common/Ray.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	void setMaterial( Material *material );
    void setTexture(Texture *texture);
    void setBumping(Bumping *bumping);

    // function to help intersect a ray with it
    virtual OptIntersect IntersectRay(const Ray &ray);

    // now we have a function that will intersect a ray
    // with a primitive, however, it will also return the line segments
    // that we want
    virtual SegmentList IntersectSegment(const Ray &ray);

	Material *m_material;
	Primitive *m_primitive;
    BoundingBox *m_bbox;

    // we keep different textures per geometry node
    Texture *m_texture = nullptr;
    Bumping *m_bumping = nullptr;

    // gets the kd depending on whether or not it has a texture map or not
    glm::vec3 get_kd(const IntersectInfo &intersect);
    // ks and shininess will also depend on the primitive (for csg)
    glm::vec3 get_ks(const IntersectInfo &intersect);
    double get_shininess(const IntersectInfo &intersect);
    double get_reflective(const IntersectInfo &intersect);
    void printMaterial();
};

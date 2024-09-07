// Termm--Fall 2023

#pragma once

#include "../GeometryNode.hpp"
#include "../primitive/Primitive.hpp"
#include "../primitive/BoundingBox.hpp"
#include "../material/Material.hpp"
#include "../../common/Ray.hpp"

// an enum to hold the different types of operations we want to do
enum class CsgOperation {
    CsgUnion,
    CsgDiff,
    CsgIntersect
};

// want to treat this as a geometry node
class CsgNode : public GeometryNode {
public:
    CsgNode(
        const std::string & name,
        GeometryNode *left_ptr,
        GeometryNode *right_ptr,
        CsgOperation op
    );

    // function to help intersect a ray with it
    // except, we want to change how we intersect rays this time
    OptIntersect IntersectRay(const Ray &ray) override;
    SegmentList IntersectSegment(const Ray &ray) override;

    // this geometry node can either be another csg node
    // or it can be an actual geometry node
    // to start out, let's enforce that these nodes are both pure geometry nodes
    GeometryNode *left_ptr;
    GeometryNode *right_ptr;

    // hold the type of operation we're doing
    CsgOperation op;
};


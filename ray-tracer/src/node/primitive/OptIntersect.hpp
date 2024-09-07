// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <vector>
#include "../material/Material.hpp"
#include "../../common/Ray.hpp"

typedef std::optional<glm::vec3> OptPoint;

struct LightingInfo {
        double t;
        // these two doubles are used to get the proper u and v coordinates we want
        // that are between 0 and 1 for texture mapping
        double u;
        double v;
        std::optional<glm::vec3> colour = std::nullopt;
        Material *mat;
};

// data structures to hold the point and the normal for lighting information
struct IntersectInfo {
    glm::vec3 point_info;
    glm::vec3 normal;
    LightingInfo lighting_info;
};

typedef std::optional<struct IntersectInfo> OptIntersect;

struct Segment {
    IntersectInfo closer;
    IntersectInfo further;
};

typedef std::vector<Segment> SegmentList;


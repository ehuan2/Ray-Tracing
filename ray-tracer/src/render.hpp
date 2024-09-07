// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "tools/directives.hpp"
#include "node/SceneNode.hpp"
#include "node/GeometryNode.hpp"
#include "common/Light.hpp"
#include "Image.hpp"
#include "common/Ray.hpp"

using std::vector;

// intersection code:
typedef std::pair<OptIntersect, GeometryNode *> Intersection;
Intersection MinIntersect(
    const Ray &ray,
    SceneNode *root,
    SceneNode *no_intersect // represents a node that we cannot intersect with
);
vector<Intersection> Intersect(
    const Ray &ray,
    SceneNode *root,
    const glm::mat4 &inv_trans,
    SceneNode *no_intersect // represents a node that we cannot intersect with
);

// lighting code
glm::vec3 get_phong_lighting(
    const Ray &ray,
    GeometryNode *geom_node,
    const IntersectInfo &intersect,
    const glm::vec3 &ambient,
    const std::list<Light *> &lights
);

// shadows code
bool has_shadows(
    const glm::vec3 &light,
    const glm::vec3 &intersect_point,
    SceneNode *root,
    GeometryNode *geom_node // the node we cannot intersect with
);

// a recursive ray tracer meant to help implement reflections
    // if it returns optional, it means that the ray hit nothing
std::optional<glm::vec3> recurse_ray_trace(
    const Ray &ray,
    SceneNode *root,
    SceneNode *no_intersect,
    const glm::vec3 &ambient,
    const std::list<Light *> &lights,
    int depth
);

glm::vec3 get_background(uint x, uint y, size_t w, size_t h);

void A4_Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
);


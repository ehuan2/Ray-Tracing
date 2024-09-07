// Termm--Fall 2023
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "OptIntersect.hpp"

// a bounding box object which each primitive will take
class BoundingBox {
public:
  BoundingBox(
    const glm::vec3 &lower_left_corner,
    const glm::vec3 &upper_right_corner
  );

  OptIntersect IntersectsBoundingBox(const Ray &ray);
  bool check_bb_hit(const IntersectInfo &intersect_info);

private:
  glm::vec3 lower_left_corner;
  glm::vec3 upper_right_corner;
};


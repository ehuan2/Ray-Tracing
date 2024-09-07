// Termm--Fall 2023
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <functional>

// this class will generate a new bumping at a certain location
// given that it has:
// (u, v) coordinate of the center of the bumps
// (d^2, d) reduction over time
// period of how big the wave is
class Bumping {
public:
  Bumping(
    const glm::vec2 &bump_uv,
    double num_period,
    double period
  );

  // given the normal at a certain location,
  // its u and v locations of the intersection,
  // return a new normal that changes based off of the bumping
  glm::vec3 getNormalMapping(
    const glm::vec3 &normal,
    std::function<glm::vec3(const glm::vec2&)> get_rotation_axis,
    const glm::vec2 &uv
  );

private:
  glm::vec2 bump_uv;
  double num_period;
  double period;
};


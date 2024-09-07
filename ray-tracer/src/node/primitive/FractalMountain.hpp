// Termm--Fall 2023
#pragma once

#include "Mesh.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <functional>

class FractalMountain : public Mesh {
public:
  // fractal mountain generation
  FractalMountain(const glm::vec3 &dim, double roughness, uint depth);

private:
  // initialize the mountain model for us
  void init();
  void init_mountain();
  // this function provides the stochasticity for us :)
  glm::vec3 generate_midpoint(const glm::vec3 &v1, const glm::vec3 &v2);
  void subdivide_face(std::vector<Triangle> &new_faces, size_t face_idx);

  // override the colour getting based on triangle
  std::optional<glm::vec3> get_colour_of_triangle(
    const Triangle &tri, const glm::vec3 &int_point
  ) override;

  // width = |x_end - x_start|
  // height = |y_end - y_start|
  // length = |z_end - z_start|
  double width, height, length;
  double roughness;
  uint depth;
  std::function<double(void)> gen_num;
};


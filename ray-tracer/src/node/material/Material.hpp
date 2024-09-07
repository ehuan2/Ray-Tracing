// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>
#include <list>
#include "../../common/Ray.hpp"
#include "../../common/Light.hpp"

class Material {
public:
  virtual ~Material();
  virtual glm::vec3 get_kd() const { return glm::vec3(1.0, 1.0, 1.0); }
  virtual glm::vec3 get_ks() const { return glm::vec3(1.0, 1.0, 1.0); }
  virtual double get_shininess() const { return 0.0; }
  virtual double get_reflective() const { return 0.0; }
  virtual void print_material() const {}

protected:
  Material();
};

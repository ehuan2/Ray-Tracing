// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
  double m_reflective;

public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess);
  PhongMaterial(
    const glm::vec3& kd,
    const glm::vec3& ks,
    double shininess,
    double reflective
  );
  virtual ~PhongMaterial();

  glm::vec3 get_kd() const override { return m_kd; };
  glm::vec3 get_ks() const override { return m_ks; };
  double get_shininess() const override { return m_shininess; };
  double get_reflective() const override { return m_reflective; };

  void print_material() const override;
};

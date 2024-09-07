// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <string>
#include "../../common/Ray.hpp"
#include "OptIntersect.hpp"
#include "BoundingBox.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual OptIntersect IntersectRay(const Ray &ray);
  virtual SegmentList IntersectSegment(const Ray &ray, Material *mat);

  // this is a function that depending on the primitive will
  // provide a bounding box
  virtual void set_bounding_box(BoundingBox **bbox_ptr);
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  OptIntersect IntersectRay(const Ray &ray) override;
  SegmentList IntersectSegment(const Ray &ray, Material *mat) override;

private:
  size_t get_roots(const Ray &start_ray, double roots[2]);

  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();
  OptIntersect IntersectRay(const Ray &ray) override;
  SegmentList IntersectSegment(const Ray &ray, Material *mat) override;

private:
  glm::vec3 m_pos;
  double m_size;
};

class Sphere : public NonhierSphere {
public:
  Sphere(): NonhierSphere(glm::vec3(0.0, 0.0, 0.0), 1) {}
  virtual ~Sphere();
};

class Cube : public NonhierBox {
public:
  Cube(): NonhierBox(glm::vec3(0.0, 0.0, 0.0), 1) {}
  virtual ~Cube();
};

// we'll use the unit cylinder, where p = 1, q = 1, and the height
// is from -1 to 1
class Cylinder : public Primitive{
public:
  Cylinder() {}
  virtual ~Cylinder();
  OptIntersect IntersectRay(const Ray &ray) override;
  SegmentList IntersectSegment(const Ray &ray, Material *mat) override;
};

// we'll use the unit cone, where p = 1, q = 1, and the height
// is from 0 to 1
class Cone : public Primitive {
public:
  Cone() {}
  virtual ~Cone();
  OptIntersect IntersectRay(const Ray &ray) override;
  SegmentList IntersectSegment(const Ray &ray, Material *mat) override;
};




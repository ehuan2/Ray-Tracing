// Termm--Fall 2023
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

class Texture {
public:
  // texture mapping stuff
  Texture(const std::string &fname);
  // u and v must be between 0 and 1
  // gets the texture's colour info at that location
  glm::vec3 getTextureColour(double u, double v);
private:
  std::vector<unsigned char> image;
  unsigned width;
  unsigned height;
};


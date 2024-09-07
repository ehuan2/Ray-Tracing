// Termm--Fall 2023

#pragma once
#include <glm/glm.hpp>

class Ray {
    glm::vec3 start;
    glm::vec3 end;

public:
    Ray(glm::vec3 start, glm::vec3 end): start{start}, end{end}{}
    Ray transform(const glm::mat4 &trans) const;
    Ray normalize() const;
    glm::vec3 get_diff_vec() const;
    glm::vec3 get_start() const;

    void print() const;
};


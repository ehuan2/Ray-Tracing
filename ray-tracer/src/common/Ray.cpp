// Termm--Fall 2023

#include <iostream>

#include "Ray.hpp"

using namespace std;

Ray Ray::transform(const glm::mat4 &trans) const {
    auto transformed_start = trans * glm::vec4(start.x, start.y, start.z, 1.0);
    auto transformed_end = trans * glm::vec4(end.x, end.y, end.z, 1.0);

    return Ray (
        glm::vec3(transformed_start.x, transformed_start.y, transformed_start.z),
        glm::vec3(transformed_end.x, transformed_end.y, transformed_end.z)
    );
}

glm::vec3 Ray::get_diff_vec() const {
    return end - start;
}

glm::vec3 Ray::get_start() const {
    return start;
}

Ray Ray::normalize() const {
    glm::vec3 diff_vec = get_diff_vec();
    return Ray (start, start + glm::normalize(diff_vec));
}

void Ray::print() const {
    auto print_vec3 = [](glm::vec3 vec) {
        cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    };
    print_vec3(start);
    cout << " ";
    print_vec3(end);
    cout << endl;
}


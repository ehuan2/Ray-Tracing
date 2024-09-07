#include "Bumping.hpp"
#include <glm/gtx/transform.hpp>
#include <iostream>
using namespace std;

// texture mapping stuff
Bumping::Bumping(
    const glm::vec2 &bump_uv,
    double num_period,
    double period
):
    bump_uv(bump_uv),
    num_period(num_period),
    period(period) {}

// get the normal mapping
glm::vec3 Bumping::getNormalMapping(
    const glm::vec3 &normal,
    std::function<glm::vec3(const glm::vec2&)> get_rotation_axis,
    const glm::vec2 &uv
) {
    // first, if (u, v) == bump_uv just return the normal
    if (bump_uv == uv) return normal;
    
    // first let's get the distance, and we want the angle to go from -45 to 45 degrees in a sine pattern
    double d = glm::distance(bump_uv, uv);

    // if the number of periods is greater than num_period, stop
    if ((d / period) > num_period) return normal;

    double angle = 1.0 * glm::cos(2.0 * glm::pi<double>() * d / period);

    // rotate it around the axis given
    glm::vec3 axis_rotation = get_rotation_axis(uv);
    glm::mat4 rotation = glm::rotate((float)angle, axis_rotation);
    glm::vec4 new_normal = rotation * glm::vec4(normal.x, normal.y, normal.z, 0);
    return glm::normalize(glm::vec3(new_normal.x, new_normal.y, new_normal.z));
}


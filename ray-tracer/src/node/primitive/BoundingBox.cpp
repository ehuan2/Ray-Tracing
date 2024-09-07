#include "BoundingBox.hpp"
// don't forget to include the directive!
#include "../../tools/directives.hpp"

BoundingBox::BoundingBox(
    const glm::vec3 &lower_left_corner,
    const glm::vec3 &upper_right_corner
) : lower_left_corner(lower_left_corner), upper_right_corner(upper_right_corner) {}

// these are algorithms for the bounding box, which we'll have each primitive take
OptIntersect BoundingBox::IntersectsBoundingBox(const Ray &ray) {
    double epsilon = 0.001;

    auto start = ray.get_start();
    auto diff = glm::normalize(ray.get_diff_vec());

    // solve each face, and if it hits, we return true
    auto get_point_to_check = [&](double t) {
        return start + ((float) t * diff);
    };

    double min_t = -1;
    glm::vec3 output_normal;

    auto update_if_in_range = [&](
        double dim_one,
        double dim_one_lower,
        double dim_one_upper,
        double dim_two,
        double dim_two_lower,
        double dim_two_upper,
        double t,
        glm::vec3 norm
    ) {
        if (dim_one_lower <= dim_one && dim_one <= dim_one_upper
            && dim_two_lower <= dim_two && dim_two <= dim_two_upper) {
            if (t >= 0 - epsilon && (min_t == -1 || t < min_t)) {
                min_t = t;
                output_normal = norm;
            }
        }
    };

    // now we generate all 6 different types:
    // x faces (side of a cube)
    if (glm::abs(diff.x) >= epsilon) {
        // check if the temp t's will work

        // right face (positive x)
        double temp = (upper_right_corner.x - start.x) / diff.x;
        // check if temp would make the vector hit in the right plane
        glm::vec3 point_to_check = get_point_to_check(temp);
        glm::vec3 normal = glm::vec3(1.0, 0.0, 0.0);

        update_if_in_range(
            point_to_check.y,
            lower_left_corner.y,
            upper_right_corner.y,
            point_to_check.z,
            lower_left_corner.z,
            upper_right_corner.z,
            temp,
            normal
        );

        // left face
        temp = (lower_left_corner.x - start.x) / diff.x;
        point_to_check = get_point_to_check(temp);
        normal = glm::vec3(-1.0, 0.0, 0.0);

        update_if_in_range(
            point_to_check.y,
            lower_left_corner.y,
            upper_right_corner.y,
            point_to_check.z,
            lower_left_corner.z,
            upper_right_corner.z,
            temp,
            normal
        );
    }

    // now we generate all 6 different types:
    // y faces (side of a cube)
    if (glm::abs(diff.y) >= epsilon) {
        // check if the temp t's will work

        // upper face (positive y)
        double temp = (upper_right_corner.y - start.y) / diff.y;
        // check if temp would make the vector hit in the right plane
        glm::vec3 point_to_check = get_point_to_check(temp);
        glm::vec3 normal = glm::vec3(0.0, 1.0, 0.0);

        update_if_in_range(
            point_to_check.x,
            lower_left_corner.x,
            upper_right_corner.x,
            point_to_check.z,
            lower_left_corner.z,
            upper_right_corner.z,
            temp,
            normal
        );

        // bottom face
        temp = (lower_left_corner.y - start.y) / diff.y;
        point_to_check = get_point_to_check(temp);
        normal = glm::vec3(0.0, -1.0, 0.0);
        update_if_in_range(
            point_to_check.x,
            lower_left_corner.x,
            upper_right_corner.x,
            point_to_check.z,
            lower_left_corner.z,
            upper_right_corner.z,
            temp,
            normal
        );
    }

    // now we generate all 6 different tzpes:
    // z faces (side of a cube)
    if (glm::abs(diff.z) >= epsilon) {
        // check if the temp t's will work

        // upper face (positive z)
        double temp = (upper_right_corner.z - start.z) / diff.z;
        // check if temp would make the vector hit in the right plane
        glm::vec3 point_to_check = get_point_to_check(temp);
        glm::vec3 normal = glm::vec3(0.0, 0.0, 1.0);

        update_if_in_range(
            point_to_check.x,
            lower_left_corner.x,
            upper_right_corner.x,
            point_to_check.y,
            lower_left_corner.y,
            upper_right_corner.y,
            temp,
            normal
        );

        // bottom face
        temp = (lower_left_corner.z - start.z) / diff.z;
        point_to_check = get_point_to_check(temp);
        normal = glm::vec3(0.0, 0.0, -1.0);

        update_if_in_range(
            point_to_check.x,
            lower_left_corner.x,
            upper_right_corner.x,
            point_to_check.y,
            lower_left_corner.y,
            upper_right_corner.y,
            temp,
            normal
        );
    }

    if (min_t == -1) return {};

    return IntersectInfo {
        get_point_to_check(min_t),
        output_normal,
        LightingInfo { min_t }
    };
}

// helper function to check if it hits the side of the bounding box
bool BoundingBox::check_bb_hit(const IntersectInfo &intersect_info) {
#ifndef RENDER_BOUNDING_VOLUMES
    return false;
#endif
    return true;
    double epsilon = 0.5;

    auto within_bounds = [&](double check, double against) {
        return against - epsilon <= check && check <= against + epsilon;
    };

    // return true as long as two of these match the min and max bounds
    bool x_match = within_bounds(intersect_info.point_info.x, lower_left_corner.x) || 
        within_bounds(intersect_info.point_info.x, upper_right_corner.x);
    bool y_match = within_bounds(intersect_info.point_info.y, lower_left_corner.y) || 
        within_bounds(intersect_info.point_info.y, upper_right_corner.y);
    bool z_match = within_bounds(intersect_info.point_info.z, lower_left_corner.z) || 
        within_bounds(intersect_info.point_info.z, upper_right_corner.z);

    return (x_match && (y_match || z_match))
        || (y_match && (x_match || z_match))
        || (z_match && (x_match || y_match));
}


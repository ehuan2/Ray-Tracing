// Termm--Fall 2023

#include "Primitive.hpp"
#include "../../tools/polyroots.hpp"
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

Primitive::~Primitive()
{
}

OptIntersect Primitive::IntersectRay(const Ray &ray) {
    return {};
}

SegmentList Primitive::IntersectSegment(const Ray &ray, Material *mat) {
    return std::vector<Segment>();
}

void Primitive::set_bounding_box(BoundingBox **bbox_ptr) {
    // set it to null
    *bbox_ptr = nullptr;
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

size_t NonhierSphere::get_roots(const Ray &start_ray, double roots[2]) {
    double epsilon = 0.01;

    // transform the ray based off of the non hierarchical sphere
        // first, undo the translate, then do the reverse scale
    auto ray = start_ray
            .transform(glm::translate(glm::mat4(), -m_pos))
            .transform(glm::scale(glm::vec3(1.0 / m_radius, 1.0 / m_radius, 1.0 / m_radius)));

    auto start = ray.get_start();
    auto diff = glm::normalize(ray.get_diff_vec());

    double A = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);
    double B = 2.0 * (diff.x * start.x + diff.y * start.y + diff.z * start.z);
    double C = (start.x * start.x + start.y * start.y + start.z * start.z - 1.0);

    return quadraticRoots(A, B, C, roots);
}

OptIntersect NonhierSphere::IntersectRay(const Ray &start_ray) {
    double epsilon = 0.01;
    double roots[2];
    size_t num_roots = get_roots(start_ray, roots);

    auto ray = start_ray
            .transform(glm::translate(glm::mat4(), -m_pos))
            .transform(glm::scale(glm::vec3(1.0 / m_radius, 1.0 / m_radius, 1.0 / m_radius)));
    auto start = ray.get_start();
    auto diff = glm::normalize(ray.get_diff_vec());

    if (num_roots == 0) {
        return {};
    }

    auto get_output = [&](double t) {
        return IntersectInfo {
            start_ray.get_start() + (float)(t * m_radius) * start_ray.get_diff_vec(),
            start + (float) t * diff,
            LightingInfo { t }
        };
    };

    if (num_roots == 1) {
        if (roots[0] < epsilon) return {};
        return get_output(roots[0]);
    }

    // take the smaller of the two that's over 0
    if (roots[0] < epsilon && roots[1] < epsilon) return {};
    if (roots[0] < epsilon) return get_output(roots[1]);
    if (roots[1] < epsilon) return get_output(roots[0]);

    // return whichever one is smaller, since they're both >= 0
    IntersectInfo root_zero_out = get_output(roots[0]);
    double root_zero_dist = glm::distance(start_ray.get_start(), root_zero_out.point_info);
    IntersectInfo root_one_out = get_output(roots[1]);
    double root_one_dist = glm::distance(start_ray.get_start(), root_one_out.point_info);

    auto print_point = [](const glm::vec3 &vec) {
        cout << vec.x << " " << vec.y << " " << vec.z;
    };

    return root_zero_dist < root_one_dist ? root_zero_out : root_one_out;
}

SegmentList NonhierSphere::IntersectSegment(const Ray &start_ray, Material *mat) {
    double epsilon = 0.01;
    double roots[2];
    size_t num_roots = get_roots(start_ray, roots);

    auto ray = start_ray
            .transform(glm::translate(glm::mat4(), -m_pos))
            .transform(glm::scale(glm::vec3(1.0 / m_radius, 1.0 / m_radius, 1.0 / m_radius)));
    auto start = ray.get_start();
    auto diff = glm::normalize(ray.get_diff_vec());

    if (num_roots == 0) return SegmentList();

    auto get_output = [&](double t) {
        // todo: fill in u and v

        return IntersectInfo {
            start_ray.get_start() + (float)(t * m_radius) * start_ray.get_diff_vec(),
            start + (float) t * diff,
            LightingInfo { t, 0.0, 0.0, std::nullopt, mat }
        };
    };

    auto get_segment_list_of_one = [&](double t) {
        // return a single segment list of just one segment with furthest
        // and closest being the same thing
        SegmentList output;

        auto intersect = get_output(t);
        double eps = 1e-09;
        auto intersect_plus_epsilon = get_output(t + eps);
        output.push_back(Segment {
            intersect, intersect_plus_epsilon
        });

        return output;
    };

    if (num_roots == 1) {
        if (roots[0] < epsilon) return SegmentList();
        return get_segment_list_of_one(roots[0]);
    }

    // take the smaller of the two that's over 0
    if (roots[0] < epsilon && roots[1] < epsilon) return SegmentList();
    if (roots[0] < epsilon) return get_segment_list_of_one(roots[1]);
    if (roots[1] < epsilon) return get_segment_list_of_one(roots[0]);

    // depending on which one is smaller, return a different ordering
    IntersectInfo root_zero_out = get_output(roots[0]);
    double root_zero_dist = glm::distance(start_ray.get_start(), root_zero_out.point_info);
    IntersectInfo root_one_out = get_output(roots[1]);
    double root_one_dist = glm::distance(start_ray.get_start(), root_one_out.point_info);

    SegmentList output;
    output.push_back(
        root_zero_dist < root_one_dist ?
        Segment{ root_zero_out, root_one_out } :
        Segment{ root_one_out, root_zero_out }
    );
    return output;
}

NonhierBox::~NonhierBox()
{
}

OptIntersect NonhierBox::IntersectRay(const Ray &start_ray) {
    double epsilon = 0.01;

    // transform the ray based off of the non hierarchical sphere
        // first, undo the translate, then do the reverse scale
    auto ray = start_ray
            .transform(glm::translate(glm::mat4(), -m_pos))
            .transform(glm::scale(glm::vec3(1.0 / m_size, 1.0 / m_size, 1.0 / m_size)));

    auto start = ray.get_start();
    auto diff = glm::normalize(ray.get_diff_vec());

    // solve each face, and if it hits, we return whichever one is the smallest t
    double min_t = -1;
    glm::vec3 output_normal;

    auto get_point_to_check = [&](double t) {
        return start + ((float) t * diff);
    };

    auto update_if_in_range = [&](double dim_one, double dim_two, double t, glm::vec3 norm) {
        if (0 <= dim_one && dim_one <= 1 && 0 <= dim_two && dim_two <= 1) {
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
        double temp = (1.0 - start.x) / diff.x;
        // check if temp would make the vector hit in the right plane
        glm::vec3 point_to_check = get_point_to_check(temp);
        update_if_in_range(point_to_check.y, point_to_check.z, temp, glm::vec3(1.0, 0.0, 0.0));

        temp = (-start.x) / diff.x;
        point_to_check = get_point_to_check(temp);
        update_if_in_range(point_to_check.y, point_to_check.z, temp, glm::vec3(-1.0, 0.0, 0.0));
    }
    // y faces (side of a cube)
    if (glm::abs(diff.y) >= epsilon) {
        // check if the temp t's will work

        // top face (positive y)
        double temp = (1.0 - start.y) / diff.y;
        // check if temp would make the vector hit in the right plane
        glm::vec3 point_to_check = get_point_to_check(temp);
        update_if_in_range(point_to_check.x, point_to_check.z, temp, glm::vec3(0.0, 1.0, 0.0));

        temp = (-start.y) / diff.y;
        point_to_check = get_point_to_check(temp);
        update_if_in_range(point_to_check.x, point_to_check.z, temp, glm::vec3(0.0, -1.0, 0.0));
    }
    // z faces (side of a cube)
    if (glm::abs(diff.z) >= epsilon) {
        // check if the temp t's will work

        // right face (positive z)
        double temp = (1.0 - start.z) / diff.z;
        // check if temp would make the vector hit in the right plane
        glm::vec3 point_to_check = get_point_to_check(temp);
        update_if_in_range(point_to_check.x, point_to_check.y, temp, glm::vec3(0.0, 0.0, 1.0));

        temp = (-start.z) / diff.z;
        point_to_check = get_point_to_check(temp);
        update_if_in_range(point_to_check.x, point_to_check.y, temp, glm::vec3(0.0, 0.0, -1.0));
    }

    if (min_t == -1) {
        return {};
    }

    return IntersectInfo {
        start_ray.get_start() + ((float) (min_t * m_size) * start_ray.get_diff_vec()),
        output_normal,
        LightingInfo { min_t }
    };
}

SegmentList NonhierBox::IntersectSegment(const Ray &start_ray, Material *mat) {
    double epsilon = 0.0001;

    // transform the ray based off of the non hierarchical sphere
        // first, undo the translate, then do the reverse scale
    auto ray = start_ray
            .transform(glm::translate(glm::mat4(), -m_pos))
            .transform(glm::scale(glm::vec3(1.0 / m_size, 1.0 / m_size, 1.0 / m_size)));

    auto start = ray.get_start();
    auto diff = glm::normalize(ray.get_diff_vec());

    // solve each face, and if it hits, we return whichever one is the smallest t
    auto get_point_to_check = [&](double t) {
        return start + ((float) t * diff);
    };

    auto get_intersect_from_t = [&](double t, const glm::vec3 &norm) {
        return IntersectInfo {
            start_ray.get_start() +
                ((float) (t * m_size) * start_ray.get_diff_vec()),
            norm,
            LightingInfo {
                t, 0.0, 0.0, std::nullopt, mat
            }
        };
    };

    OptIntersect first = std::nullopt;
    OptIntersect second = std::nullopt;
    double min_dist = std::numeric_limits<double>::max();
    double second_min_dist = std::numeric_limits<double>::max();

    auto update_minimum = [&](const IntersectInfo &intersect) {
        double next_dist = glm::distance(ray.get_start(), intersect.point_info);
        if (next_dist < min_dist) {
            second = first; // update the second one first
            second_min_dist = min_dist;
            first = intersect;
            min_dist = next_dist;
        } else if (next_dist < second_min_dist) {
            // only update second
            second = intersect;
            second_min_dist = next_dist;
        }
    };


    auto check_if_in_range = [&](double dim_one, double dim_two) {
        if (0 <= dim_one && dim_one <= 1 && 0 <= dim_two && dim_two <= 1) {
            return true;
        }
        return false;
    };

    // now we generate all 6 different types:
    // x faces (side of a cube)
    if (glm::abs(diff.x) >= epsilon) {
        // check if the temp t's will work

        // right face (positive x)
        double temp = (1.0 - start.x) / diff.x;
        // check if temp would make the vector hit in the right plane
        glm::vec3 point_to_check = get_point_to_check(temp);
        if (check_if_in_range(point_to_check.y, point_to_check.z)) {
            update_minimum(IntersectInfo {
                ray.get_start() + (float) (temp) * ray.get_diff_vec(),
                glm::vec3(1.0, 0.0, 0.0),
                LightingInfo { temp, 0, 0, std::nullopt, mat }
            });
        }

        temp = (-start.x) / diff.x;
        point_to_check = get_point_to_check(temp);
        if (check_if_in_range(point_to_check.y, point_to_check.z)) {
            update_minimum(IntersectInfo {
                ray.get_start() + (float) (temp) * ray.get_diff_vec(),
                glm::vec3(-1.0, 0.0, 0.0),
                LightingInfo { temp, 0, 0, std::nullopt, mat }
            });
        }
    }

    // y faces (side of a cube)
    if (glm::abs(diff.y) >= epsilon) {
        // check if the temp t's will work
        // top face (positive y)
        double temp = (1.0 - start.y) / diff.y;
        // check if temp would make the vector hit in the right plane
        glm::vec3 point_to_check = get_point_to_check(temp);
        if (check_if_in_range(point_to_check.x, point_to_check.z)) {
            update_minimum(IntersectInfo {
                ray.get_start() + (float) (temp) * ray.get_diff_vec(),
                glm::vec3(0.0, 1.0, 0.0),
                LightingInfo { temp, 0, 0, std::nullopt, mat }
            });
        }

        temp = (-start.y) / diff.y;
        point_to_check = get_point_to_check(temp);
        if (check_if_in_range(point_to_check.x, point_to_check.z)) {
            update_minimum(IntersectInfo {
                ray.get_start() + (float) (temp) * ray.get_diff_vec(),
                glm::vec3(0.0, -1.0, 0.0),
                LightingInfo { temp, 0, 0, std::nullopt, mat }
            });
        }
    }

    // z faces (side of a cube)
    if (glm::abs(diff.z) >= epsilon) {
        // check if the temp t's will work

        // right face (positive z)
        double temp = (1.0 - start.z) / diff.z;
        // check if temp would make the vector hit in the right plane
        glm::vec3 point_to_check = get_point_to_check(temp);
        if (check_if_in_range(point_to_check.x, point_to_check.y)) {
            update_minimum(IntersectInfo {
                ray.get_start() + (float) (temp) * ray.get_diff_vec(),
                glm::vec3(0.0, 0.0, 1.0),
                LightingInfo { temp, 0, 0, std::nullopt, mat }
            });
        }

        temp = (-start.z) / diff.z;
        point_to_check = get_point_to_check(temp);
        if (check_if_in_range(point_to_check.x, point_to_check.y)) {
            update_minimum(IntersectInfo {
                ray.get_start() + (float) (temp) * ray.get_diff_vec(),
                glm::vec3(0.0, 0.0, -1.0),
                LightingInfo { temp, 0, 0, std::nullopt, mat }
            });
        }
    }

    if (!first || !second) return SegmentList();

    SegmentList output;
    output.push_back(Segment {
        *first, *second
    });
    return output;
}

// use the unit cylinder, p = 1, q = 1, height is from -1 to 1
OptIntersect Cylinder::IntersectRay(const Ray &ray) {
    double epsilon = 0.01;

    auto start = ray.get_start();
    auto diff = glm::normalize(ray.get_diff_vec());

    double A = (diff.x * diff.x) + (diff.z * diff.z);
    double B = 2.0 * (diff.x * start.x + diff.z * start.z);
    double C = (start.x * start.x + start.z * start.z - 1.0);

    double roots[2];

    size_t num_roots = quadraticRoots(A, B, C, roots);

    if (num_roots == 0) {
        return {};
    }

    auto get_output = [&](double t) {
        // let's also calculate the u and v coordinates for texture mapping
        glm::vec3 final_point =
            ray.get_start() + (float) (t) * ray.get_diff_vec();
        double theta = glm::atan(final_point.x, final_point.z);

        double u = 1 - (theta / (2.0 * glm::pi<double>()) + 0.5);

        // since the height we have is 2, starting at -1 to 1
        double v = 1.0 - (final_point.y + 1.0) / 2.0;

        return IntersectInfo {
            final_point,
            start + (float) t * diff,
            LightingInfo { t, u, v }
        };
    };

    // first let's check if either one would have a proper height
    // since we want to bound the height
    auto check_height = [&](double t) {
        double height = (ray.get_start() + (float)(t) * ray.get_diff_vec()).y;
        return height < 1.0 && height > -1.0;
    };

    auto check_top_bottom = [&](double t) {
        if (t <= 0.0) return OptIntersect{};
        // has x^2 + z^2 <= 1
        glm::vec3 final_point =
            ray.get_start() + (float) (t) * ray.get_diff_vec();

        if (final_point.x * final_point.x + final_point.z * final_point.z > 1) {
            return OptIntersect{};
        }

        OptIntersect output = IntersectInfo {
            final_point,
            glm::vec3(0, final_point.y, 0),
            LightingInfo { t }
        };
        return output;
    };

    OptIntersect min;
    double min_dist = std::numeric_limits<double>::max();

    auto update_minimum = [&](const IntersectInfo &intersect) {
        double next_dist = glm::distance(ray.get_start(), intersect.point_info);
        if (next_dist < min_dist) {
            min_dist = next_dist;
            min = intersect;
        }
    };

    // let's also intersect with the planes of y = -1 and y = 1
    if (glm::abs(diff.y) >= epsilon) {
        double bottom_t = (-1.0 - start.y) / diff.y;
        // now we return the output if the resulting output
        double top_t = (1.0 - start.y) / diff.y;

        auto bottom = check_top_bottom(bottom_t);
        auto top = check_top_bottom(top_t);

        if (bottom.has_value()) update_minimum(*bottom);
        if (top.has_value()) update_minimum(*top);
    }

    if (num_roots == 1) {
        if (roots[0] < epsilon || !check_height(roots[0])) return min;
        update_minimum(get_output(roots[0]));
        return min;
    }

    // make sure that they both have height that's within our range
    bool zero_height = check_height(roots[0]);
    bool one_height = check_height(roots[1]);

    // if neither are safe, then stop
    if (!zero_height && !one_height) return min;

    // take the smaller of the two that's over 0
    if (roots[0] < epsilon && roots[1] < epsilon) return min;
    if (roots[0] < epsilon) {
        if (!one_height) return min;
        update_minimum(get_output(roots[1]));
        return min;
    }
    if (roots[1] < epsilon) {
        if (!zero_height) return min;
        update_minimum(get_output(roots[0]));
        return min;
    }

    if (!one_height) {
        update_minimum(get_output(roots[0]));
        return min;
    }
    if (!zero_height) {
        update_minimum(get_output(roots[1]));
        return min;
    }

    // at this point, both of them should be valid
    // return whichever one is smaller, since they're both >= 0
    IntersectInfo root_zero_out = get_output(roots[0]);
    IntersectInfo root_one_out = get_output(roots[1]);

    update_minimum(root_zero_out);
    update_minimum(root_one_out);

    return min;
}

// use the unit cylinder, p = 1, q = 1, height is from -1 to 1
SegmentList Cylinder::IntersectSegment(const Ray &ray, Material *mat) {
    double epsilon = 0.0001;

    auto start = ray.get_start();
    auto diff = glm::normalize(ray.get_diff_vec());

    double A = (diff.x * diff.x) + (diff.z * diff.z);
    double B = 2.0 * (diff.x * start.x + diff.z * start.z);
    double C = (start.x * start.x + start.z * start.z - 1.0);

    double roots[2];

    size_t num_roots = quadraticRoots(A, B, C, roots);

    if (num_roots == 0) {
        return {};
    }

    auto get_output = [&](double t) {
        // let's also calculate the u and v coordinates for texture mapping
        glm::vec3 final_point =
            ray.get_start() + (float) (t) * ray.get_diff_vec();
        double theta = glm::atan(final_point.x, final_point.z);

        double u = 1 - (theta / (2.0 * glm::pi<double>()) + 0.5);

        // since the height we have is 2, starting at -1 to 1
        double v = 1.0 - (final_point.y + 1.0) / 2.0;

        return IntersectInfo {
            final_point,
            start + (float) t * diff,
            LightingInfo { t, u, v, std::nullopt, mat }
        };
    };

    // first let's check if either one would have a proper height
    // since we want to bound the height
    auto check_height = [&](double t) {
        double height = (ray.get_start() + (float)(t) * ray.get_diff_vec()).y;
        return height < 1.0 && height > -1.0;
    };

    auto check_top_bottom = [&](double t) {
        if (t <= 0.0) return OptIntersect{};
        // has x^2 + z^2 <= 1
        glm::vec3 final_point =
            ray.get_start() + (float) (t) * ray.get_diff_vec();

        if (final_point.x * final_point.x + final_point.z * final_point.z > 1) {
            return OptIntersect{};
        }

        OptIntersect output = IntersectInfo {
            final_point,
            glm::vec3(0, final_point.y, 0),
            LightingInfo { t, 0, 0, std::nullopt, mat }
        };
        return output;
    };

    OptIntersect first = std::nullopt;
    OptIntersect second = std::nullopt;
    double min_dist = std::numeric_limits<double>::max();
    double second_min_dist = std::numeric_limits<double>::max();

    auto update_minimum = [&](const IntersectInfo &intersect) {
        double next_dist = glm::distance(ray.get_start(), intersect.point_info);
        if (next_dist < min_dist) {
            second = first; // update the second one first
            second_min_dist = min_dist;
            first = intersect;
            min_dist = next_dist;
        } else if (next_dist < second_min_dist) {
            // only update second
            second = intersect;
            second_min_dist = next_dist;
        }
    };

    auto get_final_seg = [&]() {
        if (!first || !second) return SegmentList();
        SegmentList output;
        output.push_back(Segment { *first, *second });
        return output;
    };

    // let's also intersect with the planes of y = -1 and y = 1
    if (glm::abs(diff.y) >= epsilon) {
        double bottom_t = (-1.0 - start.y) / diff.y;
        // now we return the output if the resulting output
        double top_t = (1.0 - start.y) / diff.y;

        auto bottom = check_top_bottom(bottom_t);
        auto top = check_top_bottom(top_t);

        if (bottom.has_value()) update_minimum(*bottom);
        if (top.has_value()) update_minimum(*top);
    }

    if (num_roots == 1) {
        if (roots[0] < epsilon || !check_height(roots[0])) return get_final_seg();
        update_minimum(get_output(roots[0]));
        return get_final_seg();
    }

    // make sure that they both have height that's within our range
    bool zero_height = check_height(roots[0]);
    bool one_height = check_height(roots[1]);

    // if neither are safe, then stop
    if (!zero_height && !one_height) return get_final_seg();

    // take the smaller of the two that's over 0
    if (roots[0] < epsilon && roots[1] < epsilon) return get_final_seg();
    if (roots[0] < epsilon) {
        if (!one_height) return get_final_seg();
        update_minimum(get_output(roots[1]));
        return get_final_seg();
    }
    if (roots[1] < epsilon) {
        if (!zero_height) return get_final_seg();
        update_minimum(get_output(roots[0]));
        return get_final_seg();
    }

    if (!one_height) {
        update_minimum(get_output(roots[0]));
        return get_final_seg();
    }
    if (!zero_height) {
        update_minimum(get_output(roots[1]));
        return get_final_seg();
    }

    // at this point, both of them should be valid
    // return whichever one is smaller, since they're both >= 0
    IntersectInfo root_zero_out = get_output(roots[0]);
    IntersectInfo root_one_out = get_output(roots[1]);

    update_minimum(root_zero_out);
    update_minimum(root_one_out);

    return get_final_seg();
}

Cylinder::~Cylinder() {}

// Cone Intersections ------------------------------------------------------------------------------
OptIntersect Cone::IntersectRay(const Ray &ray) {
    double epsilon = 0.0001;

    auto start = ray.get_start();
    auto diff = glm::normalize(ray.get_diff_vec());

    double A = (diff.x * diff.x) + (diff.z * diff.z) - (diff.y * diff.y);
    double B = 2.0 * (diff.x * start.x + diff.z * start.z - diff.y * start.y);
    double C = (start.x * start.x + start.z * start.z - start.y * start.y);

    double roots[2];

    size_t num_roots = quadraticRoots(A, B, C, roots);

    if (num_roots == 0) {
        return {};
    }

    auto get_output = [&](double t) {
        // let's also calculate the u and v coordinates for texture mapping
        glm::vec3 final_point =
            ray.get_start() + (float) (t) * ray.get_diff_vec();
        double theta = glm::atan(final_point.x, final_point.z);
        glm::vec3 normal =
            glm::vec3(final_point.x, -final_point.y, final_point.z);

        return IntersectInfo {
            final_point,
            normal,
            LightingInfo { t }
        };
    };

    // first let's check if either one would have a proper height
    // since we want to bound the height
    auto check_height = [&](double t) {
        double height = (ray.get_start() + (float)(t) * ray.get_diff_vec()).y;
        return height < 0.0 && height > -1.0;
    };

    auto check_bottom = [&](double t) {
        if (t <= 0.0) return OptIntersect{};

        // has x^2 + z^2 <= 1
        glm::vec3 final_point =
            ray.get_start() + (float) (t) * ray.get_diff_vec();

        if (final_point.x * final_point.x + final_point.z * final_point.z > 1) {
            return OptIntersect{};
        }

        OptIntersect output = IntersectInfo {
            final_point,
            glm::vec3(0, -1, 0),
            LightingInfo { t }
        };
        return output;
    };

    OptIntersect min;
    double min_dist = std::numeric_limits<double>::max();

    auto update_minimum = [&](const IntersectInfo &intersect) {
        double next_dist = glm::distance(ray.get_start(), intersect.point_info);
        if (next_dist < min_dist) {
            min_dist = next_dist;
            min = intersect;
        }
    };

    // let's also intersect with the planes of y = -1
    if (glm::abs(diff.y) >= epsilon) {
        double bottom_t = (-1.0 - start.y) / diff.y;
        auto bottom = check_bottom(bottom_t);
        if (bottom.has_value()) update_minimum(*bottom);
    }

    if (num_roots == 1) {
        if (roots[0] < epsilon || !check_height(roots[0])) return min;
        update_minimum(get_output(roots[0]));
        return min;
    }

    // make sure that they both have height that's within our range
    bool zero_height = check_height(roots[0]);
    bool one_height = check_height(roots[1]);

    // if neither are safe, then stop
    if (!zero_height && !one_height) return min;

    // take the smaller of the two that's over 0
    if (roots[0] < epsilon && roots[1] < epsilon) return min;
    if (roots[0] < epsilon) {
        if (!one_height) return min;
        update_minimum(get_output(roots[1]));
        return min;
    }
    if (roots[1] < epsilon) {
        if (!zero_height) return min;
        update_minimum(get_output(roots[0]));
        return min;
    }

    if (!one_height) {
        update_minimum(get_output(roots[0]));
        return min;
    }
    if (!zero_height) {
        update_minimum(get_output(roots[1]));
        return min;
    }

    // at this point, both of them should be valid
    // return whichever one is smaller, since they're both >= 0
    IntersectInfo root_zero_out = get_output(roots[0]);
    IntersectInfo root_one_out = get_output(roots[1]);

    update_minimum(root_zero_out);
    update_minimum(root_one_out);

    return min;
}

SegmentList Cone::IntersectSegment(const Ray &ray, Material *mat) {
    double epsilon = 0.0001;

    auto start = ray.get_start();
    auto diff = glm::normalize(ray.get_diff_vec());

    double A = (diff.x * diff.x) + (diff.z * diff.z) - (diff.y * diff.y);
    double B = 2.0 * (diff.x * start.x + diff.z * start.z - diff.y * start.y);
    double C = (start.x * start.x + start.z * start.z - start.y * start.y);

    double roots[2];

    size_t num_roots = quadraticRoots(A, B, C, roots);

    if (num_roots == 0) {
        return {};
    }

    auto get_output = [&](double t) {
        // let's also calculate the u and v coordinates for texture mapping
        glm::vec3 final_point =
            ray.get_start() + (float) (t) * ray.get_diff_vec();
        double theta = glm::atan(final_point.x, final_point.z);
        glm::vec3 normal =
            glm::vec3(final_point.x, -final_point.y, final_point.z);

        return IntersectInfo {
            final_point,
            normal,
            LightingInfo { t, 0, 0, std::nullopt, mat }
        };
    };

    // first let's check if either one would have a proper height
    // since we want to bound the height
    auto check_height = [&](double t) {
        double height = (ray.get_start() + (float)(t) * ray.get_diff_vec()).y;
        return height < 0.0 && height > -1.0;
    };

    auto check_bottom = [&](double t) {
        if (t <= 0.0) return OptIntersect{};

        // has x^2 + z^2 <= 1
        glm::vec3 final_point =
            ray.get_start() + (float) (t) * ray.get_diff_vec();

        if (final_point.x * final_point.x + final_point.z * final_point.z > 1) {
            return OptIntersect{};
        }

        OptIntersect output = IntersectInfo {
            final_point,
            glm::vec3(0, -1, 0),
            LightingInfo { t, 0, 0, std::nullopt, mat }
        };
        return output;
    };

    OptIntersect first = std::nullopt;
    OptIntersect second = std::nullopt;
    double min_dist = std::numeric_limits<double>::max();
    double second_min_dist = std::numeric_limits<double>::max();

    auto update_minimum = [&](const IntersectInfo &intersect) {
        double next_dist = glm::distance(ray.get_start(), intersect.point_info);
        if (next_dist < min_dist) {
            second = first; // update the second one first
            second_min_dist = min_dist;
            first = intersect;
            min_dist = next_dist;
        } else if (next_dist < second_min_dist) {
            // only update second
            second = intersect;
            second_min_dist = next_dist;
        }
    };

    auto get_final_seg = [&]() {
        if (!first || !second) return SegmentList();

        SegmentList output;
        output.push_back(Segment { *first, *second });
        return output;
    };

    // let's also intersect with the planes of y = -1
    if (glm::abs(diff.y) >= epsilon) {
        double bottom_t = (-1.0 - start.y) / diff.y;
        auto bottom = check_bottom(bottom_t);
        if (bottom.has_value()) update_minimum(*bottom);
    }

    if (num_roots == 1) {
        if (roots[0] < -epsilon || !check_height(roots[0])) return get_final_seg();
        update_minimum(get_output(roots[0]));
        return get_final_seg();
    }

    // make sure that they both have height that's within our range
    bool zero_height = check_height(roots[0]);
    bool one_height = check_height(roots[1]);

    // if neither are safe, then stop
    if (!zero_height && !one_height) return get_final_seg();

    // take the smaller of the two that's over 0
    if (roots[0] < -epsilon && roots[1] < -epsilon) return get_final_seg();
    if (roots[0] < -epsilon) {
        if (!one_height) return get_final_seg();
        update_minimum(get_output(roots[1]));
        return get_final_seg();
    }
    if (roots[1] < -epsilon) {
        if (!zero_height) return get_final_seg();
        update_minimum(get_output(roots[0]));
        return get_final_seg();
    }

    if (!one_height) {
        update_minimum(get_output(roots[0]));
        return get_final_seg();
    }
    if (!zero_height) {
        update_minimum(get_output(roots[1]));
        return get_final_seg();
    }

    // at this point, both of them should be valid
    // return whichever one is smaller, since they're both >= 0
    IntersectInfo root_zero_out = get_output(roots[0]);
    IntersectInfo root_one_out = get_output(roots[1]);

    update_minimum(root_zero_out);
    update_minimum(root_one_out);

    return get_final_seg();
}

Cone::~Cone() {}


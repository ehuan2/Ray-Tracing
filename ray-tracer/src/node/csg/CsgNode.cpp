// Termm--Fall 2023
#include "CsgNode.hpp"
#include <iostream>
using namespace std;

// Constructors -------------------------------------------------------------------------
CsgNode::CsgNode(
	const std::string & name,
    GeometryNode *left_ptr,
    GeometryNode *right_ptr,
    CsgOperation op
) : GeometryNode ( name, nullptr, nullptr ),
    left_ptr(left_ptr), right_ptr(right_ptr),
    op(op)
{}

// right now, we'll return nothing
OptIntersect CsgNode::IntersectRay(const Ray &ray) {
    auto segment_list = IntersectSegment(ray);
    if (segment_list.size() == 0) return {};
    
    // given a segment list, we want to return whichever one
    // had the smallest intersection point
    // we'll enforce that IntersectSegment must return
    // the segments in order of closest intersection point
    return segment_list[0].closer;
}

glm::vec3 apply_transformation(GeometryNode *node, const glm::vec3 &vec) {
    return glm::vec3(node->get_transform() * glm::vec4(vec.x, vec.y, vec.z, 1.0));
}

glm::vec3 apply_inverse_transpose(GeometryNode *node, const glm::vec3 &normal) {
    return glm::mat3(glm::transpose( node->get_inverse() )) * normal;
}

// a function that will help propagate the segment back up the tree
SegmentList propagate_up(GeometryNode *node, const SegmentList &seg_list) {
    // redo the transformation as we go up
    // also do the normal transform too
    SegmentList output;
    for (auto &seg : seg_list) {
        output.push_back(Segment {
            IntersectInfo {
                apply_transformation(node, seg.closer.point_info),
                apply_inverse_transpose(node, seg.closer.normal),
                seg.closer.lighting_info
            },
            IntersectInfo {
                apply_transformation(node, seg.further.point_info),
                apply_inverse_transpose(node, seg.further.normal),
                seg.further.lighting_info
            }
        });
    }
    return output;
}

std::pair<double, double> get_dist_values(const Ray &ray, const Segment &seg) {
    return make_pair(
        glm::distance(ray.get_start(), seg.closer.point_info),
        glm::distance(ray.get_start(), seg.further.point_info)
    );
}

bool segment_has_intersection(
    const Ray &ray,
    const Segment &left_seg,
    const Segment &right_seg
) {
    auto left_ts = get_dist_values(ray, left_seg);
    auto right_ts = get_dist_values(ray, right_seg);

    // if left starts before right, then return whether left ends after right starts
    if (left_ts.first < right_ts.first) {
        return left_ts.second > right_ts.first;
    }
    // if left starts after right, then return whether right ends after left starts
    return right_ts.second > left_ts.first;
}

typedef std::optional<Segment> OptSegment;

// Helper functions --------------------------------------------------------------------------------
// helper function that helps decide whether a segment contains another
bool a_contains_b(std::pair<double, double> a, std::pair<double, double> b) {
    return a.first <= b.first && b.second <= a.second;
}

// helper function that tells us if something is strictly less than something else
bool a_less_than_b(const Ray &ray, const Segment &a, const Segment &b) {
    auto left_ts = get_dist_values(ray, a);
    auto right_ts = get_dist_values(ray, b);
    return left_ts.first < right_ts.first;
}

// helper function that tells us whether a is less than b and has no intersection
bool a_strictly_less_than(const Ray &ray, const Segment &a, const Segment &b) {
    return a_less_than_b(ray, a, b) && segment_has_intersection(ray, a, b);
}

bool a_ends_first(const Ray &ray, const Segment &a, const Segment &b) {
    auto left_ts = get_dist_values(ray, a);
    auto right_ts = get_dist_values(ray, b);
    return left_ts.second < right_ts.second;
}

// Difference --------------------------------------------------------------------------------------
SegmentList segment_diff(const Ray &ray, const Segment &left_seg, const Segment &right_seg) {
    // left - right
    auto left_ts = get_dist_values(ray, left_seg);
    auto right_ts = get_dist_values(ray, right_seg);

    // if the right contains the left, then return empty
    if (a_contains_b(right_ts, left_ts)) {
        return SegmentList{};
    }

    auto get_closer = [&]() {
        return Segment {
            left_seg.closer,
            IntersectInfo {
                right_seg.closer.point_info,
                -right_seg.closer.normal,
                left_seg.closer.lighting_info
            }
        };
    };

    auto get_further = [&]() {
        return Segment {
            IntersectInfo {
                right_seg.further.point_info,
                -right_seg.further.normal,
                left_seg.further.lighting_info
            },
            left_seg.further
        };
    };

    // know it strictly contains, therefore, we need to create two
    // different segments
    if (a_contains_b(left_ts, right_ts)) {
        SegmentList output;
        // left first to right first
        output.push_back(get_closer());
        output.push_back(get_further());
        return output;
    }

    // final case, neither contains each other, only one segment
    if (left_ts.first < right_ts.first) {
        SegmentList output;
        // for intersection, we want to take the further of the closer one
        // and the closer of the further one
        output.push_back(get_closer());
        return output;
    }

    SegmentList output;
    output.push_back(get_further());
    return output;
}

SegmentList segment_minus_list(
    const Ray &ray,
    const Segment &left_seg,
    const SegmentList &right_seg
) {
    SegmentList leftover;
    leftover.push_back(left_seg);

    size_t right_ptr = 0;
    while (right_ptr < right_seg.size()) {
        // we keep incrementing until we have an intersection
        while (
            right_ptr < right_seg.size() &&
            !segment_has_intersection(ray, left_seg, right_seg[right_ptr])
        ) {
            // increment the left if that's the one that's smaller
            if (a_less_than_b(ray, left_seg, right_seg[right_ptr])) {
                // add it cause it doesn't intersect with any right
                break;
            } else {
                // increment right if that's the one that's smaller
                ++right_ptr;
            }
        }

        // stop processing, nothing left to process
        if (a_less_than_b(ray, left_seg, right_seg[right_ptr])) {
            break;
        }

        // now we just take all of the segments in leftover
        // and subtract them by right_seg
        SegmentList diff;
        for (auto &seg : leftover) {
            if (!segment_has_intersection(ray, seg, right_seg[right_ptr])) {
                diff.push_back(seg);
                continue;
            }
            auto diff_result = segment_diff(ray, seg, right_seg[right_ptr]);

            for (auto &next_seg : diff_result) {
                diff.push_back(next_seg);
            }
        }

        leftover = diff;
        ++right_ptr;
    }
    return leftover;
}

SegmentList segment_list_diff(
    const Ray &ray,
    const SegmentList &left_seg,
    const SegmentList &right_seg
) {
    if (left_seg.size() == 0) return SegmentList();
    if (right_seg.size() == 0) return left_seg;

    // check if they have an intersection, if not just return left
    SegmentList output;
    size_t left_ptr = 0;

    // continue iterating over all segments
    // we want to iterate over left_segment primarily
    while (left_ptr < left_seg.size()) {
        SegmentList diff = segment_minus_list(ray, left_seg[left_ptr], right_seg); 
        for (auto &seg : diff) {
            output.push_back(seg);
        }
        ++left_ptr;
    }

    // now they all have intersections
    return output;
}

// Union -------------------------------------------------------------------------------------------
// only call this if they have an intersection
Segment segment_union(const Ray &ray, const Segment &left_seg, const Segment &right_seg) {
    auto left_ts = get_dist_values(ray, left_seg);
    auto right_ts = get_dist_values(ray, right_seg);

    auto closer_segment =
        left_ts.first < right_ts.first ?
            left_seg.closer : right_seg.closer;

    auto further_segment =
        left_ts.second < right_ts.second ?
            left_seg.further : right_seg.further;

    return Segment {
        closer_segment,
        further_segment
    };
}

SegmentList segment_list_union(
    const Ray &ray,
    const SegmentList &left_seg,
    const SegmentList &right_seg
) {
    // given two segments, we need to find their union
    // also needs to return everything in order of closest to furthest

    // check if they both exist
    if (left_seg.size() == 0 && right_seg.size() == 0) return SegmentList();
    if (left_seg.size() == 0) return right_seg;
    if (right_seg.size() == 0) return left_seg;

    // go over all the segments, and union over all of them
    SegmentList output;
    size_t left_ptr = 0;
    size_t right_ptr = 0;

    // continue iterating over all segments
    while (left_ptr < left_seg.size() || right_ptr < right_seg.size()) {
        // check that it's within bounds, if not just add the rest
        if (left_ptr >= left_seg.size()) {
            while (right_ptr < right_seg.size()) {
                output.push_back(right_seg[right_ptr]);
                ++right_ptr;
            }
            // stop now
            break;
        }

        if (right_ptr >= right_seg.size()) {
            while (left_ptr < left_seg.size()) {
                output.push_back(left_seg[left_ptr]);
                ++left_ptr;
            }
            // stop now
            break;
        }

        // we keep incrementing until we have an intersection
        while (
            left_ptr < left_seg.size() && right_ptr < right_seg.size() &&
            !segment_has_intersection(ray, left_seg[left_ptr], right_seg[right_ptr])
        ) {
            // increment the left if that's the one that's smaller
            if (a_less_than_b(ray, left_seg[left_ptr], right_seg[right_ptr])) {
                output.push_back(left_seg[left_ptr]);
                ++left_ptr;
            } else {
                // increment right if that's the one that's smaller
                output.push_back(right_seg[right_ptr]);
                ++right_ptr;
            }
        }

        // check if there are any out of bounds
        if (left_ptr >= left_seg.size()) {
            while (right_ptr < right_seg.size()) {
                output.push_back(right_seg[right_ptr]);
                ++right_ptr;
            }
            // stop now
            break;
        }

        if (right_ptr >= right_seg.size()) {
            while (left_ptr < left_seg.size()) {
                output.push_back(left_seg[left_ptr]);
                ++left_ptr;
            }
            // stop now
            break;
        }

        // now that we know they intersect, we now do the intersection
        // except we keep unioning until they don't have an intersection any more
        Segment to_union = segment_union(ray, left_seg[left_ptr], right_seg[right_ptr]); 

        // because we've included both of them, we just need to increment both
        ++left_ptr;
        ++right_ptr;

        // continue incrementing until the segments no longer intersect
        while (
            (left_ptr < left_seg.size()
                && segment_has_intersection(ray, to_union, left_seg[left_ptr])) ||
            (right_ptr < right_seg.size()
                && segment_has_intersection(ray, to_union, right_seg[right_ptr]))
        ) {
            if (right_ptr < right_seg.size() &&
                    segment_has_intersection(ray, to_union, right_seg[right_ptr])) {
                to_union = segment_union(ray, to_union, right_seg[right_ptr]);
                ++right_ptr;
            } else if (left_ptr < left_seg.size() &&
                    segment_has_intersection(ray, to_union, left_seg[left_ptr])) {
                to_union = segment_union(ray, to_union, left_seg[left_ptr]);
                ++left_ptr;
            }
        }

        output.push_back(to_union);
    }

    return output;
}

// Intersection ------------------------------------------------------------------------------------
Segment segment_intersect(const Ray &ray, const Segment &left_seg, const Segment &right_seg) {
   
    std::pair<double, double> left_ts = get_dist_values(ray, left_seg);
    std::pair<double, double> right_ts = get_dist_values(ray, right_seg);

    // for intersection, take the further "closer" value as the start
    // but then take the minimum over both furthers
    IntersectInfo closer = left_ts.first < right_ts.first ? right_seg.closer : left_seg.closer;
    IntersectInfo further = left_ts.second < right_ts.second ? left_seg.further : right_seg.further;

    return Segment {
        closer,
        further
    };
}

SegmentList segment_list_intersect(
    const Ray &ray,
    const SegmentList &left_seg,
    const SegmentList &right_seg
) {
    // if we try intersecting and get nothing...
    if (left_seg.size() == 0 || right_seg.size() == 0) return SegmentList();

    // assume that:
    // 1. Both segment lists are disjoint segments (within a list)
    // 2. Both segment lists are in sorted order
    SegmentList output;

    size_t left_ptr = 0;
    size_t right_ptr = 0;

    // continue intersecting while they're both within range
    while (left_ptr < left_seg.size() && right_ptr < right_seg.size()) {
        // now we need to increment either the left or right ptr depending on
        // which one is further behind

        // we keep incrementing until we have an intersection, or until we can't
        while (
            left_ptr < left_seg.size() && right_ptr < right_seg.size() &&
            !segment_has_intersection(ray, left_seg[left_ptr], right_seg[right_ptr])
        ) {
            // increment the left if that's the one that's smaller
            if (a_less_than_b(ray, left_seg[left_ptr], right_seg[right_ptr])) ++left_ptr;
            // increment rigth if that's the one that's smaller
            else ++right_ptr;
        }

        // stop if it's out of bounds
        if (left_ptr >= left_seg.size() || right_ptr >= right_seg.size()) {
            break;
        }

        // now that we know they intersect, we now do the intersection
        auto seg = segment_intersect(ray, left_seg[left_ptr], right_seg[right_ptr]);
        output.push_back(seg);

        // now we increment left or right depending on which one ends first
        // if the left ends first, then we need to increment left
        // if the right ends first, we need to increment right
        if (a_ends_first(ray, left_seg[left_ptr], right_seg[right_ptr])) ++left_ptr;
        else ++right_ptr;
    }

    return output;
}

SegmentList CsgNode::IntersectSegment(const Ray &ray) {
    if (!left_ptr || !right_ptr) {
        cout << "incorrect csg node, no left or no right pointer" << endl;
        exit(1);
    }
    // undo the transformation of the ray as we go down
    Ray left_ray = ray.transform(left_ptr->get_inverse()).normalize();
    SegmentList left_seg = propagate_up(
        left_ptr, left_ptr->IntersectSegment(left_ray)
    );

    Ray right_ray = ray.transform(right_ptr->get_inverse()).normalize();
    SegmentList right_seg = propagate_up(
        right_ptr, right_ptr->IntersectSegment(right_ray)
    );

    // now that we know both segments must exist, we need to do some sort of op on them
    if (op == CsgOperation::CsgUnion) {
        return segment_list_union(ray, left_seg, right_seg);
    } else if (op == CsgOperation::CsgDiff) {
        return segment_list_diff(ray, left_seg, right_seg);
    }
    return segment_list_intersect(ray, left_seg, right_seg);
}


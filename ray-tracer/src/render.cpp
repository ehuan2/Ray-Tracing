// Termm--Fall 2023

#include <glm/ext.hpp>
#include <vector>
#include <random>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>

#include "MathUtils.hpp"
#include "render.hpp"
#include "tools/traverse.hpp"
#include "node/tree/LSystemTree.hpp"

using namespace std;

// Intersection portion -------------------------------------------------------------------------------
// helper functions to help apply a transformation (needed for vec3)
    // applies it to a point
glm::vec3 apply_transformation(SceneNode *root, const glm::vec3 &vec) {
    return glm::vec3(root->get_transform() * glm::vec4(vec.x, vec.y, vec.z, 1.0));
}
// helper function to apply an inverse transpose transformation to a normal
glm::vec3 apply_inverse_transpose(SceneNode *root, const glm::vec3 &normal) {
    return glm::mat3(glm::transpose(root->get_inverse())) * normal;
}

// this function will intersect the ray with the scene to get what we want
std::pair<OptIntersect, GeometryNode *> MinIntersect(
    const Ray &ray,
    SceneNode *root,
    SceneNode *no_intersect
) {
    // intersect with whatever is the smallest distance
    OptIntersect min_intersect {};
    double min_distance = -1.0;
    GeometryNode *to_intersect_with = nullptr;

    auto intersections = Intersect(ray, root, glm::mat4(1.0), no_intersect);
    for (auto &intersection : intersections) { 
        // only intersect if there is an intersection
        if (!intersection.first.has_value()) continue;

        double next_dist = glm::distance(
            intersection.first->point_info,
            ray.get_start()
        );

        if (!min_intersect.has_value() || next_dist < min_distance) {
            to_intersect_with = intersection.second;
            min_intersect = intersection.first;
            min_distance = next_dist;
        }
    }

    return make_pair(min_intersect, to_intersect_with);
}

// this function will intersect the ray with the scene to get what we want
vector<Intersection> Intersect(
    const Ray &ray,
    SceneNode *root,
    const glm::mat4 &inv_trans,
    SceneNode *no_intersect
) {
    // a helper function that returns a transformed intersect info:
    auto get_intersect_info = [&](const OptIntersect &intersect) {
        return IntersectInfo {
            // reapply the transformation!
            apply_transformation(root, intersect->point_info),
            // apply the inverse transitive
            apply_inverse_transpose(root, intersect->normal),
            intersect->lighting_info
        };
    };

    vector<Intersection> output;

    // apply parent inverse first
    glm::mat4 new_inv = root->get_inverse() * inv_trans;

    // we check to make sure we only intersect with what we want
    bool is_valid = (!no_intersect || root->m_name != no_intersect->m_name);

    // if it's a geometry node, check for an intersection
    if (root->m_nodeType == NodeType::GeometryNode && is_valid) {
        // used to do the distance calcs + transforming nodes
        GeometryNode *geom_node = static_cast<GeometryNode *>(root);

        Ray new_ray = ray.transform(new_inv).normalize();
        OptIntersect intersect = geom_node->IntersectRay(new_ray);

        // if we do have a value, get it
        if (intersect.has_value()) {
            output.push_back(make_pair(
                get_intersect_info(intersect),
                geom_node
            ));
        }
    }

    // recurse through the entire tree and then find the closest intersection
    for (auto &child : root->children) {
        vector<Intersection> child_intersections;

        // if it's not a geometry node, continue to go down the scene
        child_intersections = Intersect(ray, child, new_inv, no_intersect);

        for (auto &intersect : child_intersections) {
            if (!intersect.first.has_value()) continue;
            output.push_back(make_pair (
                get_intersect_info(intersect.first),
                intersect.second
            ));
        }
    }

    return output;
}

// Lighting stuff -------------------------------------------------------------------------------------
glm::vec3 get_phong_lighting(
    const Ray &ray,
    GeometryNode *geom_node,
    const IntersectInfo &intersect,
    const glm::vec3 &ambient,
    const std::list<Light *> &lights,
    SceneNode *root
) {
    glm::vec3 m_kd = geom_node->get_kd(intersect);
    glm::vec3 m_ks = geom_node->get_ks(intersect);
    double m_shininess = geom_node->get_shininess(intersect);

    // k_a * I_a - ambient light
    glm::vec3 ambient_light = ambient * m_kd;

    // then sum over all of the light point sources as well:
    glm::vec3 diffuse_light (0.0);
    glm::vec3 specular_reflection (0.0);

    for (auto &light : lights) {
        // don't give contribution from a light if it doesn't hit it
        if (has_shadows(light->position, intersect.point_info, root, geom_node)) continue;

        // normalize the light and the normal vectors
        glm::vec3 l = glm::normalize(light->position - intersect.point_info);

        glm::vec3 normal = glm::normalize(intersect.normal);

        float dist = glm::distance(l, intersect.point_info);
        float dist_adjustment = 1.0 / (light->falloff[0] + light->falloff[1] * dist + light->falloff[2] * dist * dist);

        float diffuse_dot = glm::dot(l, normal);

        // only give contribution if the dot product would actually be visible
        if (diffuse_dot < 0) continue;

        diffuse_light = diffuse_light + (m_kd * (diffuse_dot * dist_adjustment) * light->colour);

        glm::vec3 r = glm::normalize(-l + 2.0f * glm::dot(l, normal) * dist_adjustment * normal);
        glm::vec3 eye = glm::normalize(ray.get_start() - intersect.point_info);

        float specular_dot = glm::dot(r, eye);
        if (specular_dot < 0) continue;

        specular_reflection = specular_reflection + (m_ks * glm::pow(specular_dot, (float)m_shininess) * light->colour);
    }

    return ambient_light + diffuse_light + specular_reflection;
}

bool has_shadows(
    const glm::vec3 &light,
    const glm::vec3 &intersect_point,
    SceneNode *root,
    GeometryNode *geom_node
) {
    Ray ray (intersect_point, light);
    return MinIntersect(ray, root, geom_node).first.has_value();
}

// a recursive ray tracer!
std::optional<glm::vec3> recurse_ray_trace(
    const Ray &ray,
    SceneNode *root,
    SceneNode *no_intersect,
    // Lighting parameters  
    const glm::vec3 & ambient,
    const std::list<Light *> & lights,
    int depth
) {
    // first we intersect the ray with the scene, using the no_intersect
    auto intersect_result = MinIntersect( ray, root, no_intersect );

    OptIntersect opt_intersect = intersect_result.first;
    GeometryNode *geom_node = intersect_result.second;

    // if it has a value return it
    if (!opt_intersect.has_value() || geom_node == nullptr) return std::nullopt;

    // now we get the phong lighting, and add that to the output
    glm::vec3 lighting = get_phong_lighting(
        ray, geom_node, opt_intersect.value(), ambient, lights, root
    );

    // get the reflectiveness, if it's 0, stop doing more than needed
    double reflectiveness = geom_node->get_reflective(opt_intersect.value());

    // if it's the max depth, just return the lighting as is
    if (depth >= MAX_RAY_RECURSE_DEPTH || reflectiveness == 0.0) return lighting;

    // time to calculate the mirror ray
    // calculate the end point based off of the normal:

    // 1. calculate the theta based off of the diff vec and the normal
    glm::vec3 normal = opt_intersect->normal;
    glm::vec3 vec = ray.get_diff_vec();

    // use the formula found from stanford, find the reflections ray direction
    glm::vec3 reflect_ray = vec - 2 * (glm::dot(normal, vec) * normal);
    // add the reflective ray distance to the intersection point
    glm::vec3 output_point = opt_intersect->point_info + reflect_ray;
    Ray next_ray = Ray(opt_intersect->point_info, output_point);

    // now we recurse on the next one
    auto next_ray_lighting =
        recurse_ray_trace(next_ray, root, geom_node, ambient, lights, depth + 1);

    if (!next_ray_lighting.has_value()) return lighting;

    return (1.0 - reflectiveness) * lighting +
        (reflectiveness) * next_ray_lighting.value();
}

// ----------------------------------------------------------------------------------------------------
// x and y are a ratio of how much of the scene is drawn
glm::vec3 get_background (uint x, uint y, size_t w, size_t h) {
    // let's do a gradient
    return glm::vec3(0.5, 0.3, 1.0 - ((double)y / (double)h));
}

// Anti-aliasing --------------------------------------------------------------------------------------
glm::vec3 shootAntiAliasingRays(
    SceneNode *root,
    const glm::vec3 &eye,
    uint x,
    uint y,
    size_t w,
    size_t h,
    double pixel_height,
    const glm::vec3 &x_norm,
    const glm::vec3 &y_norm,
    const glm::vec3 &z_norm,
    mt19937 &rng,
    // Lighting parameters  
    const glm::vec3 & ambient,
    const std::list<Light *> & lights
) {
    double aspect = (double) w / (double) h;

    double pixel_width = aspect * pixel_height;

    glm::vec3 x_part =
        (- aspect * pixel_height + x * 2 * aspect * pixel_height / (double) w) * x_norm;
    glm::vec3 y_part = (- pixel_height + y * 2 * pixel_height / (double) h) * y_norm;

    double single_pixel_width = pixel_width * 2 * aspect / (double) w;
    double single_pixel_height = pixel_height * 2 / (double) h;

    glm::vec3 output = glm::vec3(0, 0, 0);

    uint32_t fine_grain = 5;
    std::uniform_int_distribution<> int_dist(-fine_grain, fine_grain);

    // for anti-aliasing, we will want to shoot 9 different rays:
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) {
            // change the height by a little bit

            auto x_rand = int_dist(rng);
            auto y_rand = int_dist(rng);

            // normalize over -0.5 and 0.5
            double x_contribution = ((float)x_rand / (float)(2 * fine_grain + 1));
            double y_contribution = ((float)y_rand / (float)(2 * fine_grain + 1));

            glm::vec3 x_jitter =
                x_norm * ((float)(single_pixel_width) / 3.0 * (x_contribution + (float)i));
            glm::vec3 y_jitter =
                y_norm * ((float)(single_pixel_height) / 3.0 * (y_contribution + (float)j));

            glm::vec3 end_point = eye + x_part + y_part + z_norm + x_jitter + y_jitter;

            Ray ray_intersect = Ray( eye, end_point );
            auto result = recurse_ray_trace( ray_intersect, root, nullptr, ambient, lights, 0 );
            if (!result.has_value()) {
                // add background colour
                output += get_background(x, y, w, h) / 9.0;
            } else {
                // add the colour itself
                output += result.value() / 9.0;
            }
        }
    }

    return output;
}

// Non anti-aliasing --------------------------------------------------------------------------------------
glm::vec3 shootRays(
    SceneNode *root,
    const glm::vec3 &eye,
    uint x,
    uint y,
    size_t w,
    size_t h,
    double pixel_height,
    const glm::vec3 &x_norm,
    const glm::vec3 &y_norm,
    const glm::vec3 &z_norm,
    // Lighting parameters  
    const glm::vec3 & ambient,
    const std::list<Light *> & lights
) {
    double aspect = (double) w / (double) h;

    double pixel_width = aspect * pixel_height;

    glm::vec3 x_part = (- aspect * pixel_height + x * 2 * aspect * pixel_height / (double) w) * x_norm;
    glm::vec3 y_part = (- pixel_height + y * 2 * pixel_height / (double) h) * y_norm;

    glm::vec3 output = glm::vec3(0, 0, 0);

    glm::vec3 end_point = eye + x_part + y_part + z_norm;

    Ray ray_intersect = Ray( eye, end_point );
    auto result = recurse_ray_trace( ray_intersect, root, nullptr, ambient, lights, 0 );
    if (result.has_value()) return result.value();
    return get_background(x, y, w, h);
}

// Render functions -----------------------------------------------------------------------------------

// this function renders the image from y_min to y_max range
void render_y_range(
    uint y_min,
    uint y_max,
    SceneNode *root,
    const glm::vec3 &eye,
    size_t w,
    size_t h,
    double pixel_height,
    const glm::vec3 &x_norm,
    const glm::vec3 &y_norm,
    const glm::vec3 &z_norm,
    // Lighting parameters  
    const glm::vec3 & ambient,
    const std::list<Light *> & lights,
    Image &image
) {
    // Anti-aliasing, create a different random number generator per thread
    mt19937 rng;
    rng.seed(12345);
#ifndef USE_ANTI_ALIASING
    bool anti_alias = false;
#else
    bool anti_alias = true;
#endif

    for (uint y = y_min; y < y_max; ++y) {
#ifndef SINGLE_PIXEL_DEBUG
#ifndef USE_THREADING
#ifdef PRINT_PROGRESS
        cout << 100.0 * (float)(y) / float(h) << "\%" << endl;
#endif
#endif
#endif
		for (uint x = 0; x < w; ++x) {
#ifdef SINGLE_PIXEL_DEBUG
            int grid_size = 1;
            if (!((x >= w / 2 - grid_size && x <= w/2 + grid_size) && (y >= h / 2 - grid_size && y <= h / 2 + grid_size))) continue;
#endif

            glm::vec3 pixel_colour = anti_alias ?
                shootAntiAliasingRays(
                    root, eye, x, y, w, h, pixel_height, x_norm, y_norm, z_norm, rng, ambient, lights
                ) :
                shootRays(
                    root, eye, x, y, w, h, pixel_height, x_norm, y_norm, z_norm, ambient, lights
                );

            // we invert the y's because we want to draw it right side up
            image(x, h - y - 1, 0) = pixel_colour[0];
            image(x, h - y - 1, 1) = pixel_colour[1];
            image(x, h - y - 1, 2) = pixel_colour[2];
        }
    }
}

void A4_Render(
		// What to render  
		SceneNode * root,

		// Image to write to, set to a given width and height  
		Image & image,

		// Viewing parameters  
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {
    std::cout << "F23: Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;

	size_t h = image.height();
	size_t w = image.width();

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			// Red: 
			image(x, y, 0) = (double)1.0;
			// Green: 
			image(x, y, 1) = (double)1.0;
			// Blue: 
			image(x, y, 2) = (double)1.0;
		}
	}

    // find the x normalized direction as well as the y normalized direction
    glm::vec3 x_norm = glm::normalize(glm::cross(view, up));
    glm::vec3 y_norm = glm::normalize(up);
    glm::vec3 z_norm = glm::normalize(view);

    double pixel_height = glm::tan(degreesToRadians(fovy / 2.0));

    // we keep trakc of the time that we want
    auto start_time = std::chrono::system_clock::now();

#ifdef USE_THREADING
    // now we use threads to split it up!
    int batch_size = 2; // how big the size of each column that each thread will go through
    int batch_total = h / batch_size + (h % batch_size == 0 ? 0 : 1); // round up the total number of batches

    std::atomic<int> batch_num = 0;
    std::mutex batch_num_mutex;
 
    // now we use threads to split it up!
    int num_threads = 16;
    vector<std::thread> threads;

    // running threads
    auto thread_runner = [&] () {
        uint saved_batch_num;
        batch_num_mutex.lock();
            saved_batch_num = batch_num;
            batch_num += 1;
#ifdef PRINT_PROGRESS
            cout << (float)saved_batch_num / (float)batch_total * 100.0 << "\%" << endl;
#endif
        batch_num_mutex.unlock();

        while(saved_batch_num < batch_total) {
            uint y_min = saved_batch_num * batch_size;
            uint y_max = (saved_batch_num + 1) * batch_size;
            if (saved_batch_num + 1 == batch_total) {
                y_max = h;
            }

            render_y_range(
                y_min, y_max,
                root, eye, w, h, pixel_height,
                x_norm, y_norm, z_norm, ambient, lights, std::ref(image)
            );

            batch_num_mutex.lock();
                saved_batch_num = batch_num;
                if (batch_num != batch_total) {
                    batch_num += 1;
#ifdef PRINT_PROGRESS
                    cout << (float)saved_batch_num / (float)batch_total * 100.0 << "\%" << endl;
#endif
                }
            batch_num_mutex.unlock();
        }
    };

    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(std::thread(thread_runner));
    }
    for (auto &th : threads) th.join();

#else
    render_y_range(
        0, h, root, eye, w, h, pixel_height,
        x_norm, y_norm, z_norm, ambient, lights, image
    );
#endif

    auto end_time = std::chrono::system_clock::now();
    std::chrono::duration<double> seconds = end_time - start_time;

    cout << "Time to render: " << seconds.count() << " seconds" << endl;
}


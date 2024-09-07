#include "Texture.hpp"
#include <lodepng/lodepng.h>
#include <iostream>
using namespace std;

// texture mapping stuff
Texture::Texture(const std::string &fname) {
    // here we'll use lodepng to get the image info
    // on load we want to decode this into a char stream for us to use
    unsigned error = lodepng::decode(image, width, height, fname);
    if (error) {
        std::cerr << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        // exit on error
        exit(1);
    }

    // now we saved it all into image as we wanted, with 4 bytes per pixel
}

// u and v must be between 0 and 1
// gets the texture's colour info at that location
glm::vec3 Texture::getTextureColour(double u, double v) {
    // here we'll get the texture's colour depending on the image field
    // as well as u and v

    // now depending on the u and v, we'll get the image location
    // basically, width * y + x is the index we want
    unsigned index = 4 * width * (unsigned)((height - 1) * v) + 4 * (unsigned)((width - 1) * u);

    unsigned red = image.at(index);
    unsigned blue = image.at(index + 1);
    unsigned green = image.at(index + 2);

    auto scale_down = [](unsigned rgb) {
        return (float)rgb / 255.0;
    };

    auto result = glm::vec3(scale_down(red), scale_down(green), scale_down(blue));
    return result;
}

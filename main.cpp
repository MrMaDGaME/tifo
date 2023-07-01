#include <iostream>
#include "image/image.hh"

#define DETECTION_THRESHOLD 0.5

int main() {
    std::cout << "loading image..." << std::endl;
    Image image("../inputs/redbone.png");

    std::cout << "detecting lines..." << std::endl;
    image.align(DETECTION_THRESHOLD);

    std::cout << "removing lines..." << std::endl;
    image.removeHorizontalLines(DETECTION_THRESHOLD);

    std::cout << "transforming image..." << std::endl;
    image.close(4);

    std::cout << "saving image..." << std::endl;
    image.to_ppm("../results/redbone.ppm");
    return 0;
}

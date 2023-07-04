#include <iostream>
#include "image/image.hh"

#define DETECTION_THRESHOLD 0.5
#define BIN_TRESHOLD 200

int main() {
    std::cout << "loading image..." << std::endl;
    Image image("../inputs/test2.png", BIN_TRESHOLD);

    std::cout << "detecting lines..." << std::endl;
    image.align(DETECTION_THRESHOLD);

    std::cout << "removing lines..." << std::endl;
    image.removeHorizontalLines(DETECTION_THRESHOLD);

    std::cout << "transforming image..." << std::endl;
    image.close(4);

    std::cout << "saving image..." << std::endl;
    image.to_ppm("../results/test2.ppm");
    return 0;
}

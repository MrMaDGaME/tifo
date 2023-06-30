#include <iostream>
#include "image/image.hh"

int main() {
    std::cout << "loading image..." << std::endl;
    Image image("../inputs/come_and_get_your_love.png");

    image.removeHorizontalLines(0.5);
    image.dilate();
    image.erode();

    std::cout << "saving image..." << std::endl;
    image.to_ppm("../results/come_and_get_your_love.ppm");

    return 0;
}

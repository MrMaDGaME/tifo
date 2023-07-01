#pragma once

#include <iostream>
#include <png.h>
#include <sstream>
#include <fstream>

class Image {
public:

    struct Line {
        int rho;
        int theta;
    };

    Image();

    explicit Image(const std::string &path);

    Image(int width, int height);

    ~Image();

    int getWidth() const;

    int getHeight() const;

    bool getPixel(int x, int y) const;

    void setPixel(int x, int y, bool value);

    void to_ppm(const std::string &path) const;

    std::vector<int> detectHorizontalLines(float threshold = 0.5) const;

    void removeHorizontalLines(float threshold = 0.5);

    void dilate(int neighborhoodSize);

    void erode(int neighborhoodSize);

    void open(int neighborhoodSize);

    void close(int neighborhoodSize);

    // Create a new image with a rotation of angle degrees
    Image *rotate(int degrees) const;

    void align(float threshold) const;

protected:
    int width;
    int height;
    bool *binaryPixels;
};

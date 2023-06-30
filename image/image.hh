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

    explicit Image(const std::string &path);

    ~Image();

    int getWidth() const;

    int getHeight() const;

    bool getPixel(int x, int y) const;

    void setPixel(int x, int y, bool value);

    void to_ppm(const std::string &path) const;

    std::vector<int> detectHorizontalLines(float threshold = 0.5);

    void removeHorizontalLines(float threshold = 0.5);

    void dilate();

    void erode();

protected:
    int width;
    int height;
    bool *binaryPixels;
};

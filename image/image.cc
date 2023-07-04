#include "image.hh"

Image::Image() : width(0), height(0), binaryPixels(nullptr) {}

Image::Image(const std::string &path, int threshold) {

    // Ouvrir le fichier image PNG en lecture
    FILE *file = fopen(path.c_str(), "rb");
    if (!file) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier image." << std::endl;
        return;
    }

    // Initialiser la structure png pour lire l'image
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        std::cerr << "Erreur : Impossible de créer la structure de lecture PNG." << std::endl;
        fclose(file);
        return;
    }

    // Initialiser la structure d'information de l'image PNG
    png_infop info = png_create_info_struct(png);
    if (!info) {
        std::cerr << "Erreur : Impossible de créer la structure d'information PNG." << std::endl;
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(file);
        return;
    }

    // Gestion des erreurs de lecture
    if (setjmp(png_jmpbuf(png))) {
        std::cerr << "Erreur : Erreur lors de la lecture de l'image PNG." << std::endl;
        png_destroy_read_struct(&png, &info, NULL);
        fclose(file);
        return;
    }

    // Initialiser la lecture de l'image depuis le fichier
    png_init_io(png, file);
    png_read_info(png, info);

    // Récupérer les dimensions de l'image
    width = static_cast<int>(png_get_image_width(png, info));
    height = static_cast<int>(png_get_image_height(png, info));
    binaryPixels = new bool[width * height];

    // Lire l'image en RGBA (8 bits par canal)
    png_set_expand(png);
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    png_set_gray_to_rgb(png);

    // Allouer de la mémoire pour stocker les binaryPixels
    png_bytep *rowPointers = new png_bytep[height];
    for (int y = 0; y < height; y++) {
        rowPointers[y] = new png_byte[4 * width];
    }

    // Lire les données de l'image
    png_read_image(png, rowPointers);

    // Binariser l'image en binaryPixels noirs (true) ou blancs (false)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            png_bytep pixel = &(rowPointers[y][x * 4]);
            bool isBlack = ((pixel[0] * 0.299 + pixel[1] * 0.587 + pixel[2] * 0.114) < threshold) && (pixel[3] > 0);
            binaryPixels[y * width + x] = isBlack;
        }
    }

    // Nettoyage et libération de la mémoire
    for (int y = 0; y < height; y++) {
        delete[] rowPointers[y];
    }
    delete[] rowPointers;
    png_destroy_read_struct(&png, &info, NULL);
    fclose(file);
}

Image::Image(int width, int height) : width(width), height(height) {
    binaryPixels = new bool[width * height];
    for (int i = 0; i < width * height; ++i) {
        binaryPixels[i] = false;
    }
}

Image::~Image() {
    delete[] binaryPixels;
}

int Image::getWidth() const {
    return width;
}

int Image::getHeight() const {
    return height;
}

bool Image::getPixel(int x, int y) const {
    return binaryPixels[y * width + x];
}

void Image::setPixel(int x, int y, bool value) {
    binaryPixels[y * width + x] = value;
}

void Image::to_ppm(const std::string &path) const {
    std::stringstream ppm;
    ppm << "P3\n" << width << ' ' << height << "\n255\n";
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            auto r = binaryPixels[j * width + i] ? 0 : 255.0;
            auto g = binaryPixels[j * width + i] ? 0 : 255.0;
            auto b = binaryPixels[j * width + i] ? 0 : 255.0;
            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);
            ppm << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
    std::ofstream myfile;
    myfile.open(path);
    myfile << ppm.str();
    myfile.close();
}

std::vector<int> Image::detectHorizontalLines(float threshold) const {
    std::vector<int> lines;
    for (int y = 0; y < height; ++y) {
        int count = 0;
        for (int x = 0; x < width; ++x) {
            if (getPixel(x, y)) { // Si le pixel est noir (true)
                count++;
            }
        }
        if (static_cast<float>(count) > threshold * static_cast<float>(width)) {
            lines.push_back(y);
        }
    }
    return lines;
}

void Image::removeHorizontalLines(float threshold) {
    std::vector<int> lines = detectHorizontalLines(threshold);
    for (int y: lines) {
        for (int x = 0; x < width; ++x) {
            setPixel(x, y, false); // Marquer la ligne comme du fond (blanc)
        }
    }
}

void Image::dilate(int neighborhoodSize) {
    bool *tmp = new bool[width * height];
    for (int i = 0; i < width * height; ++i) {
        tmp[i] = false;
    }

    // Définir le voisinage (fenêtre) pour la dilatation
    int neighborhoodOffset = neighborhoodSize / 2; // Décalage pour centrer la fenêtre

    // Parcourir tous les pixels de l'image
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            // Vérifier si le pixel est noir (true)
            if (getPixel(x, y)) {
                // Parcourir le voisinage autour du pixel
                for (int j = -neighborhoodOffset; j <= neighborhoodOffset; ++j) {
                    int ny = y + j;

                    // Vérifier les limites de l'image
                    if (x >= 0 && x < width && ny >= 0 && ny < height) {
                        tmp[ny * width + x] = true; // Effectuer la dilatation (OR logique)
                    }
                }
            }
        }
    }
    delete[] binaryPixels;
    binaryPixels = tmp;
}

void Image::erode(int neighborhoodSize) {
    bool *tmp = new bool[width * height];
    for (int i = 0; i < width * height; ++i) {
        tmp[i] = false;
    }

    // Définir le voisinage (fenêtre) pour l'érosion
    int neighborhoodOffset = neighborhoodSize / 2; // Décalage pour centrer la fenêtre

    // Parcourir tous les pixels de l'image
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {

            // Vérifier si le pixel est noir (true)
            if (getPixel(x, y)) {
                bool isEroded = true;

                // Parcourir le voisinage autour du pixel
                for (int j = -neighborhoodOffset; j <= neighborhoodOffset; ++j) {
                    int ny = y + j;

                    // Vérifier les limites de l'image
                    if (x >= 0 && x < width && ny >= 0 && ny < height) {
                        if (!getPixel(x, ny)) { // Vérifier si le pixel est blanc (false)
                            isEroded = false;
                        }
                    }
                }
                tmp[y * width + x] = isEroded; // Effectuer l'érosion (AND logique)
            }
        }
    }
    delete[] binaryPixels;
    binaryPixels = tmp;
}

void Image::open(int neighborhoodSize) {
    erode(neighborhoodSize);
    dilate(neighborhoodSize);
}

void Image::close(int neighborhoodSize) {
    dilate(neighborhoodSize);
    erode(neighborhoodSize);
}

Image *Image::rotate(int deg) const {
    auto *rotatedImage = new Image(width, height);
    int cx = width / 2;
    int cy = height / 2;
    // Convertir l'angle en radians
    double angle = deg * M_PI / 180.0f;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int rx = static_cast<int>(std::cos(angle) * static_cast<float>(x - cx) -
                                      std::sin(angle) * static_cast<float>(y - cy) + static_cast<float>(cx));
            int ry = static_cast<int>(std::sin(angle) * static_cast<float>(x - cx) +
                                      std::cos(angle) * static_cast<float>(y - cy) + static_cast<float>(cy));
            if (rx >= 0 && rx < width && ry >= 0 && ry < height) {
                rotatedImage->setPixel(rx, ry, getPixel(x, y));
            }
        }
    }
    return rotatedImage;
}

void Image::align(float threshold) const {
    Image *rotatedImages[21];
    unsigned long max_lines = 0;
    int max_deg = 0;
    for (int deg = -10; deg <= 10; ++deg) {
        rotatedImages[deg + 10] = rotate(deg);
        unsigned long lines = rotatedImages[deg + 10]->detectHorizontalLines(threshold).size();
        if (lines > max_lines) {
            max_lines = lines;
            max_deg = deg;
        }
    }
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            binaryPixels[y * width + x] = rotatedImages[max_deg + 10]->getPixel(x, y);
        }
    }
    for (auto &rotatedImage: rotatedImages) {
        delete rotatedImage;
    }
}

#include "Filters.h"
#include <iostream>

int main(int argc, char** argv) {

    if (argc < 3) {
        std::cout << "not enough arguments\n";
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];

    // Создание объекта изображения
    Image image;

    try {
        // Чтение изображения из файла
        image.Read(input_file);

        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-crop" && i + 2 < argc) {
                int width = std::stoi(argv[++i]);
                int height = std::stoi(argv[++i]);
                Crop crop(width, height);
                crop.ApplyFilter(image);
            } else if (arg == "-gs") {
                Grayscale gs;
                gs.ApplyFilter(image);
            } else if (arg == "-neg") {
                Negative neg;
                neg.ApplyFilter(image);
            } else if (arg == "-sharp") {
                Sharpening sharp;
                sharp.ApplyFilter(image);
            } else if (arg == "-edge") {
                float threshold = std::stof(argv[++i]);
                EdgeDetection edge(threshold);
                edge.ApplyFilter(image);
            } else if (arg == "-blur") {
                float sigma = std::stof(argv[++i]);
                Blur blur(sigma);
                blur.ApplyFilter(image);
            } else if (arg == "-disc") {
                float radius = std::stof(argv[++i]);
                float angle = std::stof(argv[++i]);
                SpinningDisk disk(radius, angle);
                disk.ApplyFilter(image);
            } else {
                std::cerr << "Unknown argument: " << arg << std::endl;
            }
        }

        // Сохранение изображения в другой файл
        SaveFile(output_file, image);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

#include <algorithm>
#include <cmath>
#include <iostream>
#include "Filters.h"
#include <stdexcept>

Crop::Crop() : width_(0), height_(0) {
}

Crop::Crop(int width, int height) {
    try {
        if (width < 0 || height < 0) {
            throw std::invalid_argument("arguments must be non-negative");
        }
        width_ = width;
        height_ = height;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
}

void Crop::ApplyFilter(Image& image) {
    int new_width = std::min(width_, image.Width());
    int new_height = std::min(height_, image.Height());

    Image new_image(new_width, new_height);
    for (int y = 0; y < new_height; ++y) {
        for (int x = 0; x < new_width; ++x) {
            RGB curr_pixel = image.GetRgb(x, y + image.Height() - new_height);
            new_image.ChangePixel(curr_pixel, x, y);
        }
    }
    image = new_image;
}

Crop::~Crop() {
}

void Grayscale::ApplyFilter(Image& image) {
    for (int y = 0; y < image.Height(); ++y) {
        for (int x = 0; x < image.Width(); ++x) {
            RGB curr_pixel = image.GetRgb(x, y);
            const float fc = 0.299f;
            const float sc = 0.587f;
            const float tc = 0.114f;
            RGB new_pixel = RGB(fc * curr_pixel.r_ + sc * curr_pixel.g_ + tc * curr_pixel.b_,
                                fc * curr_pixel.r_ + sc * curr_pixel.g_ + tc * curr_pixel.b_,
                                fc * curr_pixel.r_ + sc * curr_pixel.g_ + tc * curr_pixel.b_);
            image.ChangePixel(new_pixel, x, y);
        }
    }
}

void Negative::ApplyFilter(Image& image) {
    for (int y = 0; y < image.Height(); ++y) {
        for (int x = 0; x < image.Width(); ++x) {
            RGB curr_pixel = image.GetRgb(x, y);
            RGB new_pixel = RGB(1 - curr_pixel.r_, 1 - curr_pixel.g_, 1 - curr_pixel.b_);
            image.ChangePixel(new_pixel, x, y);
        }
    }
}

void Sharpening::ApplyFilter(Image& image) {
    std::vector<std::vector<float> > kernel(3, std::vector<float>(3, 0.0f));
    kernel[0][1] = -1.f;
    kernel[1][0] = -1.f;
    kernel[1][1] = 5.f;  // NOLINT
    kernel[1][2] = -1.f;
    kernel[2][1] = -1.f;

    float factor = 1.f / 2.f;  // NOLINT
    float bias = 0.0;

    for (int y = 0; y < image.Height(); ++y) {
        for (int x = 0; x < image.Width(); ++x) {
            float sum_r = 0.0f;
            float sum_g = 0.0f;
            float sum_b = 0.0f;

            for (int yk = -1; yk <= 1; ++yk) {
                for (int xk = -1; xk <= 1; ++xk) {
                    int pixel_x = std::max(0, std::min(image.Width() - 1, x + xk));
                    int pixel_y = std::max(0, std::min(image.Height() - 1, y + yk));
                    sum_r += kernel[yk + 1][xk + 1] * image.GetRgb(pixel_x, pixel_y).r_;
                    sum_g += kernel[yk + 1][xk + 1] * image.GetRgb(pixel_x, pixel_y).g_;
                    sum_b += kernel[yk + 1][xk + 1] * image.GetRgb(pixel_x, pixel_y).b_;
                }
            }

            RGB new_pixel = RGB(std::min(std::max(factor * sum_r + bias, 0.f), 1.f),
                                std::min(std::max(factor * sum_g + bias, 0.f), 1.f),
                                std::min(std::max(factor * sum_b + bias, 0.f), 1.f));

            image.ChangePixel(new_pixel, x, y);
        }
    }
}

EdgeDetection::EdgeDetection() : threshold_(0) {
}

EdgeDetection::EdgeDetection(float threshold) {
    try {
        if (threshold < 0) {
            throw std::invalid_argument("threshold must be non-negative");
        }
        threshold_ = threshold;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
}

void EdgeDetection::ApplyFilter(Image& image) {
    Grayscale gs;
    gs.ApplyFilter(image);

    std::vector<std::vector<float> > kernel(3, std::vector<float>(3, 0.0f));
    const float four = 4.f;
    kernel[0][1] = -1.f;
    kernel[1][0] = -1.f;
    kernel[1][1] = four;
    kernel[1][2] = -1.f;
    kernel[2][1] = -1.f;

    for (int y = 0; y < image.Height(); ++y) {
        for (int x = 0; x < image.Width(); ++x) {
            float sum_r = 0.0f;
            float sum_g = 0.0f;
            float sum_b = 0.0f;

            for (int yk = -1; yk <= 1; ++yk) {
                for (int xk = -1; xk <= 1; ++xk) {
                    int pixel_x = std::max(0, std::min(image.Width() - 1, x + xk));
                    int pixel_y = std::max(0, std::min(image.Height() - 1, y + yk));
                    sum_r += kernel[yk + 1][xk + 1] * image.GetRgb(pixel_x, pixel_y).r_;
                    sum_g += kernel[yk + 1][xk + 1] * image.GetRgb(pixel_x, pixel_y).g_;
                    sum_b += kernel[yk + 1][xk + 1] * image.GetRgb(pixel_x, pixel_y).b_;
                }
            }

            bool cond = (std::max(0.0f, std::min(1.0f, sum_r)) > threshold_) &&
                        (std::max(0.0f, std::min(1.0f, sum_g)) > threshold_) &&
                        (std::max(0.0f, std::min(1.0f, sum_b)) > threshold_);

            if (cond) {
                image.ChangePixel(RGB(1, 1, 1), x, y);
            } else {
                image.ChangePixel(RGB(0, 0, 0), x, y);
            }
        }
    }
}

EdgeDetection::~EdgeDetection() {
}

Blur::Blur() : sigma_(1.0f) {
}

Blur::Blur(float sigma) {
    try {
        if (sigma <= 0) {
            throw std::invalid_argument("sigma must be positive");
        }
        sigma_ = sigma;
        GenerateGaussianKernel();
    } catch (const std::invalid_argument& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
}

void Blur::ApplyFilter(Image& image) {
    int kernel_size = static_cast<int>(kernel_.size());
    int half_kernel_size = kernel_size / 2;
    int image_width = image.Width();
    int image_height = image.Height();
    Image temp_image(image_width, image_height);

    for (int y = 0; y < image_height; ++y) {
        for (int x = 0; x < image_width; ++x) {
            ApplyBlur(image, temp_image, x, y, half_kernel_size);
        }
    }
    image = temp_image;
}

Blur::~Blur() {
}

void Blur::GenerateGaussianKernel() {
    int kernel_size = CalculateKernelSize();
    float kernel_sum = 0.0;
    kernel_.resize(kernel_size, std::vector<float>(kernel_size));

    for (int y = 0; y < kernel_size; ++y) {
        for (int x = 0; x < kernel_size; ++x) {
            float dx = static_cast<float>(x) - static_cast<float>(kernel_size) / 2;
            float dy = static_cast<float>(y) - static_cast<float>(kernel_size) / 2;
            float weight = std::exp(-(dx * dx + dy * dy) / (2 * sigma_ * sigma_));
            kernel_[y][x] = weight;
            kernel_sum += weight;
        }
    }

    for (int y = 0; y < kernel_size; ++y) {
        for (int x = 0; x < kernel_size; ++x) {
            kernel_[y][x] /= kernel_sum;
        }
    }
}

int Blur::CalculateKernelSize() const {
    const float size_multiplier = 6.0f;
    return std::max(1, static_cast<int>(std::ceil(size_multiplier * sigma_)));
}

void Blur::ApplyBlur(const Image& src_image, Image& dest_image, int x, int y, int half_kernel_size) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;
    int image_width = src_image.Width();
    int image_height = src_image.Height();

    for (int ky = 0; ky < kernel_.size(); ++ky) {
        for (int kx = 0; kx < kernel_.size(); ++kx) {
            int src_x = x + kx - half_kernel_size;
            int src_y = y + ky - half_kernel_size;

            src_x = std::min(std::max(src_x, 0), image_width - 1);
            src_y = std::min(std::max(src_y, 0), image_height - 1);

            RGB pixel = src_image.GetRgb(src_x, src_y);
            float weight = kernel_[ky][kx];
            red += pixel.r_ * weight;
            green += pixel.g_ * weight;
            blue += pixel.b_ * weight;
        }
    }

    RGB new_pixel(red, green, blue);
    dest_image.ChangePixel(new_pixel, x, y);
}

SpinningDisk::SpinningDisk() : radius_(0), angle_(0) {
}

SpinningDisk::SpinningDisk(float radius, float angle) {
    try {
        if (radius < 0 || angle < 0) {
            throw std::invalid_argument("arguments must be non-negative");
        }
        radius_ = radius;
        angle_ = angle;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
}

void SpinningDisk::ApplyFilter(Image& image) {
    int center_x = image.Width() / 2;
    int center_y = image.Height() / 2;

    // Создаем временное изображение для применения искажения
    Image temp_image(image.Width(), image.Height());

    // Применяем искажение к каждому пикселю изображения
    for (int y = 0; y < image.Height(); ++y) {
        for (int x = 0; x < image.Width(); ++x) {
            // Определяем расстояние до центра изображения
            float distance = Distance(x, y, center_x, center_y);

            // Вычисляем угол вращения
            float angle = angle_ * (1 - distance / radius_);

            // Координаты точки после вращения
            int new_x =
                static_cast<int>(cos(angle) * static_cast<float>(x - center_x) -
                                 sin(angle) * static_cast<float>((y - center_y)) + static_cast<float>(center_x));
            int new_y =
                static_cast<int>(sin(angle) * static_cast<float>((x - center_x)) +
                                 cos(angle) * static_cast<float>((y - center_y)) + static_cast<float>(center_y));

            if (new_x >= 0 && new_x < image.Width() && new_y >= 0 && new_y < image.Height()) {
                RGB pixel = image.GetRgb(new_x, new_y);
                temp_image.ChangePixel(pixel, x, y);
            }
        }
    }

    // Копируем временное изображение в исходное
    image = temp_image;
}

SpinningDisk::~SpinningDisk() {
}

float SpinningDisk::Distance(int x, int y, int cx, int cy) const {
    float dx = static_cast<float>(x - cx);
    float dy = static_cast<float>(y - cy);
    return sqrtf(dx * dx + dy * dy);
}

#pragma once

#include "Image.h"

class ImageFilter {
public:
    virtual void ApplyFilter(Image& image) = 0;
    virtual ~ImageFilter() {
    }
};

class Crop : public ImageFilter {
public:
    Crop();
    Crop(int width, int height);
    void ApplyFilter(Image& image) override;
    ~Crop() override;

private:
    int width_;
    int height_;
};

class Grayscale : public ImageFilter {
public:
    void ApplyFilter(Image& image) override;
};

class Negative : public ImageFilter {
public:
    void ApplyFilter(Image& image) override;
};

class Sharpening : public ImageFilter {
public:
    void ApplyFilter(Image& image) override;
};

class EdgeDetection : public ImageFilter {
public:
    EdgeDetection();
    explicit EdgeDetection(float threshold);
    void ApplyFilter(Image& image) override;
    ~EdgeDetection() override;

private:
    float threshold_;
};

class Blur : public ImageFilter {
public:
    Blur();
    explicit Blur(float sigma);
    void ApplyFilter(Image& image) override;
    ~Blur() override;

private:
    float sigma_;
    std::vector<std::vector<float> > kernel_;
    void GenerateGaussianKernel();
    int CalculateKernelSize() const;
    void ApplyBlur(const Image& src_image, Image& dest_image, int x, int y, int half_kernel_size);
};

class SpinningDisk : ImageFilter {
public:
    SpinningDisk();
    explicit SpinningDisk(float radius, float angle);
    void ApplyFilter(Image& image) override;
    ~SpinningDisk() override;

private:
    float radius_;
    float angle_;
    float Distance(int x, int y, int cx, int cy) const;
};

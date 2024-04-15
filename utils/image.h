#pragma once
#include "exceptions.h"

#include <cmath>
#include <cstdint>
#include <tuple>
#include <vector>
#include <iostream>
#include <iomanip>

struct Pixel {
private:
    const double epsilon_ = 1e-2;

    void SetColor(double r, double g, double b) {
        this->r = std::max(0.0, std::min(1.0, r));
        this->g = std::max(0.0, std::min(1.0, g));
        this->b = std::max(0.0, std::min(1.0, b));
    }

public:
    const double max_color = 255.0;

    double r;
    double g;
    double b;

    Pixel() : r(0), g(0), b(0){};

    Pixel(double r, double g, double b) {
        SetColor(r, g, b);
    };

    Pixel(uint8_t r, uint8_t g, uint8_t b) {
        SetColor(static_cast<double>(r) / max_color, static_cast<double>(g) / max_color,
                 static_cast<double>(b) / max_color);
    };

    explicit Pixel(const std::tuple<double, double, double>& colors) {
        const auto [r, g, b] = colors;
        SetColor(r, g, b);
    }

    explicit Pixel(const std::tuple<uint8_t, uint8_t, uint8_t>& colors) {
        const auto [r, g, b] = colors;
        SetColor(r / max_color, g / max_color, b / max_color);
    }

    std::tuple<uint8_t, uint8_t, uint8_t> ToRGB() const {
        return std::make_tuple(static_cast<uint8_t>(r * max_color), static_cast<uint8_t>(g * max_color),
                               static_cast<uint8_t>(b * max_color));
    }

    std::tuple<double, double, double> Tuple() const {
        return std::make_tuple(r, g, b);
    }

    Pixel& operator=(const Pixel& other) {
        SetColor(other.r, other.g, other.b);
        return *this;
    }

    Pixel& operator+=(const Pixel& other) {
        SetColor(this->r + other.r, this->g + other.g, this->b + other.b);
        return *this;
    }

    template <typename T>
    Pixel& operator*=(T multuplier) {
        SetColor(this->r * multuplier, this->g * multuplier, this->b * multuplier);
        return *this;
    }

    template <typename T>
    Pixel& operator*=(const std::tuple<T, T, T>& rhs) {
        SetColor(this->r * std::get<0>(rhs), this->g * std::get<1>(rhs), this->b * std::get<2>(rhs));
        return *this;
    }

    bool operator==(const Pixel& rhs) const {
        return std::abs(r - rhs.r) < epsilon_ && std::abs(g - rhs.g) < epsilon_ && std::abs(b - rhs.b) < epsilon_;
    }

    bool operator!=(Pixel& rhs) const {
        return !(*this == rhs);
    }

    friend Pixel operator+(Pixel lhs, const Pixel& rhs) {
        lhs += rhs;
        return lhs;
    }

    template <typename T>
    friend Pixel operator*(Pixel lhs, T rhs) {
        lhs *= rhs;
        return lhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const Pixel& pixel) {
        auto [r, g, b] = pixel.ToRGB();
        os << "(" << r << ", " << g << ", " << b << ")";
        return os;
    }
};

class Image {
private:
    int64_t height_;
    int64_t width_;

    size_t horizontal_resolution_;
    size_t vertical_resolution_;

    std::vector<std::vector<Pixel>> pixels_;

public:
    Image() : height_(0), width_(0), horizontal_resolution_(0), vertical_resolution_(0) {
    }

    Image(int64_t height, int64_t width, size_t horizontal_resolution, size_t vertical_resolution)
        : height_(height),
          width_(width),
          horizontal_resolution_(horizontal_resolution),
          vertical_resolution_(vertical_resolution) {
        pixels_.resize(height);

        for (auto& row : pixels_) {
            row.resize(width);
        }
    }

    explicit Image(const std::vector<std::vector<Pixel>>& img)
        : height_(static_cast<int64_t>(img.size())), width_(static_cast<int64_t>(img[0].size())) {
        pixels_.resize(img.size());

        for (size_t i = 0; i != img.size(); ++i) {
            std::copy(img[i].begin(), img[i].end(), std::back_inserter(pixels_[i]));
        }
    }

    void Reshape(int64_t new_height, int64_t new_width) {
        height_ = new_height;
        width_ = new_width;

        pixels_.resize(new_height);

        for (auto& row : pixels_) {
            row.resize(new_width);
        }
    }

    Pixel Get(int64_t i, int64_t j) const {
        return pixels_[std::min(static_cast<int64_t>(height_) - 1, std::max(0l, i))]
                      [std::min(static_cast<int64_t>(width_) - 1, std::max(0l, j))];
    }

    Pixel& Get(int64_t i, int64_t j) {
        return pixels_[std::min(static_cast<int64_t>(height_) - 1, std::max(0l, i))]
                      [std::min(static_cast<int64_t>(width_) - 1, std::max(0l, j))];
    }

    std::vector<std::vector<Pixel>>& GetPixels() {
        return pixels_;
    }

    std::tuple<int64_t, int64_t> Shape() const {
        return std::make_tuple(height_, width_);
    }

    void SetResolution(size_t horizontal_resolution, size_t vertical_resolution) {
        horizontal_resolution_ = horizontal_resolution;
        vertical_resolution_ = vertical_resolution;
    }

    std::tuple<size_t, size_t> Resolution() const {
        return std::make_tuple(horizontal_resolution_, vertical_resolution_);
    }
};
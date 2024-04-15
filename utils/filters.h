#pragma once

#include "image.h"
#include "exceptions.h"
#include "math.h"

#include <string>
#include <vector>
#include <tuple>
#include <queue>

class AbstractFilter {
public:
    static const std::string ALIAS;

    virtual void Apply(Image& img, std::queue<std::string> parameters) const {
        throw NotImplementedError{};
    }

    virtual ~AbstractFilter() = default;
};

class AbstractMatrixFilter : public AbstractFilter {
private:
    static const std::vector<std::vector<int16_t>> FILTER_MATRIX;

protected:
    AbstractMatrixFilter() {
    }

    std::tuple<double, double, double> ApplyMatrix(Image& img, int32_t y, int32_t x,
                                                   const std::vector<std::vector<int16_t>>& matrix) const;
};

class CropFilter : public AbstractFilter {
public:
    static const std::string ALIAS;

    void Apply(Image& img, std::queue<std::string> parameters) const override;
};

class GrayscaleFilter : public AbstractFilter {
private:
    static const std::tuple<double, double, double> COEFS;

public:
    static const std::string ALIAS;

    void Apply(Image& img, std::queue<std::string> parameters) const override;
};

class NegativeFilter : public AbstractFilter {
public:
    static const std::string ALIAS;

    void Apply(Image& img, std::queue<std::string> parameters) const override;
};

class SharpeningFilter : public AbstractMatrixFilter {
private:
    static const std::vector<std::vector<int16_t>> FILTER_MATRIX;

public:
    static const std::string ALIAS;

    void Apply(Image& img, std::queue<std::string> parameters) const override;
};

class EdgeDetectionFilter : public AbstractMatrixFilter {
private:
    static const std::vector<std::vector<int16_t>> FILTER_MATRIX;

public:
    static const std::string ALIAS;

    void Apply(Image& img, std::queue<std::string> parameters) const override;
};

class GaussianBlurFilter : public AbstractFilter {
private:
    enum class BlurDirection { horizontal, vertical };

    void ApplyOneWayBlur(Image& img, const std::vector<double> gaussian_coefficients, BlurDirection direction) const;

public:
    static const std::string ALIAS;

    static std::vector<double> CalculateGaussianCoefficients(double sigma);

    void Apply(Image& img, std::queue<std::string> parameters) const override;
};

#include "../utils/filters.h"

std::tuple<double, double, double> AbstractMatrixFilter::ApplyMatrix(
    Image& img, int32_t y, int32_t x, const std::vector<std::vector<int16_t>>& matrix) const {
    std::tuple<double, double, double> result;

    for (size_t i = 0; i != matrix.size(); ++i) {
        for (size_t j = 0; j != matrix[i].size(); ++j) {
            Pixel cur = img.Get(static_cast<int64_t>(y - 1 + i), static_cast<int64_t>(x - 1 + j));

            result =
                std::make_tuple(std::get<0>(result) + cur.r * matrix[j][i], std::get<1>(result) + cur.g * matrix[j][i],
                                std::get<2>(result) + cur.b * matrix[j][i]);
        }
    }

    return result;
}

const std::string CropFilter::ALIAS = "-crop";

void CropFilter::Apply(Image& img, std::queue<std::string> parameters) const {
    if (parameters.size() != 2) {
        throw InvalidFilterParametersError{"crop"};
    }

    auto [height, width] = img.Shape();
    int64_t new_height = height;
    int64_t new_width = width;

    try {
        new_width = std::stol(parameters.front());
        parameters.pop();
        new_height = std::stol(parameters.front());
        parameters.pop();
    } catch (const std::invalid_argument& e) {
        throw InvalidFilterParametersError{"crop"};
    }

    if (new_height < 0 || new_width < 0) {
        throw InvalidFilterParametersError{"crop"};
    }

    img.Reshape(std::min(height, new_height), std::min(width, new_width));
}

const std::string GrayscaleFilter::ALIAS = "-gs";
const std::tuple<double, double, double> GrayscaleFilter::COEFS = {0.299, 0.587, 0.114};

void GrayscaleFilter::Apply(Image& img, std::queue<std::string> parameters) const {
    if (!parameters.empty()) {
        throw InvalidFilterParametersError{"grayscale"};
    }

    auto [height, width] = img.Shape();

    for (int64_t i = 0; i != height; ++i) {
        for (int64_t j = 0; j != width; ++j) {
            Pixel multiplied = img.Get(i, j) * COEFS;
            double new_color = multiplied.r + multiplied.g + multiplied.b;

            img.Get(i, j) = Pixel(new_color, new_color, new_color);
        }
    }
}

const std::string NegativeFilter::ALIAS = "-neg";

void NegativeFilter::Apply(Image& img, std::queue<std::string> parameters) const {
    if (!parameters.empty()) {
        throw InvalidFilterParametersError{"negative"};
    }

    auto [height, width] = img.Shape();

    for (int64_t i = 0; i != height; ++i) {
        for (int64_t j = 0; j != width; ++j) {
            Pixel& current_pixel = img.Get(i, j);

            current_pixel.r = 1 - current_pixel.r;
            current_pixel.g = 1 - current_pixel.g;
            current_pixel.b = 1 - current_pixel.b;
        }
    }
}

const std::string SharpeningFilter::ALIAS = "-sharp";
const std::vector<std::vector<int16_t>> SharpeningFilter::FILTER_MATRIX = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};

void SharpeningFilter::Apply(Image& img, std::queue<std::string> parameters) const {
    if (!parameters.empty()) {
        throw InvalidFilterParametersError{"sharpening"};
    }

    auto [height, width] = img.Shape();
    std::vector<std::vector<Pixel>> new_data(height, std::vector<Pixel>(width));

    for (int64_t i = 0; i != height; ++i) {
        for (int64_t j = 0; j != width; ++j) {
            new_data[i][j] =
                Pixel(this->ApplyMatrix(img, static_cast<int32_t>(i), static_cast<int32_t>(j), FILTER_MATRIX));
        }
    }

    img.GetPixels() = std::move(new_data);
}

const std::string EdgeDetectionFilter::ALIAS = "-edge";
const std::vector<std::vector<int16_t>> EdgeDetectionFilter::FILTER_MATRIX = {{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}};

void EdgeDetectionFilter::Apply(Image& img, std::queue<std::string> parameters) const {
    if (parameters.size() != 1) {
        throw InvalidFilterParametersError{"edge detection"};
    }

    double threshold = NAN;
    try {
        threshold = std::stod(parameters.front());
        parameters.pop();
    } catch (const std::invalid_argument& e) {
        throw InvalidFilterParametersError{"edge detection"};
    }

    if (threshold < 0 || threshold > 1) {
        throw InvalidFilterParametersError{"edge detection"};
    }

    auto [height, width] = img.Shape();

    GrayscaleFilter().Apply(img, parameters);

    std::vector<std::vector<Pixel>> new_data(height, std::vector<Pixel>(width));

    for (int64_t i = 0; i != height; ++i) {
        for (int64_t j = 0; j != width; ++j) {
            auto new_color = this->ApplyMatrix(img, static_cast<int32_t>(i), static_cast<int32_t>(j), FILTER_MATRIX);

            if (std::get<0>(new_color) >= threshold) {
                new_data[i][j] = Pixel(1., 1., 1.);
            } else {
                new_data[i][j] = Pixel(0., 0., 0.);
            }
        }
    }

    img.GetPixels() = std::move(new_data);
}

const std::string GaussianBlurFilter::ALIAS = "-blur";

std::vector<double> GaussianBlurFilter::CalculateGaussianCoefficients(double sigma) {
    const int normal_distribution_radius = 6;

    std::vector<double> coefficients(static_cast<size_t>(std::ceil(sigma)) * normal_distribution_radius + 1);
    size_t center = (coefficients.size() - 1) / 2;

    for (size_t i = 0; i != coefficients.size(); ++i) {
        double exponent_power_numerator = std::pow(abs(static_cast<int32_t>(center) - static_cast<int32_t>(i)), 2);
        double exponent_power_denominator = 2 * std::pow(sigma, 2);

        coefficients[i] = (1 / (std::sqrt(2 * M_PI) * sigma)) *
                          std::pow(M_E, -(exponent_power_numerator / exponent_power_denominator));
    }

    return coefficients;
}

void GaussianBlurFilter::ApplyOneWayBlur(Image& img, const std::vector<double> gaussian_coefficients,
                                         BlurDirection direction) const {
    auto [height, width] = img.Shape();
    int64_t radius = static_cast<int64_t>(gaussian_coefficients.size() - 1) / 2;
    std::vector<std::vector<Pixel>> new_data(height, std::vector<Pixel>(width));

    for (int64_t i = 0; i != height; ++i) {
        for (int64_t j = 0; j != width; ++j) {
            double red = 0;
            double green = 0;
            double blue = 0;

            for (size_t k = 0; k != gaussian_coefficients.size(); ++k) {
                Pixel current_pixel;

                if (direction == BlurDirection::horizontal) {
                    current_pixel = img.Get(i, static_cast<int64_t>(j - radius + k));
                } else {
                    current_pixel = img.Get(static_cast<int64_t>(i - radius + k), j);
                }

                red += current_pixel.r * gaussian_coefficients[k];
                green += current_pixel.g * gaussian_coefficients[k];
                blue += current_pixel.b * gaussian_coefficients[k];
            }

            new_data[i][j] = Pixel(red, green, blue);
        }
    }

    img.GetPixels() = std::move(new_data);
}

void GaussianBlurFilter::Apply(Image& img, std::queue<std::string> parameters) const {
    if (parameters.size() != 1) {
        throw InvalidFilterParametersError{"blur"};
    }

    double sigma = NAN;
    try {
        sigma = std::stod(parameters.front());
        parameters.pop();
    } catch (const std::invalid_argument& e) {
        throw InvalidFilterParametersError{"blur"};
    }

    if (sigma < 0) {
        throw InvalidFilterParametersError{"blur"};
    }

    std::vector<double> gaussian_coefficients = CalculateGaussianCoefficients(sigma);
    ApplyOneWayBlur(img, gaussian_coefficients, BlurDirection::horizontal);  // true - horizontal blur
    ApplyOneWayBlur(img, gaussian_coefficients, BlurDirection::vertical);    // false - vertical blur
}

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <filesystem>

#include "utils/exceptions.h"
#include "utils/filters.h"
#include "utils/bmp_reader.h"
#include "utils/console_interface.h"

namespace {
const std::filesystem::path TEST_PATH = "../tasks/image_processor/test_script/data";

bool ComparePixelwise(const Image& img1, const Image& img2) {
    auto [height, width] = img1.Shape();

    for (int64_t i = 0; i < height; ++i) {
        for (int64_t j = 0; j < width; ++j) {
            if (img1.Get(i, j) != img2.Get(i, j)) {
                return false;
            }
        }
    }

    return true;
}
}  // namespace

TEST_CASE("Crop filter test") {
    Image* img = nullptr;
    img = bmp_reader::ReadFile(TEST_PATH / "lenna.bmp", img);
    AbstractFilter* filter_to_check = new CropFilter();

    // test wrong parameters
    std::queue<std::string> parameters;
    parameters.push("100");
    REQUIRE_THROWS(filter_to_check->Apply(*img, parameters), InvalidFilterParametersError{"crop"});
    console_interface::Clear(parameters);

    parameters.push("100");
    parameters.push("-100");
    REQUIRE_THROWS(filter_to_check->Apply(*img, parameters), InvalidFilterParametersError{"crop"});
    console_interface::Clear(parameters);

    parameters.push("abacaba");
    REQUIRE_THROWS(filter_to_check->Apply(*img, parameters), InvalidFilterParametersError{"crop"});
    console_interface::Clear(parameters);

    // test correct parameters

    // test applied once
    parameters.push("999");
    parameters.push("1999");
    filter_to_check->Apply(*img, parameters);
    console_interface::Clear(parameters);

    Image* correct = nullptr;
    correct = bmp_reader::ReadFile(TEST_PATH / "lenna_crop.bmp", correct);

    REQUIRE(correct->Shape() == img->Shape());
    REQUIRE(ComparePixelwise(*img, *correct));

    // test applied twice
    parameters.push("100");
    parameters.push("1");
    filter_to_check->Apply(*img, parameters);
    console_interface::Clear(parameters);

    delete correct;
    correct = bmp_reader::ReadFile(TEST_PATH / "lenna_crop_crop.bmp", correct);

    REQUIRE(correct->Shape() == img->Shape());
    REQUIRE(ComparePixelwise(*img, *correct));

    delete img;
    delete correct;
    delete filter_to_check;
}

TEST_CASE("Grayscale filter test") {
    Image* img = nullptr;
    img = bmp_reader::ReadFile(TEST_PATH / "lenna.bmp", img);
    AbstractFilter* filter_to_check = new GrayscaleFilter();

    // test wrong parameters
    std::queue<std::string> parameters;
    parameters.push("100");
    REQUIRE_THROWS(filter_to_check->Apply(*img, parameters), InvalidFilterParametersError{"grayscale"});
    console_interface::Clear(parameters);

    // test correct parameters
    // filter applied once
    filter_to_check->Apply(*img, parameters);

    Image* correct = nullptr;
    correct = bmp_reader::ReadFile(TEST_PATH / "lenna_gs.bmp", correct);

    REQUIRE(correct->Shape() == img->Shape());
    REQUIRE(ComparePixelwise(*img, *correct));

    // filter applied twice
    filter_to_check->Apply(*img, parameters);
    delete correct;
    correct = bmp_reader::ReadFile(TEST_PATH / "lenna_gs_gs.bmp", correct);

    REQUIRE(ComparePixelwise(*img, *correct));

    delete img;
    delete correct;
    delete filter_to_check;
}

TEST_CASE("Negative filter test") {
    Image* img = nullptr;
    img = bmp_reader::ReadFile(TEST_PATH / "lenna.bmp", img);
    AbstractFilter* filter_to_check = new NegativeFilter();

    // test wrong parameters
    std::queue<std::string> parameters;
    parameters.push("20");
    REQUIRE_THROWS(filter_to_check->Apply(*img, parameters), InvalidFilterParametersError{"negative"});
    console_interface::Clear(parameters);

    // test correct parameters
    // filter applied once
    filter_to_check->Apply(*img, parameters);

    Image* correct = nullptr;
    correct = bmp_reader::ReadFile(TEST_PATH / "lenna_neg.bmp", correct);

    REQUIRE(correct->Shape() == img->Shape());
    REQUIRE(ComparePixelwise(*img, *correct));

    // filter applied twice
    filter_to_check->Apply(*img, parameters);
    delete correct;
    correct = bmp_reader::ReadFile(TEST_PATH / "lenna_neg_neg.bmp", correct);

    REQUIRE(ComparePixelwise(*img, *correct));

    delete img;
    delete correct;
    delete filter_to_check;
}

TEST_CASE("Sharpening filter test") {
    Image* img = nullptr;
    img = bmp_reader::ReadFile(TEST_PATH / "lenna.bmp", img);
    AbstractFilter* filter_to_check = new SharpeningFilter();

    // test wrong parameters
    std::queue<std::string> parameters;
    parameters.push("100");
    REQUIRE_THROWS(filter_to_check->Apply(*img, parameters), InvalidFilterParametersError{"sharpening"});
    console_interface::Clear(parameters);

    // test correct parameters
    // filter applied once
    filter_to_check->Apply(*img, parameters);

    Image* correct = nullptr;
    correct = bmp_reader::ReadFile(TEST_PATH / "lenna_sharp.bmp", correct);

    REQUIRE(correct->Shape() == img->Shape());
    REQUIRE(ComparePixelwise(*img, *correct));

    // filter applied twice
    filter_to_check->Apply(*img, parameters);
    delete correct;
    correct = bmp_reader::ReadFile(TEST_PATH / "lenna_sharp_sharp.bmp", correct);

    REQUIRE(ComparePixelwise(*img, *correct));

    delete img;
    delete correct;
    delete filter_to_check;
}

TEST_CASE("Edge detection filter test") {
    Image* img = nullptr;
    img = bmp_reader::ReadFile(TEST_PATH / "flag.bmp", img);
    AbstractFilter* filter_to_check = new EdgeDetectionFilter();

    // test wrong parameters
    std::queue<std::string> parameters;
    // wrong parameter range
    parameters.push("100");
    REQUIRE_THROWS(filter_to_check->Apply(*img, parameters), InvalidFilterParametersError{"edge detection"});
    console_interface::Clear(parameters);

    // wrong number of parameters
    parameters.push("100");
    parameters.push("200");
    REQUIRE_THROWS(filter_to_check->Apply(*img, parameters), InvalidFilterParametersError{"edge detection"});
    console_interface::Clear(parameters);

    // test correct parameters
    // filter applied once
    parameters.push("0.1");
    filter_to_check->Apply(*img, parameters);
    console_interface::Clear(parameters);

    Image* correct = nullptr;
    correct = bmp_reader::ReadFile(TEST_PATH / "flag_edge.bmp", correct);

    REQUIRE(correct->Shape() == img->Shape());
    REQUIRE(ComparePixelwise(*img, *correct));

    // filter applied twice
    parameters.push("0.1");
    filter_to_check->Apply(*img, parameters);

    correct = bmp_reader::ReadFile(TEST_PATH / "flag_edge_edge.bmp", correct);

    REQUIRE(correct->Shape() == img->Shape());
    REQUIRE(ComparePixelwise(*img, *correct));

    delete img;
    delete correct;
    delete filter_to_check;
}

TEST_CASE("Gaussian blur filter test") {
    Image* img = nullptr;
    img = bmp_reader::ReadFile(TEST_PATH / "lenna.bmp", img);
    AbstractFilter* filter_to_check = new GaussianBlurFilter();

    // wrong number of parameters
    std::queue<std::string> parameters;
    parameters.push("100");
    parameters.push("0.5");
    REQUIRE_THROWS(filter_to_check->Apply(*img, parameters), InvalidFilterParametersError{"blur"});
    console_interface::Clear(parameters);

    parameters.push("-0.5");
    REQUIRE_THROWS(filter_to_check->Apply(*img, parameters), InvalidFilterParametersError{"blur"});
    console_interface::Clear(parameters);

    // test correct parameters
    // filter applied once
    parameters.push("7.5");
    filter_to_check->Apply(*img, parameters);

    Image* correct = nullptr;
    correct = bmp_reader::ReadFile(TEST_PATH / "lenna_blur.bmp", correct);

    REQUIRE(correct->Shape() == img->Shape());
    REQUIRE(ComparePixelwise(*img, *correct));

    delete img;
    delete correct;
    delete filter_to_check;
}
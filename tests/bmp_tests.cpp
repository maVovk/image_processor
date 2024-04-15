#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <filesystem>

#include "utils/bmp_reader.h"

TEST_CASE("bmp_reader::ByteRead test") {
    uint8_t array[5]{0x28, 0x00, 0x13, 0x0b, 0x10};  // NOLINT

    REQUIRE(40 == bmp_reader::ByteRead(array, 0, 2));       // NOLINT
    REQUIRE(1051411 == bmp_reader::ByteRead(array, 2, 3));  // NOLINT
}

TEST_CASE("bmp_reader::ByteWrite test") {
    uint8_t array[5];                                           // NOLINT
    std::vector<uint8_t> correct{0x96, 0x0, 0x3d, 0xbb, 0x02};  // NOLINT
    bmp_reader::ByteWrite(array, 150, 0, 2);                    // NOLINT
    bmp_reader::ByteWrite(array, 179005, 2, 3);                 // NOLINT

    REQUIRE_THAT(std::vector<uint8_t>(array, array + 5), Catch::Matchers::Equals(correct));
}

TEST_CASE("bmp_reader::ReadFile test") {  // NOLINT
    Image* test = nullptr;
    std::filesystem::path test_path = "../tasks/image_processor/test_script/data";

    // test exceptions
    REQUIRE_THROWS(bmp_reader::ReadFile("", test), FileNotFoundError{});
    REQUIRE_THROWS(bmp_reader::ReadFile(test_path / "cat-sus.png", test),
                   UnsupportedFileFormat{test_path / "cat-sus.png"});
    REQUIRE_THROWS(bmp_reader::ReadFile(test_path / "empty.bmp", test), FileNotFoundError{});

    // test correct reading
    test = bmp_reader::ReadFile(test_path / "flag.bmp", test);

    REQUIRE(std::make_tuple(20, 10) == test->Shape());       // NOLINT
    REQUIRE(Pixel(0., 0., 187. / 255.) == test->Get(0, 0));  // NOLINT
    REQUIRE(Pixel(1., 0., 0.) == test->Get(4, 4));           // NOLINT

    delete test;

    test = bmp_reader::ReadFile(test_path / "lenna.bmp", test);

    REQUIRE(std::make_tuple(2048, 2048) == test->Shape());                         // NOLINT
    REQUIRE(std::make_tuple(11811, 11811) == test->Resolution());                  // NOLINT
    REQUIRE(Pixel(227. / 255., 138. / 255., 111. / 255.) == test->Get(0, 0));      // NOLINT
    REQUIRE(Pixel(233. / 255., 139. / 255., 109. / 255.) == test->Get(200, 200));  // NOLINT
}

TEST_CASE("bmp_reader::WriteFile test") {
    std::filesystem::path test_path = "../tasks/image_processor/test_script/data";
    Image* test = new Image{5, 5, 100, 456};  // NOLINT

    std::vector<std::vector<Pixel>> bitmap{
        {Pixel(1., 1., 1.), Pixel(0.5, 0.5, 0.5), Pixel(0., 0., 0.), Pixel(0.5, 0.5, 0.5),                    // NOLINT
         Pixel(1., 1., 1.)},                                                                                  // NOLINT
        {Pixel(1., 0., 1.), Pixel(0., 0.5, 0.), Pixel(0., 0.5, 0.), Pixel(0.5, 0., 0.5), Pixel(0., 1., 0.)},  // NOLINT
        {Pixel(0., 0., 1.), Pixel(0., 0.5, 0.5), Pixel(0.5, 0.5, 0.), Pixel(0., 0., 0.5),                     // NOLINT
         Pixel(0., 160. / 255., 0.)},                                                                         // NOLINT
        {Pixel(0., 0., 1.), Pixel(0., 0.5, 0.5), Pixel(0.5, 0.5, 0.), Pixel(0., 0., 0.5),                     // NOLINT
         Pixel(0., 160. / 255., 0.)},                                                                         // NOLINT
        {Pixel(0., 0., 0.), Pixel(0., 0., 0.), Pixel(0., 0., 0.), Pixel(0., 0., 0.), Pixel(0., 0., 0.)}};     // NOLINT
    test->GetPixels() = std::move(bitmap);

    // test saving file
    bmp_reader::SaveFile(test_path / "test.bmp", test);

    delete test;

    test = bmp_reader::ReadFile(test_path / "test.bmp", test);

    REQUIRE(std::make_tuple(5, 5) == test->Shape());           // NOLINT
    REQUIRE(std::make_tuple(100, 456) == test->Resolution());  // NOLINT
    REQUIRE(Pixel(0., 160. / 255., 0.) == test->Get(2, 4));    // NOLINT
    REQUIRE(Pixel(0., 0.5, 0.5) == test->Get(3, 1));           // NOLINT

    delete test;
}
#include "../utils/bmp_reader.h"

namespace {
const uint8_t BYTE = 8;
const uint8_t FILE_HEADER_SIZE = 14;
const uint8_t DIB_HEADER_SIZE = 40;

enum FIELDS_OFFSET {
    application_specific = 6,
    bitmap_offset = 10,
    width = 4,
    height = 8,
    color_planes = 12,
    color_depth = 14,
    compression = 16,
    bitmap_size = 20,
    horizontal_resolution = 24,
    vertical_resolution = 28,
    color_pallete_size = 32,
    important_color = 36
};
}  // namespace

uint32_t bmp_reader::ByteRead(uint8_t* array, size_t start, size_t length) {
    uint32_t result = 0;

    for (size_t i = 0; i != length; ++i) {
        result += array[start + i] << (BYTE * i);
    }

    return result;
}

template <typename T>
void bmp_reader::ByteWrite(uint8_t* array, const T& data, size_t start, size_t length) {
    for (size_t i = 0; i != length; ++i) {
        array[start + i] = data >> (BYTE * i);
    }
}

Image* bmp_reader::ReadFile(const std::string& file_path, Image* img) {
    std::ifstream f;
    f.open(file_path, std::ios::in | std::ios::binary);

    if (!f.is_open()) {
        throw FileNotFoundError{};
    }

    uint8_t file_header[FILE_HEADER_SIZE];
    f.read(reinterpret_cast<char*>(file_header), FILE_HEADER_SIZE);
    size_t bitmap_offset = ByteRead(file_header, FIELDS_OFFSET::bitmap_offset, 4);

    if ((file_header[0] != 'B') || (file_header[1] != 'M')) {
        throw UnsupportedFileFormat{file_path};
    }
    if (bitmap_offset != FILE_HEADER_SIZE + DIB_HEADER_SIZE) {
        throw UnsupportedFileFormat{"Not 54 bytes header"};
    }

    uint8_t information_header[DIB_HEADER_SIZE];
    f.read(reinterpret_cast<char*>(information_header), DIB_HEADER_SIZE);

    int64_t width = ByteRead(information_header, FIELDS_OFFSET::width, 4);
    int64_t height = ByteRead(information_header, FIELDS_OFFSET::height, 4);
    size_t bits_per_pixel = ByteRead(information_header, FIELDS_OFFSET::color_depth, 2);
    uint32_t horizontal_resolution = ByteRead(information_header, FIELDS_OFFSET::horizontal_resolution, 4);
    uint32_t vertical_resolution = ByteRead(information_header, FIELDS_OFFSET::vertical_resolution, 4);

    if (bits_per_pixel != 3 * BYTE) {
        throw UnsupportedFileFormat{std::to_string(bits_per_pixel) + " bits color"};
    }

    img = new Image(height, width, horizontal_resolution, vertical_resolution);

    const uint16_t padding = (4 - (3 * width % 4)) % 4;

    for (int64_t i = 0; i != height; ++i) {
        for (int64_t j = 0; j != width; ++j) {
            uint8_t rgb[3];
            f.read(reinterpret_cast<char*>(rgb), 3);

            img->Get(height - i - 1, j) = Pixel(rgb[2], rgb[1], rgb[0]);
        }

        f.ignore(padding);
    }

    f.close();
    return img;
}

void bmp_reader::SaveFile(const std::string& output_path, Image* img) {
    std::fstream f;
    f.open(output_path, std::ios::out | std::ios::binary);

    if (!f.is_open()) {
        throw FileCreationError{};
    }

    auto [height, width] = img->Shape();
    auto [horizontal_resolution, vertical_resolution] = img->Resolution();

    const uint16_t padding = (4 - (3 * width % 4)) % 4;
    const uint32_t bitmap_offset = FILE_HEADER_SIZE + DIB_HEADER_SIZE;
    const uint32_t bitmap_size = 3 * height * width + height * padding;
    const uint32_t file_size = FILE_HEADER_SIZE + DIB_HEADER_SIZE + bitmap_size;

    uint8_t file_header[FILE_HEADER_SIZE];
    file_header[0] = 'B';
    file_header[1] = 'M';

    ByteWrite(file_header, file_size, 2, 4);                            // writing file size in .bmp header
    ByteWrite(file_header, 0, FIELDS_OFFSET::application_specific, 4);  // unused application specific bytes
    ByteWrite(file_header, bitmap_offset, FIELDS_OFFSET::bitmap_offset,
              4);  // writing offset where the pixel array starting

    uint8_t dib_header[DIB_HEADER_SIZE];

    ByteWrite(dib_header, DIB_HEADER_SIZE, 0, 4);
    ByteWrite(dib_header, width, FIELDS_OFFSET::width, 4);
    ByteWrite(dib_header, height, FIELDS_OFFSET::height, 4);
    ByteWrite(dib_header, 1, FIELDS_OFFSET::color_planes, 2);        // Number of color planes
    ByteWrite(dib_header, 3 * BYTE, FIELDS_OFFSET::color_depth, 2);  // Color depth, 24 bits
    ByteWrite(dib_header, 0, FIELDS_OFFSET::compression, 4);         // No compession used
    ByteWrite(dib_header, bitmap_size, FIELDS_OFFSET::bitmap_size, 4);
    ByteWrite(dib_header, horizontal_resolution, FIELDS_OFFSET::horizontal_resolution, 4);
    ByteWrite(dib_header, vertical_resolution, FIELDS_OFFSET::vertical_resolution, 4);
    ByteWrite(dib_header, 0, FIELDS_OFFSET::color_pallete_size, 4);  // Number of colors in the pallete
    ByteWrite(dib_header, 0, FIELDS_OFFSET::important_color, 4);     // Number of important colors

    f.write(reinterpret_cast<char*>(file_header), FILE_HEADER_SIZE);
    f.write(reinterpret_cast<char*>(dib_header), DIB_HEADER_SIZE);

    uint8_t bmp_padding[3] = {0, 0, 0};
    for (int64_t i = 0; i != height; ++i) {
        for (int64_t j = 0; j != width; ++j) {
            auto [r, g, b] = img->Get(static_cast<int64_t>(height - i - 1), j).ToRGB();
            uint8_t color[] = {b, g, r};

            f.write(reinterpret_cast<char*>(color), 3);
        }

        f.write(reinterpret_cast<char*>(bmp_padding), padding);
    }

    f.close();
}
